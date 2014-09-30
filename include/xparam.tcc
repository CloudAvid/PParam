/**
 * \file xparam.tpp
 * defines common parameter structures for managing/manipulating them.
 *
 * Our focus is on parameters of virtual machines to read, write, process,...
 * them from config repository.
 *
 * Copyright 2010 PDNSoft Co. (www.pdnsoft.com)
 * \author hamid jafarian (hamid.jafarian\pdnsoft.com)
 *
 * xparam is part of PParam.
 *
 * PParam is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PParam is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PParam.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdexcept>

namespace pparam
{

/* Implementation of "XMixParam" Class.
 */
template<typename List>
_XMixParam<List>::_XMixParam(const string& _pname) :
	XParam(_pname)
{
	//xmap = NULL;
}

template<typename List>
_XMixParam<List>::_XMixParam(_XMixParam &&_xmp) : XParam(std::move(_xmp)),
					dbengine(_xmp.dbengine)
{ 
	/* We cant move params, because XMixParam is mix of some fixed
	 * parameters.
	 */
}

template<typename List>
XParam *_XMixParam<List>::value(int index) const 
{ 
	int i = 0;
	for (const_iterator iter = const_begin(); 
				iter != const_end(); ++iter, ++i) {
		if (i == index) return *iter;
	}
	return NULL;	
}

template<typename List>
XParam* _XMixParam<List>::value(string name) const
{

	for (const_iterator iter = const_begin(); 
				iter != const_end(); ++iter) {
		if ((*iter)->get_pname() == name)
			return *iter;
	}
	return NULL;
}

template<typename List>
XParam& _XMixParam<List>::operator =(const XmlNode* node) throw (Exception)
{
	if (!is_myNode(node))
		return *this;

	for (iterator iter = params.begin(); iter != params.end(); ++iter) {
		XParam *child = *iter;
		const XParam::XmlNode::NodeList nlist = node->get_children(
			child->get_pname());
		int lsize = nlist.size();
		if (lsize == 1)
			(*child) = *nlist.begin();
		else if (lsize == 0)
			continue;
		else if (lsize > 1)
			/* in mixture parameters, we should have only one
			 * instance for each parameter*/
			throw Exception(
				"There is multiple " + child->get_pname()
					+ " node !", TracePoint("pparam"));
	}
	return (*this);
}

template<typename List>
XParam& _XMixParam<List>::operator =(const XParam& xp) throw (Exception)
{
	const XMixParam* _xmp = dynamic_cast<const XMixParam*>(&xp);
	XMixParam* xmp = (XMixParam*) (_xmp);
	if (xmp == NULL)
		throw Exception("Bad mix XParam in assignment !",
			TracePoint("pparam"));

	// check prameter name and size of child parameters.
	if (get_pname() != xmp->get_pname()
		|| params.size() != xmp->params.size())
		throw Exception("Different mix xparameters "
			"in assignment !", TracePoint("pparam"));

	try {
		assignHelper(xp);
	} catch (Exception &e) {
		e.addTracePoint(TracePoint("pparam"));
		throw e;
	}
	iterator iter = params.begin();
	iterator xp_iter = xmp->begin();
	for (; iter != params.end(); ++iter, ++xp_iter) {
		XParam* child = *iter;
		XParam* xp_child = *xp_iter;
		*child = *xp_child;
	}
	return *this;
}

template<typename List>
bool _XMixParam<List>::operator == (const XParam &parameter) throw (Exception)
{
	const	XMixParam	*_mixParameter =
				dynamic_cast<const XMixParam*>(&parameter);
	XMixParam		*mixParameter = (XMixParam*) _mixParameter;
	iterator		first = params.begin();
	iterator		second = mixParameter->begin();

	if (!mixParameter)
		throw Exception(Exception::FAILED,
				"Bax mix parameter in assignment !",
				TracePoint("pparam"));
	if (params.size() != mixParameter->size())
		return false;
	for (; first != params.end(); first++, second++) {
		if (**first != **second)
			return false;
	}

	return true;
}

template<typename List>
bool _XMixParam<List>::operator != (const XParam &parameter) throw (Exception)
{
	return !(*this == parameter);
}

template<typename List>
void _XMixParam<List>::reset()
{
	for (const_iterator iter = params.begin(); iter != params.end(); ++iter)
		(*iter)->reset();
}

template<typename List>
string _XMixParam<List>::_xml(bool show_runtime, 
			const int& indent, const string& endl) const
{
	/* we shouldn't write runtime parameters. */
	if (dont_show(show_runtime))
		return "";

	string ind(indent, ' ');
	string xstr = "";
	string ver = (version.empty()) ? "" : " ver=\"" + version + "\"";
	xstr += ind + "<" + pname + ver + ">" + endl;
	for (const_iterator iter = params.begin(); iter != params.end(); ++iter) {
		xstr += (*iter)->_xml(show_runtime,
			(indent) ? indent + 4 : indent, endl);
	}
	xstr += ind + "</" + pname + ">" + endl;
	return xstr;
}

template<typename List>
bool _XMixParam<List>::verify() throw (Exception)
{
	bool ret = true;
	for (iterator iter = params.begin(); iter != params.end(); ++iter) {
		ret = ret && (*iter)->verify();
	}
	return ret;
}

template<typename List>
void _XMixParam<List>::dbSave(const XParam* parentNode) throw (Exception)
{
	if (params.size() == 0)
		return;

	stringList fields, values;
	string lkey = this->get_key();
	if (lkey.empty()) {
		dbengine->rollbackTransaction();
		throw Exception("No key assigned : " + this->get_pname(),
			TracePoint("pparam"));
	}
	if (parentNode == NULL)
		dbengine->startTransaction();

	for (iterator iter = params.begin(); iter != params.end(); ++iter) {
		XMixParam *xmix = dynamic_cast<XMixParam *>(*iter);
		if (xmix == NULL) //its single
		{
			const XParam *xpar = *iter;
			if (xpar == NULL) {
				dbengine->rollbackTransaction();
				throw Exception(
					"Cant cast member of "
						+ this->get_pname() + "!",
					TracePoint("pparam"));
			} else {
				fields.push_back(xpar->get_pname());
				values.push_back(xpar->value());
			}
		} else
			//its mix
			xmix->dbSave((XParam*) this);
	}
	if (parentNode != NULL)
		dbengine->saveXParam(this->get_pname(), this->get_key(),
			parentNode->get_pname(), parentNode->get_key(), fields,
			values);
	else
		dbengine->saveXParam(this->get_pname(), this->get_key(), fields,
			values);

	if (parentNode == NULL)
		dbengine->commitTransaction();
}

template<typename List>
void _XMixParam<List>::dbUpdate(const XParam* parentNode) throw (Exception)
{
	if (params.size() == 0)
		return;

	stringList fields, values;
	if (parentNode == NULL)
		dbengine->startTransaction();

	string lkey = this->get_key();
	if (lkey.empty()) {
		dbengine->rollbackTransaction();
		throw Exception("No key assigned : " + this->get_pname(),
			TracePoint("pparam"));
	}
	//fields
	for (iterator iter = params.begin(); iter != params.end(); ++iter) {
		XMixParam *xmix = dynamic_cast<XMixParam *>(*iter);
		if (xmix == NULL) //its single
		{
			const XParam *xpar = *iter;
			if (xpar == NULL) {
				dbengine->rollbackTransaction();
				throw Exception(
					"Cant cast member of "
						+ this->get_pname() + "!",
					TracePoint("pparam"));
			} else {
				fields.push_back(xpar->get_pname());
				values.push_back(xpar->value());
			}
		} else { //its mix
			xmix->dbUpdate((XParam*) this);
		}
	}
	if (parentNode == NULL)
		dbengine->updateXParam(this->get_pname(), this->get_key(),
			fields, values);
	else
		dbengine->updateXParam(this->get_pname(), this->get_key(),
			parentNode->get_pname(), parentNode->get_key(), fields,
			values);

	if (parentNode == NULL)
		dbengine->commitTransaction();
}

template<typename List>
void _XMixParam<List>::dbDelete(const XParam* parentNode) throw (Exception)
{
	if (parentNode == NULL)
		dbengine->startTransaction();

	if (this->get_key().empty()
		|| (parentNode != NULL && parentNode->get_key().empty())) {
		dbengine->rollbackTransaction();
		throw Exception("No key assigned : " + this->get_pname(),
			TracePoint("pparam"));
	}
	for (iterator iter = params.begin(); iter != params.end(); ++iter) {
		XMixParam *xmix = dynamic_cast<XMixParam *>(*iter);
		if (xmix != NULL) { //its mix
			xmix->dbDelete((XParam*) this);
		}
	}
	if (parentNode != NULL)
		dbengine->removeXParam(this->get_pname(), this->get_key(),
			parentNode->get_pname(), parentNode->get_key());
	else
		dbengine->removeXParam(this->get_pname(), this->get_key());

	if (parentNode == NULL)
		dbengine->commitTransaction();
}

template<typename List>
void _XMixParam<List>::dbCreateStructure(const XParam* parentNode)
	throw (Exception)
{
	if (params.size() == 0)
		return;

	stringList fields;
	vector<DBFieldTypes> ftypes;
	if (parentNode == NULL)
		dbengine->startTransaction();

	for (iterator iter = params.begin(); iter != params.end(); ++iter) {
		XMixParam *xmix = dynamic_cast<XMixParam *>(*iter);
		if (xmix == NULL) //its single
		{
			const XParam *xpar = *iter;
			if (xpar == NULL) {
				dbengine->rollbackTransaction();
				throw Exception(
					"Cant cast member of "
						+ this->get_pname() + "!",
					TracePoint("pparam"));
			} else {
				fields.push_back(xpar->get_pname());
				ftypes.push_back(xpar->getDataType());
			}
		} else { //its mix
			xmix->dbCreateStructure((XParam*) this);
		}
	}
	if (parentNode == NULL)
		dbengine->createXParamStructure(this->get_pname(), fields,
			ftypes);
	else
		dbengine->createXParamStructure(this->get_pname(),
			parentNode->get_pname(), fields, ftypes);

	if (parentNode == NULL)
		dbengine->commitTransaction();
}

template<typename List>
void _XMixParam<List>::dbDestroyStructure(const XParam* parentNode)
	throw (Exception)
{
	if (parentNode == NULL)
		dbengine->startTransaction();

	for (iterator iter = params.begin(); iter != params.end(); ++iter) {
		XMixParam *xmix = dynamic_cast<XMixParam *>(*iter);
		if (xmix != NULL) { //its mix
			xmix->dbDestroyStructure((XParam*) this);
		}
	}
	dbengine->destroyXParamStructure(this->get_pname());
	if (parentNode == NULL)
		dbengine->commitTransaction();
}

template<typename List>
void _XMixParam<List>::dbLoad(const XParam* parentNode) throw (Exception)
{
	stringList fields, values;
	int res;
	if (parentNode == NULL)
		res = dbengine->loadXParamRow(this->get_pname(),
			this->get_key(), fields, values);
	else
		res = dbengine->loadXParamRow(this->get_pname(),
			this->get_key(), parentNode->get_pname(),
			parentNode->get_key(), fields, values);
	if (res == 0)
		return;
	this->dbLoad(fields, values);
}

template<typename List>
void _XMixParam<List>::dbLoad(stringList &fields, stringList &values)
	throw (Exception)
{
	for (unsigned int i = 0; i < fields.size(); i++) {
		if (this->value(fields[i]) == NULL)
			throw Exception(
				"Theres no field with name of '" + fields[i]
					+ "' in '" + this->get_pname()
					+ "' to put loaded data in it.",
				TracePoint("pparam"));
		(*this->value(fields[i])) = values[i];
	}
	for (unsigned int i = 0; i < params.size(); i++) {
		XMixParam *xptr = dynamic_cast<XMixParam *>(value(i));
		if (xptr != NULL)
			xptr->dbLoad(this);
	}
}

template<typename List>
void _XMixParam<List>::setDBEngine(XDBEngine* engine)
{
	dbengine = engine;
	for (iterator iter = params.begin(); iter != params.end(); ++iter) {
		XMixParam *xmix = dynamic_cast<XMixParam *>(*iter);
		if (xmix != NULL) { //its mix
			xmix->setDBEngine(engine);
		}
	}
}

template<typename List>
XDBEngine* _XMixParam<List>::getDBEngine()
{
	return dbengine;
}

template<typename List>
string _XMixParam<List>::generateJoinStmts(const XParam* parentNode)
{
	std::stringstream buff;
	if (parentNode != NULL)
		buff << " INNER JOIN " << this->get_pname() << " ON "
			<< this->get_pname() << "." << parentNode->get_pname()
			<< "_key=" << parentNode->get_pname() << "."
			<< parentNode->get_pname() << "_key";
	for (iterator iter = params.begin(); iter != params.end(); ++iter) {
		XMixParam *xmix = dynamic_cast<XMixParam *>(*iter);
		if (xmix != NULL) { //its mix
			buff << xmix->generateJoinStmts(this);
		}
	}
	return buff.str();
}

/* Implementation of "XIntParam" Class.
 */
template <typename T>
XParam &XIntParam<T>::operator=(const XParam &xp) throw (Exception)
{
	const XIntParam *xip = dynamic_cast<const XIntParam *>(&xp);
	if (xip == NULL)
		throw Exception(get_pname() + ": Bad XIntParam in assignment !",
					TracePoint("pparam"));
	// check prameter name.
	if (get_pname() != xip->get_pname())
		throw Exception("Different mix xparameters "
				"in assignment !",
				TracePoint("pparam"));
	try {
		assignHelper(xp);
	} catch(Exception &e) {
		e.addTracePoint(TracePoint("pparam"));

		throw e;
	}
	min = xip->min;
	max = xip->max;
	val = xip->val;

	return *this;
}

template <typename T>
XIntParam<T> &XIntParam<T>::operator++()
{
	val ++;
	if (checkLimit() && (val > max)) val = min;
	return *this;
}

template <typename T>
XIntParam<T> XIntParam<T>::operator++(int)
{
	XIntParam<T> temp = *this;
	val ++;
	if (checkLimit() && (val > max)) val = min;
	return temp;
}

template <typename T>
XIntParam<T> &XIntParam<T>::operator--()
{
	val --;
	if (checkLimit() && (val < min)) val = max;
	return *this;
}

template <typename T>
XIntParam<T> XIntParam<T>::operator--(int)
{
	XIntParam<T> temp = *this;
	val --;
	if (checkLimit() && (val < min)) val = max;
	return temp;
}

/* Implementation of "XEnumParam" Class.
 */
template <typename T>
XParam &XEnumParam<T>::operator = (const XParam &xp) throw (Exception)
{
	const XEnumParam<T> *xep =
			dynamic_cast<const XEnumParam<T> *>(&xp);
	if (xep == NULL)
		throw Exception("Bad enum XParam in assignment !",
					TracePoint("pparam"));
	// check prameter name.
	if (get_pname() != xep->get_pname())
		throw Exception("Different enum xparameters "
			"in assignment !", TracePoint("pparam"));
	try {
		assignHelper(xp);
	} catch (Exception &e) {
		e.addTracePoint(TracePoint("pparam"));
		throw e;
	}
	def = xep->def;
	val = xep->val;
	return *this;
}

/* Implementation of "XSetParam" Class.
 */
template<typename T, typename Key, typename List>
XParam &XSetParam<T, Key, List>::operator=(const XmlNode *node) throw (Exception)
{
	if (!is_myNode(node)) return (*this);

	XmlNode::NodeList nlist = node->get_children();
	for (XmlNode::NodeList::iterator iter = nlist.begin();
				iter != nlist.end(); ++iter) {
		const xmlpp::Element *nElem =
			dynamic_cast<const xmlpp::Element *>(*iter);
		if (nElem) {
			/** parameter with type of sub-parameters.
			 */
			XParam *sparam = NULL;
			try {
				sparam = newT(*iter);
				if (sparam->is_myNode(*iter)) {
					(*sparam) = (*iter);
					addParam(sparam);
				} else delete sparam;
			} catch (Exception &e) {
				clear();
				if (sparam) delete sparam;
				e.addTracePoint(TracePoint("pparam"));
				throw e;
			}
		}
	}
	return (*this);
}

template<typename T, typename Key, typename List>
XParam &XSetParam<T, Key, List>::operator=(const XParam &xp) throw (Exception)
{
	const _XSetParam *_xsp = dynamic_cast<const _XSetParam*>(&xp);
	_XSetParam *xsp = (_XSetParam *) _xsp;
	if (xsp == NULL)
		throw Exception("Bad set XParam in assignment !",
					TracePoint("pparam"));
	/* check prameter name. */
	if (get_pname() != xsp->get_pname())
		throw Exception("Different set xparameters "
			"in assignment !", TracePoint("pparam"));
	try {
		assignHelper(xp);
	} catch(Exception &e) {
		e.addTracePoint(TracePoint("pparam"));
		throw e;
	}
	/* clear current content. */
	clear();
	for (iterator xp_iter = xsp->begin(); xp_iter != xsp->end();
							++xp_iter) {
		T *sparam = dynamic_cast<T *>(*xp_iter);
		if (sparam == NULL) {
			clear();
			throw Exception("Bad T param in set"
					" Xparam assginment !",
					TracePoint("pparam"));
		}
		try {
			addT(*sparam);
		} catch (Exception &e) {
			clear();
			e.addTracePoint(TracePoint("pparam"));
			throw e;
		}
	}
	return *this;
}

template<typename T, typename Key, typename List>
void XSetParam<T, Key, List>::dbSave(const XParam *parentNode) throw (Exception)
{
	if (params.size() == 0)
		return;
	if (parentNode == NULL)
		dbengine->startTransaction();
	iterator iter = params.begin();
	const XMixParam *xmix = dynamic_cast<const XMixParam *>(*iter);
	if (xmix == NULL) { //its single
		for (iterator iter = params.begin();
			iter != params.end(); ++iter) {
			const XParam *xsp = (const XParam *) *iter;
			stringList fields, values;
			fields.push_back(xsp->get_pname());
			values.push_back(xsp->value());
			if (parentNode == NULL)
				dbengine->saveXParam(xsp->get_pname(),
					xsp->get_key(), fields, values);
			else
				dbengine->saveXParam(xsp->get_pname(),
					xsp->get_key(),
					parentNode->get_pname(),
					parentNode->get_key(), fields,
					values);
		}
	} else { //its mix
		for (iterator iter = params.begin();
			iter != params.end(); ++iter) {
			XMixParam *xmp = (XMixParam *)(*iter);
			xmp->dbSave(parentNode);
		}
	}
	if (parentNode == NULL)
		dbengine->commitTransaction();
}

template<typename T, typename Key, typename List>
void XSetParam<T, Key, List>::dbUpdate(const XParam *parentNode) throw (Exception)
{
	if (params.size() == 0)
		return;
	stringList fields, values;
	if (parentNode == NULL)
		dbengine->startTransaction();
	XParam *xptr = newT(NULL);
	const XMixParam *xmix = dynamic_cast<const XMixParam *>(xptr);
	if (xmix == NULL) { //its single
		dbengine->removeXParamByParent(xptr->get_pname(),
						parentNode->get_pname(),
						parentNode->get_key());
		for (iterator iter = params.begin();
			iter != params.end(); ++iter) {
			const XParam *xsp = (const XParam *) *iter;
			stringList fields, values;
			fields.push_back(xsp->get_pname());
			values.push_back(xsp->value());
			if (parentNode == NULL)
				dbengine->saveXParam(xsp->get_pname(),
					xsp->get_key(), fields, values);
			else
				dbengine->saveXParam(xsp->get_pname(),
					xsp->get_key(),
					parentNode->get_pname(),
					parentNode->get_key(), fields,
					values);
		}
	} else { //its mix
		for (iterator iter = params.begin();
			iter != params.end(); ++iter) {
			XMixParam *xmp = (XMixParam *)(*iter);
			xmp->dbUpdate(parentNode);
		}
	}
	if (parentNode == NULL)
		dbengine->commitTransaction();
}

template<typename T, typename Key, typename List>
void XSetParam<T, Key, List>::dbDelete(const XParam *parentNode) throw (Exception)
{
	if (parentNode == NULL)
		dbengine->startTransaction();
	XParam *xptr = newT(NULL);
	XMixParam *xmix = dynamic_cast<XMixParam *>(xptr);
	if (xmix != NULL) { //its mix
		xmix->dbDelete((XParam*) this);
	} else {
		dbengine->removeXParamByParent(xptr->get_pname(),
			parentNode->get_pname(), parentNode->get_key());
	}
	if (parentNode == NULL)
		dbengine->commitTransaction();
}

template<typename T, typename Key, typename List>
void XSetParam<T, Key, List>::dbCreateStructure(const XParam *parentNode) 
							throw (Exception)
{
	if (params.size() == 0)
		return;
	stringList fields;
	vector<DBFieldTypes> ftypes;
	if (parentNode == NULL)
		dbengine->startTransaction();
	XParam *xptr = newT(NULL);
	XMixParam *xmix = dynamic_cast<XMixParam *>(xptr);
	if (xmix == NULL) //its single
	{
		if (xptr == NULL) {
			dbengine->rollbackTransaction();
			throw Exception(
				"Cant cast member of "
					+ this->get_pname() + "!",
				TracePoint("pparam"));
		} else {
			fields.push_back(xptr->get_pname());
			ftypes.push_back(xptr->getDataType());
			if (parentNode == NULL)
				dbengine->createXParamStructure(
					xptr->get_pname(), fields,
					ftypes);
			else
				dbengine->createXParamStructure(
					xptr->get_pname(),
					parentNode->get_pname(), fields,
					ftypes);
		}
	} else { //its mix
		xmix->dbCreateStructure((XParam*) this);
	}
	if (parentNode == NULL)
		dbengine->commitTransaction();
}

template<typename T, typename Key, typename List>
void XSetParam<T, Key, List>::dbDestroyStructure(const XParam *parentNode) 
							throw (Exception)
{
	if (parentNode == NULL)
		dbengine->startTransaction();
	XParam *xptr=newT(NULL);
	XMixParam *xmix = dynamic_cast<XMixParam *>(xptr);
	if (xmix != NULL) { //its mix
		xmix->dbDestroyStructure((XParam*) this);
	}
	else
	{
		dbengine->destroyXParamStructure(xptr->get_pname());
	}
	if (parentNode == NULL)
		dbengine->commitTransaction();
}

template<typename T, typename Key, typename List>
void XSetParam<T, Key, List>::dbLoad(const XParam *parentNode) throw (Exception)
{
	XParam *test = newT(NULL);
	XMixParam *xmix = dynamic_cast<XMixParam *>(test);
	if (xmix != NULL) { //its mix
		stringList keys;
		dbengine->loadXParamKeyListByParent(test->get_pname(),
			parentNode->get_pname(), parentNode->get_key(),
			keys);
		for (unsigned int i = 0; i < keys.size(); i++) {
			XMixParam *newitem = (XMixParam*)newT(NULL);
			newitem->setDBEngine(this->getDBEngine());
			stringList fields, values;
			dbengine->loadXParamRow(newitem->get_pname(),
				keys[i], parentNode->get_pname(),
				parentNode->get_key(), fields, values);
			newitem->dbLoad(fields,values);
			this->addParam(newitem);
		}
	} else {
		stringList values;
		dbengine->loadXParamValueListByParent(test->get_pname(),
			parentNode->get_pname(), parentNode->get_key(),
			test->get_pname(), values);
		for (unsigned int i = 0; i < values.size(); i++) {
			XParam *newitem = newT(NULL);
			(*newitem) = values[i];
			this->addParam(newitem);
		}
	}
	delete test;
}

template<typename T, typename Key, typename List>
void XSetParam<T, Key, List>::dbQuery(XDBCondition &conditions)
{
	XMixParam *test = (XMixParam *) newT(NULL);
	string cmd = "SELECT DISTINCT " + test->get_pname() + "."
		+ test->get_pname() + "_key FROM " + test->get_pname()
		+ " " + test->generateJoinStmts() + " WHERE "
		+ conditions.getConditions();
	vector<vector<string> > res;
	vector<string> cols;
	this->getDBEngine()->getData(cmd, res, cols);

	//xptr->get_pname();
	for (unsigned int i = 0; i < res.size(); i++) {
		XMixParam *newitem = (XMixParam*) newT(NULL);
		newitem->setDBEngine(this->getDBEngine());
		stringList fields, values;
		this->getDBEngine()->loadXParamRow(newitem->get_pname(),
			res[i][0], fields, values);
		newitem->dbLoad(fields, values);
		this->addParam(newitem);
	}
	delete test;
}

template<typename T, typename Key, typename List>
string XSetParam<T, Key, List>::generateJoinStmts(const XParam *parentNode)
{
	XParam *xptr=newT(NULL);
	XMixParam *xmix = dynamic_cast<XMixParam *>(xptr);
	if (xmix != NULL) { //its mix
		return xmix->generateJoinStmts(parentNode);
	}
	else { //its single
		std::stringstream buff;
		buff << " INNER JOIN " << xptr->get_pname() << " ON "
			<< xptr->get_pname() << "."
			<< parentNode->get_pname() << "_key="
			<< parentNode->get_pname() << "."
			<< parentNode->get_pname() << "_key";

		return buff.str();
	}
}

} // namespace pparam
