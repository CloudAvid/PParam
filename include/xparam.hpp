/**
 * \file xparam.hpp
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

#ifndef _PDN_XPARAM_HPP_
#define _PDN_XPARAM_HPP_

#include <iostream>
#include <libxml++/libxml++.h>

#include "exception.hpp"

#include <stdio.h>
#include <string>
using std::string;

#include <vector>
using std::vector;

#include <map>
using std::map;

#include <algorithm>
using std::find;

#include "xdbengine.hpp"
#include "xlist.hpp"

namespace pparam
{
typedef char		XByte;
typedef unsigned char	XUByte;
typedef short		XShort;
typedef unsigned short	XUShort;
typedef int		XInt;
typedef unsigned int	XUInt;
typedef long		XLong;
typedef unsigned long	XULong;
typedef float		XFloat;

/**
 * \class XParam (X Parameter)
 * abstract class, defines common attributes/functions of X-Parameters.
 */
class XParam
{
public:
	/** Parser for xml documents.
	 */
	typedef xmlpp::DomParser XmlParser;
	/** Node in parseed xml document.
	 */
	typedef xmlpp::Node XmlNode;
	/** typedef for byte values in XParam.
	 */
	typedef pparam::XByte XByte;
	typedef pparam::XUByte XUByte;
	/** typedef for short values in XParam.
	 */
	typedef pparam::XShort XShort;
	typedef pparam::XUShort XUShort;
	/** typedef for integer values in XParam.
	 */
	typedef pparam::XInt XInt;
	typedef pparam::XUInt XUInt;
	/** typedef for long values in XParam.
	 */
	typedef pparam::XLong XLong;
	typedef pparam::XULong XULong;
	/** typedef for float values in XParam.
	 */
	typedef pparam::XFloat XFloat;

	XParam();
	XParam(XParam &&);
	XParam(const string &_pname);
	/**
	 * Read parameter value from correspnonding XML node.
	 * \param node pointer to parameter node in XML document.
	 *
	 * Implemetation of this function in inherited classes should
	 * throw an exception of "Exception" type in any error
	 * or mismached condition.
	 */
	virtual XParam &operator = (const XmlNode *node) = 0;
	/**
	 * Retrive parameter value from given string.
	 *
	 * Implemetation of this function in inherited classes should
	 * throw an exception of "Exception" type in any error
	 * or mismached condition.
	 */
	virtual XParam &operator = (const string &) = 0;
	/**
	 * Retrive parameter value from correspond parameter.
	 *
	 * Implemetation of this function in inherited classes should
	 * throw an exception of "Exception" type in any error
	 * or mismached condition.
	 * All of inherited classes should check correctness of "left
	 * value" parameter by dynamic_cast.
	 */
	virtual XParam &operator = (const XParam &) = 0;

	/**
	 * Compares two parameters. Throws exception on dynamic casting error
	 * \return Returns true on success,otherwise returns false
	 */
	virtual bool operator == (const XParam &) = 0;
	virtual bool operator != (const XParam &) = 0;
	/**
	 * Would be called befor any xparam assignment by lvalue in
	 * XParam &operator= (const XParam).
	 *
	 * Assignment operator(=) works on XParam inherited parameters.
	 * If we have some non-XParam parameters in our class, we should
	 * implement this function to fill this paramters base on "lvalue"
	 * object.
	 * Also we may want to do some pre assignment functions we can
	 * implement this function.
	 * This function would be called after examination of truthness of
	 * left value object type (base on rvalue type) and also general
	 * xparam field like of name and before any XParam field
	 * assignment.
	 * Implemetation of this function in inherited classes should
	 * throw an exception of "Exception" type in any error
	 * or mismached condition.
	 * \param xp left value in operator=(Xparam &)
	 */
	virtual void assignHelper(const XParam &xp) { return; }

	/**
	 * Load parameter from xml-formatted string.
	 *
	 * This function would read xml string, parse him and pass the
	 * pointer of dom-root node to the parameter.
	 * \param xstr xml document.
	 * \param parser pointer to the specified parser to use from.
	 */
	void loadXmlStr(string xstr, XmlParser *parser = NULL)
						throw (Exception);
	/**
	 * Load parameter from xml-formatted content of specified file.
	 *
	 * This function would read xml document, parse him and pass the
	 * pointer of dom-root node to the parameter.
	 */
	void loadXmlDoc(string xdoc, XmlParser *parser = NULL)
						throw (Exception);
	/**
	 * Save the xml output of xparam in the specified file.
	 */
	void saveXmlDoc(string xdoc,
			bool show_runtime = false, const int &indent = 0,
			bool with_endl = false) const throw (Exception);
	/**
	 * return parameter value.
	 */
	virtual string value() const = 0;
	/**
	 * Default key operator.
	 * \see XSetParam
	 */
	virtual bool key(int &_key) { return false; }
	/**
	 * return Key in form of a string
	 * \return a string that contain key of this object. 
	 * NULL if theres no key specified
	 */
	virtual string get_key() const { return ""; }
	/**
	 * Reset parameter value to default value
	 */
	virtual void reset() = 0;
	/**
	 * Print out parameter value in xml format.
	 * \param show_runtime whould we see runtime parameters in xml string.
	 * \param indent size of indention.
	 * \param with_endl if put "endl" at the end of each line?
	 *
	 * this function is a wrapper for: _xml(...) function
	 */
	string xml(bool show_runtime = false,
			const int &indent = 0, bool with_endl = false) const;
	/**
	 * Print out parameter value in xml format.
	 * \param indent size of indention.
	 * \param endl 	 string would be used as end-line character.
	 */
	virtual string _xml(bool show_runtime,
			const int &indent, const string &endl) const = 0;
	/**
	 * Verifies parameter value.
	 *
	 * This function should be used after reading the parameter value
	 * from config repository.
	 * Implemetation of this function in inherited classes should
	 * throw an exception of "Exception" type in any error
	 * or mismached situation in parameter value..
	 * This function may be used to generate parameter local(private)
	 * values base on loaded parameter from config repository.
	 */
	virtual bool verify();
	/** Set parameter name.
	 */
	void set_pname(const string &name) { pname = name; }
	/** Set parametr version.
	 */
	void set_version(const string &ver) { version = ver; }
	/** Set/unset parameter as a runtime parameter.
	 */
	void set_runtime(bool rt = true) { runtime = rt; }
	/** Returns parametr name.
	 */
	string get_pname() const { return pname; }
	/** Returns parameter version.
	 */
	string get_version() const { return version; }
	/** is this paramter a runtime parameter.
	 */
	bool is_runtime() const { return runtime; }
	/** Is this parameter value empty?
	 */
	bool is_empty() const { return value().empty(); }
	/** get Database related type of this parameter.
	 */
	DBFieldTypes getDataType() const { return DBTEXT; }
	/**
	 * is this node mine?.
	 * \param node  pointer to parameter node in XML document.
	 * \return true: it's mine. false: it isn't mine.
	 *
	 * This function is used in "operator=(const XmlNode *node)".
	 * He verifies parameter name and version(if not empty) attribute.
	 */
	bool is_myNode(const XmlNode *node) throw (Exception);

	virtual ~XParam() {}
protected:
	/** strip blanks from front and end of string.
	 */
	string stripBlanks(string str);
	/** don't show this parameter in xml string..!
	 */
	bool dont_show(bool show_runtime) const
			{ return is_runtime() && !show_runtime; }
protected:
	/** Parameter name.
	 * name of parameter in config repository.
	 */
	string pname;
	/**
	 * Parameter version.
	 *
	 * if parameter has a version number (version != ""), parametr should
	 * have a version attribute that must be equal to "ver".
	 * \note inherited classes should use set_version(...) to set their
	 * specific version number.
	 */
	string version;
	/**
	 * Parameter is a runtime parameter.
	 * true: is runtime parameter   false: is not runtime parameter
	 * default value: false
	 *
	 * runtime parameter is an specific parameter that his value wouldn't
	 * write to parameter repository. e.g. we can't see him
	 * in xml config file. His value is defined at runtime by program.
	 * (like of index paramter for disks)
	 * \note use set_runtime() to change runtime.
	 */
	bool runtime;
};

/**
 * \class XSingleField
 * defines parameter with single value.
 */
class XSingleParam : public XParam
{
public:
	XSingleParam(const string &_pname);
	XSingleParam(XSingleParam &&);

	/** Read parameter value from xml config file.
	 * \param node pointer to parameter node in XML document.
	 */
	virtual XParam &operator = (const XmlNode *node)
						throw (Exception);
	virtual bool operator == (const XParam &) throw (Exception);
	virtual bool operator != (const XParam &) throw (Exception);
	virtual string _xml(bool show_runtime,
				const int &indent, const string &endl) const;
	virtual ~XSingleParam() {}
};

/**
 * \class MixParam
 * Defines and manages a Mixture Parameter.
 *
 * Mixture Parameter is a parameter with some sub parameters.
 * Each sub-parameter can't repeat more than once.
 * \param List type of parameters list, would be one of C++ STL containors
 * of <XParam *>.
 */
template<typename List = std::vector<XParam *> >
class _XMixParam : public XParam
{
public:
	typedef _XMixParam<List>			XMixParam;
	typedef List 					list;
	typedef typename list::iterator 		iterator;
	typedef typename list::const_iterator 		const_iterator;
	typedef typename list::reverse_iterator 	riterator;
	typedef typename list::const_reverse_iterator 	const_riterator;

	_XMixParam(_XMixParam &&);
	_XMixParam(const string &_pname);

	virtual XParam &operator = (const XParam::XmlNode *node)
						throw (Exception);
	virtual XParam &operator = (const string &) { return *this; }
	virtual XParam &operator = (const XParam &xp)
						throw (Exception);
	virtual bool operator == (const XParam &) throw (Exception);
	virtual bool operator != (const XParam &) throw (Exception);
	virtual string _xml(bool show_runtime,
				const int &indent, const string &endl) const;
	virtual string value() const { return ""; }
	virtual void reset();
	virtual XParam *value(int index) const;
	virtual XParam *value(string name) const;
	virtual bool verify() throw (Exception);
	/** Add one sub-parameter to list of sub-parameters. */
	virtual void addParam(XParam *param) { params.push_back(param); }

	XUInt size() const { return params.size(); }
	iterator begin() { return params.begin(); }
	iterator end() { return params.end(); }
	const_iterator begin() const { return params.begin(); }
	const_iterator end() const { return params.end(); }

	const_iterator const_begin() const { return params.begin(); }
	const_iterator const_end() const { return params.end(); }
	const_iterator cbegin() const { return params.begin(); }
	const_iterator cend() const { return params.end(); }

	riterator rbegin() { return params.rbegin(); }
	riterator rend() { return params.rend(); }
	const_riterator rbegin() const { return params.rbegin(); }
	const_riterator rend() const { return params.rend(); }

	const_riterator const_rbegin() const { return params.rbegin(); }
	const_riterator const_rend() const { return params.rend(); }
	const_riterator crbegin() const { return params.rbegin(); }
	const_riterator crend() const { return params.rend(); }

	// Database functions
	/**
	 * save this XParam and its children using associated XDBEngine.
	 * \param [in] parentNode Parent XParam of this object
	 */
	virtual void dbSave(const XParam *parentNode =
		(XParam *) NULL) throw (Exception);
	/**
	 * Update stored data using this XParam and its children by
	 * associated XDBEngine
	 * \param [in] parentNode Parent XParam of this object
	 */
	virtual void dbUpdate(const XParam *parentNode =
		(XParam *) NULL) throw (Exception);
	/**
	 * Remove stored data of this XParam and its children by
	 * associated XDBEngine
	 * \param [in] parentNode Parent XParam of this object
	 */
	virtual void dbDelete(const XParam *parentNode =
		(XParam *) NULL) throw (Exception);
	/**
	 * Create proper structer of data in database for
	 * XParam and its children by associated XDBEngine
	 * \param [in] parentNode Parent XParam of this object
	 */
	virtual void dbCreateStructure(const XParam *parentNode =
		(XParam *) NULL) throw (Exception);
	/**
	 * Destroys related databse's data structure
	 * \param [in] parentNode Parent XParam of this object
	 */
	virtual void dbDestroyStructure(const XParam *parentNode 
						= (XParam *) NULL) 
						throw (Exception);
	virtual void dbLoad(const XParam *parentNode = (XParam *) NULL) 
						throw (Exception);
	virtual void dbLoad(stringList &fields, stringList &values)
						throw (Exception);
	virtual void setDBEngine(XDBEngine *engine);
	virtual XDBEngine *getDBEngine();
	virtual string generateJoinStmts(const XParam *parentNode =
		(XParam *) NULL);
	
	virtual ~_XMixParam() {}

protected:
	/**
	 * list of sub-element(parameters) of the mixture parameter.
	 */
	list params;
	XDBEngine *dbengine;
};
/**
 * \typedef XMixParam
 * Default "XMixParam" definition, so default usage of that.
 */
typedef _XMixParam<std::vector<XParam *> > XMixParam;

/**
 * \class XTextParam
 * Defines a parameter with text value.
 */
class XTextParam : public XSingleParam
{
public:
	XTextParam(const string &_pname);
	XTextParam(XTextParam &&_xtp);
	XTextParam &operator = (const XTextParam &vtp);
	virtual XParam &operator = (const string &str);
	virtual XParam &operator = (const char *str);
	virtual XParam &operator = (const XParam &xp) throw (Exception);
	string value() const;
	virtual void reset();
	void set_cdata(const bool _cdata);
	void set_value(const string &str);
	void set_value(const char *str);
	string get_value() const;
	bool empty() const;
	const char *c_str();
	virtual ~XTextParam();

protected:
	/* Determines generating XML in CDATA format or not */
	bool	cdata;
	/** parameter value */
	string	val;
};

/**
 * \class XIntParam
 * Defines a parameter with integer value.
 *
 * T would support >=, ++, -- operators.
 * In overloaded ++ & -- operators, when val be greater than max / lower than
 * min, val circulates to min/max.
 */
template <typename T>
class XIntParam : public XSingleParam
{
public:
	typedef XIntParam<T>	_XIntParam;

	/**
	 * \param _max maximum value of parameter.
	 * \param _min minimum value of parameter.
	 *
	 * if _max value be less than _min (_max < _min)
	 * means we don't want to check parameter boundries.
	 */
	XIntParam(const string &_pname, const T &_min, const T &_max) :
			XSingleParam(_pname),
			min(_min), max(_max), val(_min)
	{}
	XIntParam(const _XIntParam &iparam) : 
				XSingleParam(iparam.pname),
				min(iparam.min), max(iparam.max),
				val(iparam.val)
	{ }
	XIntParam(XIntParam &&_xip) : XSingleParam(std::move(_xip)),
			min(_xip.min), max(_xip.max), val(_xip.val)
	{ }

	//XIntParam &operator = (const XIntParam &vip) { val = vip.val; }
	virtual XParam &operator = (const string &str) throw (Exception)
	{
		T value;
		std::istringstream iss(str);
		iss >> value;

		return (*this) = value;
	}
	virtual XParam &operator=(const T &value) throw (Exception)
	{
		set_value(value);
		return (*this);
	}
	virtual XParam &operator=(const XParam &xp) throw (Exception);
	virtual _XIntParam &operator++();
	virtual _XIntParam operator++(int);
	virtual _XIntParam &operator--();
	virtual _XIntParam operator--(int);
	string value() const
	{
		std::ostringstream oss;
		oss << val;

		return oss.str();
	}
	virtual void reset() { val = min; }
	void set_value(const T &value) throw (Exception) 
	{ 
		if ((max >= min) /* we should check boundries. */
			&& (value < min || value > max)) {
			throw Exception(pname +
					" value is out of range !",
					TracePoint("pparam"));
		}
		val = value;
	}
	T get_value() const { return val; }
	virtual ~XIntParam() {}
protected:
	bool checkLimit()
	{
		return max >= min;
	}

	/**
	 * parameter minimum value
	 */
	T min;
	/**
	 * parameter maximum value
	 */
	T max;
	/**
	 * parameter value.
	 */
	T val;
};

/**
 * \class XFloatParam
  Defines a parameter with float value.
 */
class XFloatParam : public XSingleParam
{
public:
	/**
	 * \param _max maximum value of parameter.
	 * \param _min minimum value of parameter.
	 *
	 * if _max value be less than _min (_max < _min)
	 * means we don't want to check parameter boundries.
	 */
	XFloatParam(const string &_pname,const XFloat &_min,const XFloat &_max);
	XFloatParam(XFloatParam &&_xfp);

	XFloatParam &operator = (const XFloatParam &vip)
	{
		val = vip.val;

		return *this;
	}
	virtual XParam &operator = (const string &) throw (Exception);
	virtual XParam &operator = (const XFloat &) throw (Exception);
	virtual XParam &operator = (const XParam &xp) throw (Exception);
	string value() const;
	virtual void reset() { val = min; }
	XFloat float_value() const { return val; }
	void set_value(const XFloat &value) throw (Exception)
		{ (*this) = value; }
	XParam::XFloat get_value() const { return val; }
	virtual ~XFloatParam() {}
protected:
	/**
	 * parameter minimum value
	 */
	XFloat min;
	/**
	 * parameter maximum value
	 */
	XFloat max;
	/**
	 * parameter value.
	 */
	XFloat val;
};

/**
 * \class XEnumParam
 * Defines a parameter with enumeration values.
 *
 * T would be a class with enumeration type empedded in.
 * This class should have one specific value:
 * 	"MAX": latest value in enum.
 * Also this class should have "public static const typeString[MAX]" array
 * that would store correspond string value for each enum value.
 */
template <typename T>
class XEnumParam : public XSingleParam
{
public:
	/**
	 * \param _def default value of parameter
	 * \param _valueString string of each value.
	 */
	XEnumParam(const string &_pname,
			unsigned short _def) :
			XSingleParam(_pname), def(_def), val(_def)
			{}
	XEnumParam(XEnumParam &&_xep) : XSingleParam(std::move(_xep)),
				def(_xep.def), val(_xep.def)
			{}
	XEnumParam &operator = (const XEnumParam &vp) 
	{ 
		val = vp.val; 

		return *this;
	}
	virtual XParam &operator = (const string &str) throw (Exception)
	{
		for (int i = 0; i < static_cast<XInt>(T::MAX); ++i) {
			if (str == T::typeString[i]) {
				val = i;
				return (*this);
			}
		}
		throw Exception("Bad <" + pname + "> value !",
						TracePoint("pparam"));
	}
	virtual XParam &operator = (const XInt &value) throw (Exception)
	{
		set_value(value);

		return *this;
	}
	virtual XParam &operator = (const XParam &xp) throw (Exception);
	virtual string value() const
	{
		if (val < 0 || val >= T::MAX) return "";
		return T::typeString[val];
	}
	virtual void reset() { val = def; }
	virtual void set_value(const int &value) throw (Exception)
	{ 
		if (value >= 0 && value <= T::MAX) val = value;
		else throw Exception("Bad <" + pname + "> value !",
						TracePoint("pparam"));
	}
	virtual int get_value() const 
	{ 
		return val; 
	}

	virtual ~XEnumParam() {}
protected:
	/** default value.
	 */
	XInt def;
	/** parameter value;
	 */
	XInt val;
};

/**
 * \class XSetParam
 * manages set-parameter.
 *
 * set-parameter is a mixture parameter that his sub-parameters are from
 * same type and number of them is varriable (base on user needs)
 * like of <disks> in xml-config file.
 */
template<typename T, 
	 typename Key = int, typename List = std::vector<XParam *> >
class XSetParam : public _XMixParam<List>
{
public:
	typedef XSetParam<T, Key, List>			_XSetParam;
	typedef _XMixParam<List>			XMixParam;
	typedef typename XMixParam::iterator 		iterator;
	typedef typename XMixParam::const_iterator 	const_iterator;
	typedef typename XMixParam::riterator 		riterator;
	typedef typename XMixParam::const_riterator 	const_riterator;

	typedef std::map<Key, XParam *> 	map;
	typedef typename map::iterator 		smiterator;
	typedef typename map::const_iterator 	const_smiterator;
	typedef XParam::XmlNode			XmlNode;

	using XMixParam::begin;
	using XMixParam::end;
	using XMixParam::params;
	using XMixParam::dbengine;
	using XParam::is_myNode;
	using XParam::get_pname;
	using XParam::assignHelper;

	XSetParam(const string &_pname) : XMixParam(_pname),
						smapEnabled(false) {}
	XSetParam(XSetParam &&_xsp) : XMixParam(std::move(_xsp)),
		smap(std::move(_xsp.smap)), smapEnabled(_xsp.smapEnabled)
	{ 	
		params = std::move(_xsp.params);
	}
	/**
	 * \param node pointer to parameter node in XML document.
	 */
	virtual XParam &operator=(const XmlNode *node) throw (Exception);
	virtual XParam &operator=(const XParam &xp) throw (Exception);
	/**
	 * Add a copy of T-object to set.
	 *
	 * T should support "=" operator.
	 * \return pointer to the new created object from _t.
	 */
	virtual T *addT(const T &_t)
	{
		XParam *sparam = NULL;
		try {
			sparam = newT(_t);
			*sparam = *(XParam *)&_t;
			addParam(sparam);
		} catch (Exception &e) {
			/* delete allocated memory. */
			if(sparam) delete sparam;
			e.addTracePoint(TracePoint("pparam"));
			throw e;
		}
		return (T *)sparam;
	}
	virtual void addParam(XParam *param) throw (Exception)
	{
		T *sparam = dynamic_cast<T *>(param);
		if (sparam == NULL) {
			throw Exception("Bad T param in addParam",
				TracePoint("pparam"));
		}
		XMixParam::addParam(param);
		if (smapEnabled) {
			iterator iter = end();
			try {
				add2SMap(--iter);
			} catch (Exception &e) {
				/* Got o end to release last element.
				 */
				++ iter;
				/* remove added parameter from list.
				 */
				params.pop_back();
				e.addTracePoint(TracePoint("pparam"));
				throw e;
			}
		}
	}
	/**
	 * Clear all of child parameters.
	 */
	virtual void clear()
	{
		if (smapEnabled) clearSMap();
		/* free dynamic allocated memory. */
		for (iterator iter = begin(); iter != end(); ++iter) {
			XParam *param = *iter;
			delete param;
		}
		params.clear();
	}
	virtual void reset() { clear(); }
	/**
	 * Enable search map and ready him to work with.
	 *
	 * If you want to use search-map, you should call this function
	 * manually. Call of this function may occure at any time of
	 * life cycle of XSetParam object.
	 */
	void enable_smap() throw (Exception)
	{
		for (iterator iter = begin(); iter != end(); ++iter) {
			try {
				add2SMap(iter);
			} catch (Exception &e) {
				clearSMap();
				e.addTracePoint(TracePoint("pparam"));
				throw e;
			}
		}
		smapEnabled = true;
	}
	void disable_smap()
	{
		clearSMap();
		smapEnabled = false;
	}
	/**
	 * Find parameter base on id.
	 *
	 * You should call this function when smap has been enabled.
	 */
	XParam *find(const Key &_key) const
	{
		const_smiterator iter = smap.find(_key);
		if (iter != smap.end()) return iter->second;
		return NULL;
	}
	/**
	 * Find parameter with highest key.
	 *
	 * You should call this function when smap has been enabled.
	 */
	XParam *max()
	{
		typename map::reverse_iterator iter = smap.rbegin();
		if (iter != smap.rend()) return iter->second;
		return NULL;
	}
	/**
	 * Find parameter with lowest key.
	 *
	 * You should call this function when smap has been enabled.
	 */
	XParam *min()
	{
		smiterator iter = smap.begin();
		if (iter != smap.end()) return iter->second;
		return NULL;
	}
	/**
	 * Delete parameter(storage structure + memory) with specified.
	 *
	 * You should call this function when smap has been enabled.
	 */
	virtual void del(const Key &_key)
	{
		XParam *param = del_soft(_key);
		if (param) delete param;
	}
	/**
	 * Soft delete parameter(just storage structure) with specified key.
	 *
	 * This function will delete storage structures without erase of
	 * actual parameter memory.
	 * You should call this function when smap has been enabled.
	 */
	virtual XParam* del_soft(const Key &_key)
	{
		smiterator siter = smap.find(_key);
		if (siter == smap.end()) return NULL;
		iterator iter = std::find(begin(), end(), siter->second);
		XParam *ret = siter->second;
		params.erase(iter);
		smap.erase(siter);
		return ret;
	}
	/**
	 * Delete parameter(storage structure + memory) at specified location.
	 */
	virtual void del(iterator iter)
	{
		delete del_soft(iter);
	}
	/**
	 * Soft delete parameter(just storage structure) at specified location.
	 *
	 * This function will delete storage structures without erase of
	 * actual parameter memory.
	 */
	virtual XParam* del_soft(iterator iter)
	{
		if (smapEnabled) {
			Key _key;
			static_cast<T *>(*iter)->key(_key);
			smap.erase(smap.find(_key));
		}
		XParam *ret = *iter;
		params.erase(iter);
		return ret;
	}

	// Database functions
	/**
	 * Save this XParam and its children using associated XDBEngine
	 * to database.
	 * \param [in] parentNode Parent XParam of this object
	 */
	virtual void dbSave(const XParam *parentNode = (XParam *)NULL)
							throw (Exception);
	/**
	 * Update stored data using this XParam and its children by
	 * associated XDBEngine
	 * \param [in] parentNode Parent XParam of this object
	 */
	virtual void dbUpdate(const XParam *parentNode = (XParam *)NULL)
							throw (Exception);
	/**
	 * Remove stored data of this XParam and its children by
	 * associated XDBEngine
	 * \param [in] parentNode Parent XParam of this object
	 */
	virtual void dbDelete(const XParam *parentNode = (XParam *)NULL)
							throw (Exception);
	/**
	 * Create proper structer of data in database for
	 * XParam and its children by associated XDBEngine
	 * \param [in] parentNode Parent XParam of this object
	 */
	virtual void dbCreateStructure(const XParam *parentNode =
					(XParam *) NULL) throw (Exception);
	/**
	 * Destroys related databse's data structure
	 * \param [in] parentNode Parent XParam of this object
	 */
	virtual void dbDestroyStructure(const XParam *parentNode =
					(XParam *) NULL) throw (Exception);
	virtual void dbLoad(const XParam *parentNode = (XParam *) NULL)
							throw (Exception);
	virtual void dbQuery(XDBCondition &conditions);
	virtual string generateJoinStmts(const XParam *parentNode =
							(XParam *)NULL);
	virtual ~XSetParam()
	{
		clear();
	}

protected:
	/**
	 * Add defined parameter to search map.
	 *
	 * we can't store parameter with null id.
	 * Also we can't store parametes with same IDs. Each paramter should
	 * have a unique ID.
	 */
	void add2SMap(iterator iter) throw (Exception)
	{
		Key _key;
		T *tparam = static_cast<T *>(*iter);
		/* we can't store parameter with null id. */
		if (! tparam->key(_key)) {
			throw Exception("Parameter doesn't have any key !",
							TracePoint("pparam"));
		}
		/* we can't store parameters with same IDs. */
		if (keyExist(_key)) {
			throw Exception("Duplicated key parameter !",
						TracePoint("pparam"));
		}
		_add2SMap(_key, iter);
	}
	/**
	 * Is key exist in search map?
	 */
	virtual bool keyExist(const Key &_key)
	{
		return smap.find(_key) != smap.end();
	}
	/**
	 * Put proper elmenet at proper place in smap.
	 */
	virtual void _add2SMap(const Key &_key, const iterator &iter)
	{
		smap[_key] = *iter;
	}
	/**
	 * Clear content of smap.
	 */
	virtual void clearSMap()
	{
		smap.clear();
	}
	/**
	 * Search map base of parameters IDs.
	 *
	 * This a mapping between parameters Keys and their pointers.
	 * All of the sub-parameters(XSetParam is contailnor of them) should
	 * implement key function in this form: "bool key(Key &_key)".
	 *
	 * "key" function should set parameter key.
	 * "key" should set approprite Xparameter key value in "_key".
	 * \param _key key value that should set by "key" function.
	 * \return false or true, means there is any key for this parameter
	 * 	(true) or not (false).
	 * \code
	 * 	virtual bool key(Key &_key)
	 * 	{
	 * 		// set key value
	 * 		_key = KEY_VALUE;
	 * 		if (I HAVE A KEY) return true;
	 * 		if (I DON'T HAVE ANY KEY) return false;
	 * 	}
	 * \endcode
	 *
	 * All of the paramers should have unique Keys in XSetParam.
	 */
	map smap;
	/**
	 * smap has been enabled?
	 */
	bool smapEnabled;
	/**
	 * new functions ..
	 * This functions enable us to implement XISetParam functionalities.
	 */
	virtual T *newT(const XmlNode *node) throw (Exception)
	{
		T *t =  new T;
		if (t == NULL)
			throw Exception("Can't allocate memory !",
						TracePoint("pparam"));
		return t;
	}
	virtual T *newT(const T &t) throw (Exception)
	{
		return newT((const XmlNode *)NULL);
	}
};

/**
 * \class XISetParam
 * Manages set of inheritated parameters from T.
 *
 * Suppose we have some inherited paramters from T, and we like to have a
 * set of this parameters: like of C++ that we can have an array of pointers
 * of these inherited classes.
 * Class T for this purpose should support some features.
 * T Should define a predifined "Type" type. This type
 * should be an XParam inherited class, that can retreive type information
 * from xml-string.
 * Xml string may be in this form:
 * \code
 * 	<list>
 * 		<elem>
 * 			..
 * 			<type> ddd </type>
 * 			..
 * 		</elem>
 * 		<elem>
 * 			...
 * 		</elem>
 * 		....
 * 	</list>
 * \endcode
 * Type can retrive the "<type>" field and define type of inherited class.
 * Type also should have "public newT()" function.
 * This function should allocate appropriate type based on caller type-object.
 * If he couldn't allocate memory, shoud throw Exception.
 *
 * T should support "public type(type &t) const" function that would
 * adjust type based on caller object.
 */
template<typename T, 
	 typename Key = int, typename List = std::vector<XParam *> >
class XISetParam : public XSetParam<T, Key, List>
{
public:
	typedef typename T::Type Type;

	typedef XSetParam<T, Key, List>			_XSetParam;
	typedef typename _XSetParam::XMixParam		XMixParam;
	typedef typename _XSetParam::iterator 		iterator;
	typedef typename _XSetParam::const_iterator 	const_iterator;
	typedef typename _XSetParam::riterator 		riterator;
	typedef typename _XSetParam::const_riterator 	const_riterator;

	XISetParam(const string &_pname) : _XSetParam(_pname)
	{ }
	XISetParam(XISetParam &&_xisp) : _XSetParam(std::move(_xisp))
	{ }

protected:
	virtual T *newT(const XParam::XmlNode *node) throw (Exception)
	{
		Type tp;
		*(XParam *) &tp = node;
		T *tmp = dynamic_cast<T *>(tp.newT());
		if (tmp == NULL)
			throw Exception("newT failed!", TracePoint("pparam"));
		return tmp;
	}
	virtual T *newT(const T &t) throw (Exception)
	{
		Type tp;
		t.type(tp);
		T *tmp = dynamic_cast<T *>(tp.newT());
		if (tmp == NULL)
			throw Exception("newT failed!", TracePoint("pparam"));
		return tmp;
	}
};

/**
 * \class XListParam
 * "XList" of "XParam" parameters.
 */
template<typename T, typename Key = int>
class XListParam : public XISetParam<T, Key, XList<XParam *> >
{
public:
	typedef XISetParam<T, Key, XList<XParam *> >	_XSetParam;
	typedef XISetParam<T, Key, XList<XParam *> >	_XISetParam;
	typedef typename _XISetParam::XMixParam		XMixParam;
	typedef typename _XISetParam::iterator 		iterator;
	typedef typename _XISetParam::const_iterator 	const_iterator;
	typedef typename _XISetParam::riterator		riterator;
	typedef typename _XISetParam::const_riterator 	const_riterator;

	typedef std::map<Key, iterator> 	map;
	typedef typename map::iterator 		smiterator;
	typedef typename map::const_iterator 	const_smiterator;
	typedef XParam::XmlNode			XmlNode;

	using XMixParam::begin;
	using XMixParam::end;
	using XMixParam::params;
	using XMixParam::dbengine;
	using _XSetParam::smapEnabled;

	XListParam(const string &_pname) : _XISetParam(_pname)
	{}
	XListParam(XListParam &&_xlp) : _XISetParam(std::move(_xlp)),
					smap(std::move(_xlp.smap))
	{}

	/**
	 * Clear all of child parameters.
	 */
	virtual void clear()
	{
		iterator iter;
		while ((iter = begin()) != end()) {
			xdel_prepare(iter);
			xdel(iter);
		}
	}
	virtual void reset() { clear(); }
	/**
	 * Find parameter base on id.
	 *
	 * You should call this function when smap has been enabled.
	 */
	iterator find(const Key &_key)
	{
		const_smiterator iter = smap.find(_key);
		if (iter != smap.end()) return iter->second;
		return end();
	}
	const_iterator find(const Key &_key) const
	{
		const_smiterator iter = smap.find(_key);
		if (iter != smap.end()) return iter->second;
		return end();
	}
	/**
	 * Find parameter with highest key.
	 *
	 * You should call this function when smap has been enabled.
	 */
	const_iterator max() const
	{
		typename map::reverse_iterator iter = smap.rbegin();
		if (iter != smap.rend()) return iter->second;
		return end();
	}
	/**
	 * Find parameter with lowest key.
	 *
	 * You should call this function when smap has been enabled.
	 */
	const_iterator min() const
	{
		smiterator iter = smap.begin();
		if (iter != smap.end()) return iter->second;
		return end();
	}

	/**
	 * Prepare element with "_key" for deletion.
	 *
	 * This function would be called in critical area.
	 * After preparation, no one could see deleted element.
	 * \return end(): object doesn't exist or can't prepare him
	 * for deletion, !end(): iterator to prepared element. 
	 */
	iterator xdel_prepare(const Key &_Key)
	{
		smiterator siter = smap.find(_Key);
		if (siter == smap.end()) return end();
		iterator iter = siter->second;
		if (! params.xerase_prepare(iter)) iter = end();
		smap.erase(siter);
		return iter;
	}
	bool xdel_prepare(iterator &iter)
	{
		if (smapEnabled) {
			Key _key;
			static_cast<T *>(*iter)->key(_key);
			smiterator siter = smap.find(_key);
			if (siter != smap.end()) smap.erase(siter);
		}
		return params.xerase_prepare(iter);
	}
	/**
	 * Delete prepared element, should be called after "xdel_prepare" call.
	 *
	 * This function could be called on the fly, an would block
	 * until all concurrent accesses to the elment finish.
	 */
	void xdel(iterator &iter)
	{
		if (iter != end()) {
			XParam *xparam = *iter;
			params.xerase(iter);
			delete xparam;
		}
	}
	/**
	 * Soft delete prepared element(only storage structure), should be 
	 * called after "xdel_prepare" call.
	 * \return Pointer to parameter if iter be on any element, else NULL.
	 *
	 * This function could be called on the fly. It will not erase 
	 * parameter memory, just storage structure.
	 */
	XParam* xdel_soft(iterator &iter)
	{
		if (iter != end()) {
			XParam *xparam = *iter;
			params.xerase(iter);
			return xparam;
		} return NULL;
	}
	/**
	 * Delete parameter with specified key.
	 *
	 * You should call this function when smap has been enabled.
	 */
	virtual void del(const Key &_key)
	{
		iterator iter = xdel_prepare(_key);
		xdel(iter);
	}
	/**
	 * Soft delete parameter(only storage structure) with specified key.
	 * \return Pointer to paramter if found, else NULL.
	 *
	 * You should call this function when smap has been enabled.
	 */
	virtual XParam* del_soft(const Key &_key)
	{
		iterator iter = xdel_prepare(_key);
		return xdel_soft(iter);
	}
	/**
	 * Delete parameter at specified location.
	 */
	virtual void del(iterator &iter)
	{
		if (xdel_prepare(iter)) xdel(iter);
	}
	/**
	 * Soft delete parameter(only storage structure) at specified location.
	 * \return Pointer to paramter if could prepare, else NULL.
	 */
	virtual XParam* del_soft(iterator &iter)
	{
		if (xdel_prepare(iter)) return xdel_soft(iter);
		else return NULL;
	}

protected:
	/**
	 * Is key exist in search map?
	 */
	virtual bool keyExist(const Key &_key)
	{
		return smap.find(_key) != smap.end();
	}
	/**
	 * Put proper elmenet at proper place in smap.
	 */
	virtual void _add2SMap(const Key &_key, const iterator &iter)
	{
		smap[_key] = iter;
	}
	/**
	 * Clear content of smap.
	 */
	virtual void clearSMap()
	{
		smap.clear();
	}
	/**
	 * Search map.
	 * \see XSetParam::smap.
	 */
	map smap;
};

} // namespace pparam

#include "xparam.tcc"

#endif //_PDN_XPARAM_HPP_
