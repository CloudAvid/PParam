#include "xparam.hpp"
#include <fstream>
#include <iostream>

namespace pparam
{

XParam::XParam()
{
	pname = "__UNDEFINED__";
	version = "";
	runtime = false;
}

XParam::XParam(XParam &&_xp) : pname(std::move(_xp.pname)), 
			version(std::move(_xp.version)), runtime(_xp.runtime)
{ }

XParam::XParam(const string &_pname) :
	pname(_pname)
{
	version = "";
	runtime = false;
}

void XParam::loadXmlStr(string xstr, XParam::XmlParser *parser)
	throw (Exception)
{
	XmlParser * _parser = (parser == NULL) ? new XmlParser : parser;
	try {
		_parser->parse_memory(xstr.c_str());
		if ((*_parser)) {
			XmlNode *node =
				_parser->get_document()->get_root_node();
			XParam *_xp = this;
			*_xp = node;
			return;
		}
	} catch (std::exception &e) {
		throw Exception(
			string("Can't parse xml document: ") + e.what(),
			TracePoint("xpram"));
	} catch (Exception &e) {
		e.addTracePoint(TracePoint("pparam"));
		throw e;
	}

	if (parser == NULL)
		delete _parser;
	// In this point, there is no valid parser, so can't parse document
	throw Exception("Can't parse xml document: ",
		TracePoint("xpram"));
}

void XParam::loadXmlDoc(string xdoc, XmlParser *parser) throw (Exception)
{
	XmlParser *_parser = (parser == NULL) ? new XmlParser : parser;
	try {
		_parser->set_substitute_entities();
		_parser->parse_file(xdoc);
		if ((*_parser)) {
			XmlNode *node =
				_parser->get_document()->get_root_node();
			XParam *_xp = this;
			*_xp = node;
			return;
		}
	} catch (std::exception &e) {
		throw Exception(
			string("Can't parse xml document: ") + e.what(),
			TracePoint("xpram"));
	} catch (Exception &e) {
		e.addTracePoint(TracePoint("pparam"));
		throw e;
	}

	if (parser == NULL)
		delete _parser;
	// In this point, there is no valid parser, so can't parse document
	throw Exception("Can't parse xml document: ",
		TracePoint("pparam"));
}

void XParam::saveXmlDoc(string xdoc, bool show_runtime, const int &indent,
	bool with_endl) const throw (Exception)
{
	string sxml;
	try {
		sxml = xml(show_runtime, indent, with_endl);
	} catch (std::exception &e) {
		throw Exception("Can't generate xml to save " 
				+ get_pname() + " !: " + e.what(), 
				TracePoint("pparam"));
	}
	std::fstream xfile;
	try {
		xfile.open(xdoc.c_str(),
			std::ios_base::trunc | std::ios_base::out);
	} catch (std::exception &e) {
		throw Exception("Can't open file to save " 
				+ get_pname() + " !: " + e.what(),
				TracePoint("pparam"));
	}
	if (xfile.fail())
		throw Exception("Can't open file to save " 
				+ get_pname() + " !",
				TracePoint("pparam"));
	xfile << sxml;
	xfile.close();
}

string XParam::xml(bool show_runtime, const int &indent, bool with_endl) const
{
	string endl = (with_endl) ? "\n" : "";
	return _xml(show_runtime, indent, endl);
}

bool XParam::verify()
{
	return true;
}

string XParam::stripBlanks(string str)
{
	if (str.empty())
		return "";

	int start = str.find_first_not_of(" ");
	int end = str.find_last_not_of(" ");

	return str.substr(start, end - start + 1);
}

bool XParam::is_myNode(const XmlNode *node) throw (Exception)
{
	if (!node)
		return false;
	if (pname != node->get_name())
		return false;

	/* verify version number */
	if (version.empty())
		return true;
	xmlpp::Attribute *ver = ((xmlpp::Element *) (node))->get_attribute(
		"ver");
	if (!ver)
		throw Exception(
			"There is no \"ver\" attribute in " + pname
				+ " element", TracePoint("pparam"));

	if (ver->get_value() != version)
		throw Exception(
			"Bad " + pname + " version! " + "supported version is: "
				+ version, TracePoint("pparam"));

	return true;
}
/* Implementation of "XSingleParam" Class
 */
XSingleParam::XSingleParam(const string& _pname) :
	XParam(_pname)
{
}

XSingleParam::XSingleParam(XSingleParam &&_xsp) : XParam(std::move(_xsp))
{
}

XParam& XSingleParam::operator =(const XmlNode* node) throw (Exception)
{
	XParam* vparam = this;
	if (!is_myNode(node))
		return (*this);

	XmlNode::NodeList nlist = node->get_children();
	for (XmlNode::NodeList::iterator iter = nlist.begin();
		iter != nlist.end(); ++iter) {
		const xmlpp::TextNode* nText =
			dynamic_cast<const xmlpp::TextNode*>(*iter);
		if (nText) {
			(*vparam) = stripBlanks(nText->get_content());
		}
	}

	return (*this);
}

bool XSingleParam::operator == (const XParam &parameter) throw (Exception)
{
	const XSingleParam	*singleParameter =
				dynamic_cast<const XSingleParam*>(&parameter);
	if (!singleParameter)
		throw Exception(Exception::FAILED,
				"Bad single parameter in assignment !",
				TracePoint("pparam"));
	if (pname != singleParameter->get_pname())
		return false;
	if (value() != singleParameter->value())
		return false;

	return true;
}

bool XSingleParam::operator != (const XParam &parameter) throw (Exception)
{
	return !(*this == parameter);
}

string XSingleParam::_xml(bool show_runtime, const int& indent,
						const string& endl) const
{
	if (dont_show(show_runtime)) return "";

	string ver = (version.empty()) ? "" : " ver=\"" + version + "\"";
	string val = value();
	string ind(indent, ' ');
	return ind + "<" + pname + ver + ">" + val + "</" + pname + ">" + endl;
}

/* Implementation of "XTextParam" class
 */
XParam& XTextParam::operator =(const XParam& xp) throw (Exception)
{
	const XTextParam* xtp = dynamic_cast<const XTextParam*>(&xp);
	if (xtp == NULL)
		throw Exception("Bad text XParam in assignment !",
			TracePoint("pparam"));

	// check prameter name.
	if (get_pname() != xtp->get_pname())
		throw Exception("Different test xparameters "
			"in assignment !", TracePoint("pparam"));

	try {
		assignHelper(xp);
	} catch (Exception &e) {
		e.addTracePoint(TracePoint("pparam"));
		throw e;
	}
	val = xtp->val;
	return *this;
}

/* Implementation of "XFloatParam" Class
 */
XFloatParam::XFloatParam(const string& _pname, const XParam::XFloat& _min,
	const XParam::XFloat& _max) :
	XSingleParam(_pname), min(_min), max(_max), val(_min)
{
}

XFloatParam::XFloatParam(XFloatParam &&_xfp) : XSingleParam(std::move(_xfp)), 
			min(_xfp.min), max(_xfp.max), val(_xfp.min)
{
}

XParam& XFloatParam::operator =(const string& str) throw (Exception)
{
	XParam::XFloat value;
	std::istringstream iss(str);
	iss >> value;
	return (*this) = value;
}

XParam& XFloatParam::operator =(const XParam::XFloat& value) throw (Exception)
{
	if ((max >= min) /* we should check boundries. */
	&& (value < min || value > max)) {
		throw Exception(pname + " value is out of range !",
			TracePoint("pparam"));
	}
	val = value;
	return (*this);
}

XParam& XFloatParam::operator =(const XParam& xp) throw (Exception)
{
	const XFloatParam* xip = dynamic_cast<const XFloatParam*>(&xp);
	if (xip == NULL)
		throw Exception(
			get_pname() + ": Bad XFloatParam in assignment !",
			TracePoint("pparam"));

	// check prameter name.
	if (get_pname() != xip->get_pname())
		throw Exception("Different mix xparameters "
			"in assignment !", TracePoint("pparam"));

	try {
		assignHelper(xp);
	} catch (Exception &e) {
		e.addTracePoint(TracePoint("pparam"));
		throw e;
	}
	min = xip->min;
	max = xip->max;
	val = xip->val;
	return *this;
}

string XFloatParam::value() const
{
	char stringValue[30];
	string stringOutput;
	sprintf(stringValue, "%f", val);
	stringOutput.assign(stringValue);
	return stringOutput;
}

}// namespace pparam

