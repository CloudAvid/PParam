/**
 * \file xobject.hpp
 * Defines common object based on XParam and XISetParam featuers.
 *
 * We want to define common attributes and abilities of an object that
 * would be stored by XISetParam.
 * Also we want to manage all object dependencies in this xobject.
 * 
 * Copyright 2012 PDNSoft Co. (www.pdnsoft.com)
 * \author hamid jafarian (hamid.jafarian@pdnsoft.com)
 *
 * xobject is part of PParam.
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
 */
#ifndef _PDN_XOBJECT_HPP_
#define _PDN_XOBJECT_HPP_

#include "xparam.hpp"
#include "sparam.hpp"

namespace pparam 
{

/**
 * \class XObjectStatus
 * Different statuses of XObject in his life cycle.
 */
class XObjectStatus 
{
public:
	enum Status {
		LOADED,   /**< 0 - Object has been loaded to the memory. */
		ADDING,   /**< 1 - Add operation is active on XObject */
		NORMAL,   /**< 2 - Objects has been added and There isn't any 
					 active operation on XObject */
		MODIFYING,/**< 3 - Modify operation is active on XObject */
		DELETING, /**< 4 - Delete operation is active on XObject */
		DELETED,  /**< 5 - Delete operation completed 
					on XObject, Object deleted */
		SCANNING, /**< 6 - Someone scanning object */
		QUERYING, /**< 7 - Someone querying object */
		RELOADING,/**< 8 - Reloading object data. */
		PRINTING, /**< 9 - Printng object data - xmling */
		MAX,
	};
	/**
	 * \enum StatusTransition
	 * Type of transitions between object statuses (e-g- from x to y).
	 */
	enum StatusTransition {
		DIRECT = 0, 	/**< 0 - There is a direct path from x to y.
				Transition could be done immediately.*/
		D = DIRECT,     /**< 0 - Synonym for Direct. */
		WAIT = 1,	/**< 1 - Object can wait, there is a chance 
				that object changes his status from x to a 
				status that has direct/none transition to 
				target status(x).*/
		W = WAIT,	/**< 1- Synonim for INDIRECT */
		NONE = 2,	/**< 2 - Don't wait, May be some changes in 
				object status prepare situation for transition 
				from x to y, but there isn't any guaranty for 
				this changes to	happen.
				So don't wait and cancle operation. */
		N = NONE,	/**< Synonim for NONE. */
	};
	const static string typeString[MAX];
	const static StatusTransition transitionTable[MAX][MAX];
	static StatusTransition canTransit(int from, int to)
	{
		return transitionTable[from][to];
	}
};

template <typename Type>
class XObject;
template <typename Type>
class XObjectList;

/**
 * \class XObjectConnection
 * Defines a connection attributes between tow XObjects.
 *
 * XObjects may have different connections with other XObjects in the
 * repository. 
 * Each connection is bidirection, so when one object create a connection with
 * other objects, that objects also would have connection with creator.
 * Each XObject stores attributes of other side of connection.
 */
template <typename Type>
class XObjectConnection
{
public:
	typedef XObjectList<Type>		_XObjectList;
	typedef XObject<Type>			_XObject;
	typedef typename _XObjectList::iterator iterator;
	/**
	 * \enum ConnectionType
	 * Defines type of dependency of other side of connection to me.
	 */
	enum ConnectionType {
		WEAK = 0,
		STRONG = 1, /**< This side can't be deleted before disconnecting
		from other side. */
	};

	XObjectConnection(const string &_name, const string &_role,
				bool _notify, ConnectionType _type, 
				iterator _oside) : cid("cid")
	{
		name = _name;
		role = _role;
		notify = _notify;
		type = _type;
		oside = _oside;
	}
	XObjectConnection(const string &_name = "", const string &_role = "",
				bool _notify = false, 
				ConnectionType _type = WEAK) : cid("cid")
	{
		name = _name;
		role = _role;
		notify = _notify;
		type = _type;
	}
	XObjectConnection(const XObjectConnection &c) : cid("cid")
	{
		*this = c;
	}
	XObjectConnection &operator=(const XObjectConnection &c)
	{
		name = c.name;
		role = c.role;
		notify = c.notify;
		type = c.type;
		oside = c.oside;
		cid = c.cid.value();

		return *this;
	}
	/**
	 * Regenerating connection id.
	 */
	void regenerateCID()
	{
		cid.regenerate();
	}
	/**
	 * Return connection information in "XML" format.
	 */
	string xml() const
	{
		string ret = "<xobj_connection>";
		ret += cid.xml();
		ret += "<cname>" + name + "</cname>";
		ret += "<crole>" + role + "</crole>";
		ret += "<notify>" + 
			string((notify)? "true" : "false") + "</notify>";
		ret += "<type>" + 
			string((type == WEAK)? "weak" : "strong") + "</type>";
		ret += "<oside>" + (*oside)->get_key() + "</oside>";
		ret += "<oside_name>" + 
			((_XObject *)(*oside))->get_name() + "</oside_name>";
		ret += "</xobj_connection>";
		return ret;
	}
	string shell_xml() const
	{
		string xml = "<extra_row>";

		xml += "<column>" + cid.value() + "</column>";
		xml += "<column>" + name + "</column>";
		xml += "<column>" + role + "</column>";
		xml += "<column>" + string(notify ? "notify" : "no notify")
			+ "</column>";
		xml += "<column>" + string(type == WEAK ? "weak" : "strong")
			+ "</column>";
		xml += "<column>" + (*oside)->get_key() + "</column>";
		xml += "<column>" + ((_XObject *)(*oside))->get_name()
			+ "</column>";

		xml += "</extra_row>";

		return xml;
	}

	/**
	 * Name of connection.
	 */
	string name;
	/**
	 * Role of other side of connection.
	 */
	string role;
	/**
	 * Do notify other side of conection about changes on this side 
	 * of connection?
	 */
	bool notify;
	/**
	 * Type of connection.
	 */
	ConnectionType type;
	/**
	 * Other side of connection.
	 */
	iterator oside;
	/**
	 * Connection ID.
	 */
	UUIDParam cid;
};

/**
 * \class XObjectScanner
 * Objects from this class would used to scan connects "XObjects".
 *
 * See "XObjects" connected area like of Graph or Tree,
 * Scanner would scan graph in prefix order. Scanner when reaches
 * any object, would call scan function and when return from any object to
 * upper level, "scan_return" would called.
 */
template <typename Type>
class XObjectScanner
{
public:
	typedef XObject<Type> 		_XObject;
	typedef XObjectConnection<Type> _XObjectConnection;

	/**
	 * \enum _ScannerAction
	 * Waht scanner shoud act after scanning one object?
	 */
	enum ScannerAction {
		SA_BACKWARD,	/**< return to upper level and continue 
					scanning. */
		SA_FORWARD,	/**< continue scanning of 
					connections of this object. */
		SA_CONTINUE,	/**< continue scanning in this level. */
		SA_END,		/**< end scanning. */
	};

	XObjectScanner()
	{
		use_name = use_role = use_notify = 
			use_type = use_oside = use_cid = false;
	}
	XObjectScanner(const _XObjectConnection &c)
	{
		use_name = use_role = use_notify = 
			use_type = use_oside = use_cid = false;
		cond = c;
	}
	/**
	 * Scan "xobj".
	 * This the main function would scan target object.
	 */
	virtual ScannerAction scan(_XObject *xobj)
	{ return SA_END; }
	/**
	 * Scanner returned from "xobj" to upper level.
	 * See "XObjects" connected area like of Graph or Tree,
	 * Scanner would scan graph in prefix order.
	 */
	virtual void scan_return(_XObject *xobj)
	{ }
	/**
	 * "match" the "c" connection to see scanner would scan this connection
	 * or not.
	 */
	bool match(const _XObjectConnection c)
	{
		if (use_name 	&& (c.name != cond.name))	return false;
		if (use_role  	&& (c.role != cond.role))	return false;
		if (use_notify 	&& (c.notify != cond.notify)) 	return false;
		if (use_type 	&& (c.type != cond.type))	return false;
		if (use_oside 	&& (c.oside != cond.oside))	return false;
		if (use_cid	&& (c.cid.value() != 
					cond.cid.value()))	return false;
		return true;
	}
	void setCondition(const _XObjectConnection &c)
	{
		cond = c;
	}
	/**
	 * Set "use_*" to "val".
	 * \param val "use_*" value.
	 */
	void setAllUse(bool val = false)
	{
		use_name = use_role = use_notify = 
			use_type = use_oside = use_cid = val;
	}

	/**
	 * Connection condition to traverse xobjects.
	 * Base on value of condition, scanner would decision to traverse a 
	 * connection or not.
	 */
	_XObjectConnection cond;
	/**
	 * "use_*" attribues define whome field in "condition" would be used
	 * for decision in scan.
	 * false: means don't use, or means any value, 
	 * true: means use, only this value should use for target field.
	 */
	bool use_name;
	/** \see use_name.*/
	bool use_role;
	/** \see use_name.*/
	bool use_notify;
	/** \see use_name.*/
	bool use_type;
	/** \see use_name.*/
	bool use_oside;
	/** \see use_name.*/
	bool use_cid;
};

/**
 * \class XObject 
 * Defines XObject attributes.
 *
 * Each XObject is a mix of some parameters that define an object in the
 * system.
 *
 * Each object is XMixParam inherited class and also has some special 
 * parameters: 	xoKey (type of uuid and is object key)
 * 		xoName (name of object)
 * 		xoType (defines type of object)
 *
 * Type Defines "Type" class base on XISetParam requiremens.
 *
 * Type should have some speciall attributes to be operational
 * in XObject.
 * Type should define constructor "Type(string objClass)" that would 
 * receive Object-class at object initialization.
 * Type should define "typedef Type::Literal" that defines the literal
 * type that would be used to set/get type in Type.
 * Also sould implement "set_type(Literal ) and Literal get_type() const" that
 * set and retrieve Type values.
 * Type would be have "XParam *getTypeParam()" that would return
 * pointer to the type-field(XParam) of object.
 *
 * Objects may connect to each other. 
 *
 * Operations on XObjects restricted to: add, mod, del, reload.
 * Also some internal operations are: scan, query.
 * Concurrent operation would be done sequentially. 
 * There is possible multiple and concurrent request to object data,
 * so "XObject"s should manage concurrent access to their data.
 */
template <typename _Type>
class XObject : public XMixParam
{
	friend class XObjectList<_Type>;
public:
	/**
	 * \enum XObjectNotify
	 * Type of notification would be sent to dependent objects.
	 *
	 * By this we define what happend(MODIFY, NONE) on notifier object
	 * and notified object.
	 */
	enum XObjectNotify {
		OBJ_NOTIFY_NONE,   /**< be happy, nothing ... */
		OBJ_NOTIFY_MODIFY, /**< xobject has been modified. */
		OBJ_NOTIFY_MAX,
	};
	/**
	 * \enum XObjectOperation
	 * All operations could be done on XObject.
	 */
	enum XObjectOperation {
		OBJ_OP_ADD,	/**< Add object. */
		OBJ_OP_DEL,	/**< Delete object. */
		OBJ_OP_MOD,	/**< Modify object. */
		OBJ_OP_REL,	/**< Reload object. */
	};
	/**
	 * \typedef Type 
	 * Define type object that is required by XISetParam.
	 */
	typedef _Type Type;
	/**
	 * \typedef TypeLiteral
	 * Literal presentation of Type.
	 */
	typedef typename Type::Literal TypeLiteral;
	/**
	 * \typedef _XObject 
	 * for easy type definition.
	 */
	typedef XObject<Type> _XObject;
	/**
	 * \typedef Status
	 * A synonim for XObjectStatus.
	 */
	typedef XObjectStatus ObjStatus;
	/**
	 * \typedef FT_notify
	 * Type for notify functions.
	 */
	typedef XObjectNotify (*FT_notify)(XObjectNotify nt, 
							const _XObject *xo);
	typedef XObjectConnection<Type>			_XObjectConnection;
	typedef XObjectScanner<Type>			_XObjectScanner;
	typedef typename _XObjectScanner::ScannerAction ScannerAction;
	/** 
	 * \typedef ConnectionList
	 * List of connections and their attributes to this object.
	 */
	typedef XList<_XObjectConnection> 		ConnectionList;
	/**
	 * \typedef c_iterator
	 * Iterator to iter on connections.
	 */
	typedef typename ConnectionList::iterator	c_iterator;
	typedef typename ConnectionList::const_iterator	c_const_iterator;

	/**
	 * \class XObjectNotifier
	 * Scanner would be used for modify notifications.
	 */
	class XObjectNotifier : public _XObjectScanner
	{
	public:
		using _XObjectScanner::use_notify;
		using _XObjectScanner::cond;
		using _XObjectScanner::SA_FORWARD;
		using _XObjectScanner::SA_CONTINUE;

		XObjectNotifier(_XObject *_starter, XObjectNotify _nt) 
				: _XObjectScanner()
		{
			/* Only scan objects that require notification about
			 * modification.
			 */
			use_notify = true;
			cond.notify = true;
			starter = _starter;
			nt = _nt;
		}
		/**
		 * Scan objects and call their "notify" functions.
		 */
		ScannerAction scan(_XObject *xobj)
		{
			if (xobj == starter) return SA_FORWARD;
			
			switch (xobj->notify(nt, starter)) {
			case OBJ_NOTIFY_NONE:
				return SA_CONTINUE;
				break;
			case OBJ_NOTIFY_MODIFY:
				return SA_FORWARD;
				break;
			default:
				return SA_CONTINUE;
			}
		}
		/**
		 * Starter of scanner.
		 */
		_XObject *starter;
		/**
		 * Type of notification.
		 */
		XObjectNotify nt;
	};
	
	XObject(const string &objClass) : XMixParam(objClass), 
			xoKey("uuid"), xoName("name"),
			xoType(objClass),
			xoStatus("status", ObjStatus::LOADED),
			xoStatus_prev("status", ObjStatus::LOADED),
			cListVersion("xobj_clist_version", 0, -1)
	{
		addParam(&xoKey);
		addParam(&xoName);
		addParam(xoType.getTypeParam());
		//addParam(&xoStatus);
		//addParam(&cListVersion);

		xoStatus.set_runtime();
		cListVersion.set_runtime();

		pthread_mutex_init(&obj_status_lock, NULL);
		pthread_cond_init(&obj_status_cond, NULL);
		ruse = 0;
	}

	XObject() : XMixParam("UNNAMED_OBJ_CLASS"),
			xoKey("uuid"), xoName("name"),
			xoType("UNNAMED_OBJ_CLASS"),
			xoStatus("status", ObjStatus::LOADED),
			xoStatus_prev("status", ObjStatus::LOADED),
			cListVersion("xobj_clist_version", 0, -1)
	{
		addParam(&xoKey);
		addParam(&xoName);
		addParam(xoType.getTypeParam());
		//addParam(&xoStatus);
		//addParam(&cListVersion);

		xoStatus.set_runtime();
		cListVersion.set_runtime();

		pthread_mutex_init(&obj_status_lock, NULL);
		pthread_cond_init(&obj_status_cond, NULL);
		ruse = 0;
	}

	bool key(string &_key) const
	{
		_key = xoKey.value();
		return true;
	}
	string get_key() const
	{
		return xoKey.value();
	}
	void set_key(const string key)
	{
		xoKey = key;
	}
	string get_objClass() const
	{
		return get_pname();
	}
	void set_objClass(const string &objClass)
	{
		set_pname(objClass);
	}
	string get_name() const
	{
		return xoName.value();
	}
	void set_name(const string &_name) 
	{
		xoName = _name;
	}
	TypeLiteral get_type() const
	{
		return xoType.get_type();
	}
	void set_type(TypeLiteral t)
	{
		xoType.set_type(t);
	}
	string get_type_string()
	{
		return xoType.value();
	}
	XInt get_status()
	{
		return xoStatus.get_value();
	}
	c_iterator cListEnd()
	{
		return cList.end();
	}
	/**
	 * Should be implemented with leaf-inherited classes.
	 */
	virtual void type(Type &t) const {}
	void set_this_iter(typename XObjectList<Type>::iterator &iter)
	{
		this_iter = iter;
	}
	void this_iter_fini()
	{
		this_iter.fini();
	}
	typename XObjectList<Type>::iterator get_this_iter()
	{
		return this_iter;
	}
	void inc_ruse() 
	{ 
		++ruse; 
	}
	void dec_ruse() 
	{ 
		if (ruse) --ruse; 
	}
	bool is_busy() 
	{ 
		return (ruse)? true : false; 
	}
	/**
	 * Return XObject xml.
	 * It's a modified version of XParam::_xml, that insert some xobject
	 * specific parameters to the xml string.
	 */
	virtual string xobj_xml(bool show_runtime,
			const int &indent, const string &endl) const
	{
		string str = XMixParam::_xml(show_runtime, indent, endl);
		size_t gtPos = str.find('>');
		/* insert object status and connections.
		 */
		str.insert(gtPos + 1, xoStatus_prev.xml(show_runtime)
				+ cListVersion.xml(show_runtime)
				+ xml_connectionsList());
		return str;
	}
	/**
	 * Modified version of _xml() for XObject.
	 *
	 * At xml-generating time, some modification operations
	 * may be active on object, so we should lock him before any
	 * xml generation.
	 */
	virtual string _xml(bool show_runtime,
			const int &indent, const string &endl) const
	{
		string sxml = "";
		if (((_XObject *)this)->chStatus(ObjStatus::PRINTING)) {
			sxml = xobj_xml(show_runtime, indent, endl);
			((_XObject *)this)->bkStatus();
		}
		return sxml;
	}
	string shell_xml()
	{
		string	shellXml = "<row>";
		
		if (((_XObject *)this)->chStatus(ObjStatus::PRINTING)) {
			shellXml += "<columns>";
			shellXml += "<column>" + get_key() + "</column>";
			shellXml += "<column>" + get_name() + "</column>";
			shellXml += "<column>" + xoStatus_prev.value()
				+ "</column>";
			shellXml += "<column>" + xoType.value() + "</column>";
			shellXml += "</columns>";
			shellXml += xml_shellConnectionsList();
			((_XObject *)this)->bkStatus();
		}
		shellXml += "</row>";

		return shellXml;
	}
	/**
	 * XML without any lock.
	 */
	virtual string xml_wlock(bool show_runtime = false) const
	{
		return xobj_xml(show_runtime, 0, "\n");
	}
	/**
	 * Add created object to the system.
	 */
	void add() throw (Exception)
	{
		if (chStatus(ObjStatus::ADDING)) {
			try {
				_add();
			} catch (Exception &e) {
				if (e.is_failed()) {
					bkStatus();
					/* Disconnect any created connection
					 * to this.
					 */
					//disconnectAll();
					/* Finalize self iterator.
					 */
					//this_iter.fini();
					/* Historical Desc:
					 * Don't disconnect and finaliaze 
					 * iter, because adding may be done at
					 * repo load, in this time some 
					 * connections may occure from others
					 * to this, so if disconnect, they
					 * would lose their connectivites.
					 * So can't finalize this_iter.
					 */
				} else if (e.is_nok())
					chStatus(ObjStatus::NORMAL);
				e.addTracePoint(TracePoint("xobject"));
				throw e;
			}
			chStatus(ObjStatus::NORMAL);
		} else {
			throw Exception("Can't prepare object : " + get_name()
					+ " for adding!",
					TracePoint("xobject"));
		}
	}
	/**
	 * Delete object from system.
	 */
	void del() throw (Exception)
	{
		if (chStatus(ObjStatus::DELETING)) {
			if (is_busy()) {
				bkStatus();
				throw Exception("Can't delete busy object !",
							TracePoint("xobject"));
			}
			try {
				_del();
				/* Finalize self iterator.
				 */
				this_iter.fini();
			} catch (Exception &e) {
				if (e.is_nok()) {
					chStatus(ObjStatus::DELETED);
					disconnectAll();
				} else bkStatus();
				e.addTracePoint(TracePoint("xobject"));
				throw e;
			}
			chStatus(ObjStatus::DELETED);
			disconnectAll();
		} else {
			throw Exception("Can't prepare object : " + get_name()
					+ " for deleting!",
					TracePoint("xobject"));
		}
	}
	/**
	 * Prepare object for modification.
	 * \return true: object prepared, false: can't prepare object.
	 *
	 * After success call of this function, "mod" would be called.
	 */
	bool mod_prepare()
	{
		return chStatus(ObjStatus::MODIFYING);
	}
	/**
	 * Modify object attributes in systeam.
	 * \param xo modification target object.
	 */
	void mod(_XObject *xo) throw (Exception)
	{
		try {
			_mod(xo);
			set_name(xo->get_name());
		} catch (Exception &e) {
			bkStatus();
			if (e.is_nok()) {
				set_name(xo->get_name());
				notifyDependentObjects(OBJ_NOTIFY_MODIFY);
			}
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		bkStatus();
		notifyDependentObjects(OBJ_NOTIFY_MODIFY);
	}
	/**
	 * Prepare object for reloading.
	 * \return true: object prepared, false: can't prepare object.
	 *
	 * After success call of this function, "reload" would be called.
	 */
	bool reload_prepare()
	{
		return chStatus(ObjStatus::RELOADING);
	}
	/**
	 * Reload object attributes, would be called after success call of
	 * "reload_prepare".
	 * \param xo target object to reload base on.
	 */
	void reload(_XObject *xo) throw (Exception)
	{
		try {
			*(XParam *)this = *(XParam *)xo;
		} catch (Exception &e) {
			bkStatus();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		bkStatus();
	}
	/**
	 * Start scanning of "this" and its connections base on "s".
	 * \param [in] async If it set to false (default), change the status of
	 * the object ot SCANNING, otherwise, do not change the status of the
	 * object
	 */
	ScannerAction scan(_XObjectScanner &s,const bool async = false)
	{
		if ((!async) && (! chStatus(ObjStatus::SCANNING)))
			return _XObjectScanner::SA_CONTINUE;
		ScannerAction sact = s.scan((_XObject *) *this_iter);
		if (!async)
			bkStatus();

		switch (sact) {
		case _XObjectScanner::SA_FORWARD:
			/* FORWARD received, so scan connections to "this".
			 */
			for (c_iterator iter = cList.begin(); 
					iter != cList.end(); ++iter) {
				if (! s.match(*iter)) continue;

				_XObject *xobj = (_XObject *) *(iter->oside);
				switch (xobj->scan(s)) {
				case _XObjectScanner::SA_CONTINUE:
					/* Continue scanning of "this" 
					 * connections.
					 */
					continue;
					break;
				case _XObjectScanner::SA_BACKWARD:
					/* UP recieved from one 
					 * connections, so shoud return
					 * to parent and continue
					 * scanning objects at "this" 
					 * level.
					 */
					s.scan_return((_XObject *)*this_iter);
					return _XObjectScanner::SA_CONTINUE;
					break;
				case _XObjectScanner::SA_END:
					s.scan_return((_XObject *)*this_iter);
					return _XObjectScanner::SA_END;
					break;
				case _XObjectScanner::SA_FORWARD:
					/* This case doen't occure, because 
					 * scan don't return SA_FORWARD.
					 * Scan handles SA_FORWARD himself.
					 */
					break;
				}
			}
			break;
		case _XObjectScanner::SA_BACKWARD:
			/* Don't continue scanning in this level, return
			 * and continue scanning in parent level.
			 */
			s.scan_return((_XObject *)*this_iter);
			return _XObjectScanner::SA_BACKWARD;
			break;
		case _XObjectScanner::SA_CONTINUE:
			/* continue scanning of objects with mine level.
			*/	
			s.scan_return((_XObject *)*this_iter);
			return _XObjectScanner::SA_CONTINUE;
			break;
		case _XObjectScanner::SA_END:
			s.scan_return((_XObject *)*this_iter);
			return _XObjectScanner::SA_END;
			break;
		}
		s.scan_return((_XObject *)*this_iter);
		return _XObjectScanner::SA_CONTINUE;
	}
	/**
	 * Return list of object connections in "XML" format.
	 */
	string xml_connectionsList() const
	{
		string ret = "<xobj_clist>";
		for (c_const_iterator iter = cList.begin(); 
			iter != cList.end(); ++iter) {
				ret += iter->xml();
		}
		ret += "</xobj_clist>";
		return ret;
	}
	string xml_shellConnectionsList() const
	{
		string xml = "<extra_rows>";

		for (c_const_iterator iter = cList.begin(); 
			iter != cList.end(); ++iter) {
				xml += iter->shell_xml();
		}
		xml += "</extra_rows>";

		return xml;
	}
	/**
	 * Checking equivalency with lvalue object.
	 *
	 * We mean by equivalence, equivalency in some structural parameters
	 * of this objects.
	 * xoKey, xoName and xoType are not structural parameters.
	 */
	virtual bool operator==(const _XObject &lvalue) const
	{
		return false;
	}
	/**
	 * Returns xml-formatted of this and all objects that is 
	 * connect to this object.
	 */
	string xml_connectivityArea()
	{
		string ret = xml(true);
		for (c_iterator iter = cList.begin(); 
					iter != cList.end(); ++iter)
			ret += ((_XObject *)(* iter->oside))->xml(true);
		return ret;
	}
	/**
	 * Find connection with "cid" as connection id.
	 * \param cid connection id of target connection.
	 */
	c_iterator queryConnection(const string &cid)
	{
		for (c_iterator iter = cList.begin(); 
					iter != cList.end(); ++iter)
			if (iter->cid.value() == cid) return iter;
		return cList.end();
	}
	/**
	 * Find first connection that match "s".
	 */
	c_iterator queryConnection(_XObjectScanner &s)
	{
		for (c_iterator iter = cList.begin(); 
					iter != cList.end(); ++iter)
			if (s.match(*iter)) return iter;
		return cList.end();
	}
	/**
	 * Find first connection to "objID" that match "s".
	 * \param objID ID of other side of connection.
	 */
	c_iterator queryConnection(_XObjectScanner &s, const string &objID)
	{
		for (c_iterator iter = cList.begin(); 
					iter != cList.end(); ++iter)
			if (s.match(*iter) && 
				((*(iter->oside))->get_key() == objID))
					return iter;
		return cList.end();
	}
	/**
	 * Find first connection that match "s".
	 * \param citer location that query would start from.
	 */
	c_iterator queryConnection(_XObjectScanner &s, c_iterator citer)
	{
		for (c_iterator iter = citer; 
					iter != cList.end(); ++iter)
			if (s.match(*iter)) return iter;
		return cList.end();
	}

protected:
	/**
	 * Change object status.
	 *
	 * This function would wait to see appropriate object status to
	 */
	bool chStatus(XInt _status)
	{
		pthread_mutex_lock(&obj_status_lock);
		while (1) {
			switch (ObjStatus::canTransit(xoStatus.get_value(),
								_status)) {
			case ObjStatus::DIRECT: /* there is direct transition*/
				setStatus(_status);
				pthread_mutex_unlock(&obj_status_lock);
				return true;
				break;
			case ObjStatus::WAIT: /* would wait, may be ability
						to transit */
				pthread_cond_wait(&obj_status_cond, 
							&obj_status_lock);
				break;
			case ObjStatus::NONE: /* can't transit */
				pthread_mutex_unlock(&obj_status_lock);
				return false;
				break;
			}
		}
	}
	/**
	 * Back object status to previous status.
	 *
	 * With suppose that transition could be done.
	 */
	void bkStatus()
	{
		pthread_mutex_lock(&obj_status_lock);
		setStatus(xoStatus_prev.get_value());
		pthread_mutex_unlock(&obj_status_lock);
	}
	/**
	 * Set object status to specified value.
	 * \note should be called in protected area with "obj_status_lock".
	 */
	void setStatus(XInt _status)
	{
		xoStatus_prev = xoStatus;
		xoStatus = _status;
		/* Say to others, status has been changed.
		 */
		pthread_cond_broadcast(&obj_status_cond);
	}
	/**
	 * Connect to specified object from this object.
	 * \param fromc Connection attributes of this object in connection.
	 * \param toc Connection attributes of other object in connection.
	 *
	 * Information about other side of connection would be 
	 * queried from: "fromc.oside".
	 *
	 * Connection creation would happen in ADDING, MODIFING, NOTIFING
	 * statuses.
	 */
	bool connect(_XObjectConnection &fromc, _XObjectConnection &toc)
	{
		/* Make connection ids identical.
		 */
		fromc.cid = toc.cid.value();
		/* Change other side of connections, because each object
		 * stores connection attributes of other side.
		 */
		toc.oside = fromc.oside;
		fromc.oside = this_iter;
		addConnection(toc);
		if (! ((_XObject *) *(toc.oside))->addConnection(fromc, true)) {
			delConnection(toc.cid.value());
			return false;
		}
		return true;
	}
	/**
	 * Disconnect specified connection.
	 *
	 * Disonnection would happen in MODIFING, NOTIFING, DELETING 
	 * LOADED (after fail in addition) statuses.
	 */
	void disconnect(const string &cid)
	{
		c_iterator iter = queryConnection(cid);
		if (iter != cList.end()) disconnect(iter);
	}
	/**
	 * Disconnect first connection to "objID" that match "s".
	 */
	void disconnect(_XObjectScanner &s, const string &objID)
	{
		c_iterator iter = queryConnection(s, objID);
		if (iter != cList.end()) disconnect(iter);
	}
	/**
	 * Disconnect all connections that match s.
	 */
	void disconnectAll(_XObjectScanner &s)
	{
		c_iterator iter = cList.begin();
		while (iter != cList.end()) {
			if (s.match(*iter)) iter = disconnect(iter);
			else ++iter;
		}
	}
	/**
	 * Diconnecting all of connections to this object.
	 */
	void disconnectAll()
	{
		c_iterator iter = cList.begin();
		while (iter != cList.end()) {
			iter = disconnect(iter);
		}
	}
	c_iterator disconnect(c_iterator &iter)
	{
		string cid = iter->cid.value();
		if (xdelConnection_prepare(iter)) {
			//disconnectNotice(*iter);
			((_XObject *) *(iter->oside))->delConnection(cid);
			return xdelConnection(iter);
		} else return ++ c_iterator(iter);
	}
	/**
	 * \param toSide Add connection at "from" side (false) 
	 * or "to" side (true).
	 */
	bool addConnection(_XObjectConnection &c, bool toSide = false)
	{
		pthread_mutex_lock(&obj_status_lock);
		/* Ask from this that, May connect new connection with
		 * "c" attributes?
		 */
		if (! mayConnect(c)) {
			pthread_mutex_unlock(&obj_status_lock);
			return false;
		}
		while (xoStatus.get_value() == ObjStatus::DELETING)
			/* In DELETING status we should wait to see what would
			 * happen?
			 * If object deleted, can't add new connection, else
			 * may add new connection.
			 */
			pthread_cond_wait(&obj_status_cond, &obj_status_lock);
		if (xoStatus.get_value() == ObjStatus::DELETED) {
			pthread_mutex_unlock(&obj_status_lock);
			return false;
		}
		cList.push_back(c);
		if (c.type == _XObjectConnection::STRONG)
			/* "ruse" or "resource usage" would present
			 * number of strong connections.
			 */
			++ ruse;
		++ cListVersion;
		pthread_mutex_unlock(&obj_status_lock);
		/* Notice about new connection to this at "to" side.
		 */
		if (toSide) connectNotice(c);
		return true;
	}
	/**
	 * May new connection "c" been created?.
	 * Inherited objects, could develope this function to decision
	 * about new connecitons to them.
	 * So, objects could manage connection to them by this function.
	 * Callings to this function are sequential.
	 * \return true: may create connection, false: can't create connection.
	 */
	virtual bool mayConnect(_XObjectConnection &c)
	{
		return true;
	}
	/**
	 * Give notice about new connection to "this".
	 * After new connection creation, notice about new connection would
	 * send to "this".
	 * All inherited objects could develope this function to recieve 
	 * connect notices.
	 * Callings to this function are parallel.
	 */
	virtual void connectNotice(const _XObjectConnection &c)
	{ }
	/**
	 * Delete connection base on connection id.
	 * \param cid id of connection.
	 */
	void delConnection(const string &cid)
	{
		for (c_iterator iter = cList.begin(); 
				iter != cList.end(); ++iter) {
			if (iter->cid.value() == cid) {
				delConnection(iter);
				break;
			}
		}
	}
	/**
	 * Delete connection base on position in connections list.
	 * \param iter connection position in list.
	 */
	c_iterator delConnection(c_iterator &iter)
	{
		if (xdelConnection_prepare(iter)) {
			disconnectNotice(*iter);
			return xdelConnection(iter);
		} else return (++ c_iterator(iter));
	}
	/**
	 * Prepare connection for deletion.
	 */
	bool xdelConnection_prepare(c_iterator &iter)
	{
		bool ret;
		pthread_mutex_lock(&obj_status_lock);
		ret = cList.xerase_prepare(iter);
		if (ret && (iter->type == _XObjectConnection::STRONG))
			-- ruse;
		++ cListVersion;
		pthread_mutex_unlock(&obj_status_lock);
		return ret;
	}
	/**
	 * Erase connection data.
	 * \return iterator on the next connection.
	 */
	c_iterator xdelConnection(c_iterator &iter)
	{
		c_iterator ret = iter;
		++ ret;
		cList.xerase(iter);
		return ret;
	}
	/**
	 * Give notice about a disconnect from "this".
	 * After connection deletion and before erasing of him, notice
	 * about diconnection would send to "this".
	 * All inherited objects could develope this function to recieve 
	 * disconnect notices.
	 */
	virtual void disconnectNotice(const _XObjectConnection &c)
	{ }
	/**
	 * Notify dependent objects from changes in this object.
	 * 
	 * In this function, looping in notification path don't check.
	 * If some connection need notification about any change in its
	 * peer, so we notify him.
	 * Notify would be stopped when modification doen't have any effect
	 * on peers.
	 */
	void notifyDependentObjects(XObjectNotify nt)
	{
		XObjectNotifier notifier(this, nt);
		scan(notifier);
	}
	/**
	 * All leaf-inherited classes that want to catch changes notification
	 * in dependent objects, should implement this function.
	 *
	 * Goal of this function is notification of object from any change in
	 * dependent objects.
	 * By this function we can coach any object changes and do any require 
	 * actions.
	 * At "notify()", target object would be locked by caller object.
	 * "notify()" should report any error by log messages.
	 */
	virtual XObjectNotify notify(XObjectNotify nt, const _XObject *xo)
	{
		return OBJ_NOTIFY_NONE;
	}
	/**
	 * Specific version of add, delete, modify functions that should
	 * be implemented in inherited calsses.
	 */
	virtual void _add()
	{
		throw Exception("Unsupported function for this object !",
						TracePoint("xobject"));
	}
	virtual void _del()
	{
		throw Exception("Unsupported function for this object !",
						TracePoint("xobject"));
	}
	virtual void _mod(_XObject *xo)
	{
		throw Exception("Unsupported function for this object !",
						TracePoint("xobject"));
	}
	/**
	 * key value for object, is a uuid.
	 */
	UUIDParam xoKey;
	/**
	 * name of xobject.
	 */
	XTextParam xoName;
	/**
	 * Type of xobject.
	 */
	Type xoType;
	/**
	 * XObject status in his lifecycle.
	 */
	XEnumParam<XObjectStatus> xoStatus;
	/**
	 * Stores previous status.
	 */
	XEnumParam<XObjectStatus> xoStatus_prev;

	/**
	 * List of connections to other objects.
	 */
	ConnectionList cList;
	/**
	 * Version number of connection list.
	 *
	 * Would increment/decrement when connection added/removed 
	 * to/from list.
	 */
	XIntParam<XUInt> cListVersion;
	/**
	 * Resource Use.
	 *
	 * All of objects that depend to this object can acquire him by 
	 * "inc_ruse()" and release hin my "dec_ruse()".
	 * In object deletion, ruse should be zero.
	 */
	XInt ruse;
	/**
	 * Condition to trace any changes in xoStatus.
	 */
	pthread_cond_t obj_status_cond;
	/**
	 * Lock to manage asynchronus access to xoStatus.
	 */
	pthread_mutex_t obj_status_lock;
	/**
	 * Iterator to this object in object list.
	 */
	typename XObjectList<Type>::iterator this_iter;
};

template <class Type>
class XObjectRepository;

/*y
 * \class XObjectList
 * Manages list of XObject\<type\> and any function on that.
 *
 * Objects store base on their keys and may query/delete base on their
 * keys.
 * On object list we can only have one add()/one del() at one time.
 * mod() operations can occure concurrently.
 * Also base on objects dependencies, concurrent with add()/del()/mod()
 * we can have mod() operations called base on object dependecies.
 */
template <typename Type>
class XObjectList
{
public:
	/**
	 * \typedef _XObject for easy type definition.
	 */
	typedef XObject<Type>			_XObject;
	typedef XObjectList<Type>		_XObjectList;
	typedef typename Type::Literal 		TypeLiteral;
	typedef std::vector<TypeLiteral> 	Priority;

	typedef XListParam<_XObject, string> 	List;
	typedef typename List::iterator 	iterator;
	typedef typename List::const_iterator 	const_iterator;
	typedef typename List::riterator 	riterator;
	typedef typename List::const_riterator 	const_riterator;
	/**
	 * \typedef Status
	 * A synonim for XObjectStatus.
	 */
	typedef XObjectStatus ObjStatus;

	XObjectList(const string &name, const string &logName) : list(name),
		logs(logName)
	{
		list.enable_smap();
		pthread_mutex_init(&dup_lock, NULL);
		pthread_rwlock_init(&list_lock, NULL);
	}

	string get_name() const
	{
		return list.get_pname();
	}
	/**
	 * Load object to the list.
	 *
	 * load means addition to list without "add" function call of
	 * object.
	 * \param clone Do add clone of object or himself?
	 * \return iteartor on the loaded object.
	 */
	iterator loadObj(_XObject *xo, bool clone = true) throw (Exception)
	{
		pthread_mutex_lock(&dup_lock);
		try {
			verifyObjectDuplication(xo);
		} catch (Exception &e) {
			pthread_mutex_unlock(&dup_lock);
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		wrlock();
		iterator ret = end();
		try {
			if (clone) list.addT(*xo);
			else list.addParam(xo);
		} catch (Exception &e) {
			unlock();
			pthread_mutex_unlock(&dup_lock);
			e.addTracePoint(TracePoint("xobject"));
			string err = "Can't create new object from " + 
								xo->get_name();
			logs << LogLevel::ERROR << err;
			throw e;
		}
		/* Going to the loaded object. */
		-- ret;
		unlock();
		pthread_mutex_unlock(&dup_lock);
		return ret;
	}
	/**
	 * Load new object to the list and add.
	 * \param clone Do add to list clone of this object or himself?
	 * \return iteartor on the added object.
	 */
	iterator add(_XObject *xo, bool clone = true) throw (Exception)
	{
		iterator iter;
		try {
			iter = loadObj(xo, clone);
		} catch (Exception &e) {
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		try {
			_add(iter);
		} catch (Exception &e) {
			e.addTracePoint(TracePoint("xobject"));
			if (e.is_failed()) {
				_XObject *nxo = static_cast<_XObject *>
								(*iter);
				/* Finalize nxo->this_iter.
				 */
				nxo->disconnectAll();
				nxo->this_iter_fini();
				list.del(iter);
			} 
			throw e;
		}
		return iter;
	}
	/**
	 * Add loaded object, would be called in repo load.
	 * \param key key of loaded object.
	 * \return iteartor on the added object.
	 */
	iterator add(const string &key) throw (Exception)
	{
		rdlock();
		iterator iter = list.find(key);
		if (iter == end()) {
			unlock();
			throw Exception("Object doesn't exist !", 
							TracePoint("xobject"));
		}
		unlock();
		try {
			_add(iter);
		} catch (Exception &e) {
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		return iter;
	}
	/**
	 * Deleting specified object by key from the list.
	 * \param key key value of target object.
	 * \return iterator on the next object of deleted object.
	 */
	iterator del(const string &key) throw (Exception)
	{
		wrlock();
		iterator iter = list.find(key);
		if (iter == end()) {
			unlock();
			throw Exception("Object doesn't exist !", 
							TracePoint("xobject"));
		}
		unlock();
		try {
			return del(iter);
		} catch (Exception &e) {
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
	}
	/**
	 * Delete object at specified location (iter).
	 * \return iterator on the next object of deleted object.
	 */
	iterator del(iterator &iter) throw (Exception)
	{
		_XObject *obj = static_cast<_XObject *>(*iter);
		try {
			obj->del();
		} catch (Exception &e) {
			e.addTracePoint(TracePoint("xobject"));
			if (e.is_failed()) {
				string err = obj->get_name() + 
					" couldn't be deleted: " +
					e.what();
				logs << LogLevel::ERROR << err;
				throw e;
			} else if (e.is_nok()) {
				string err = obj->get_name() + 
					" was deleted but some errors\
					occured: " +
					e.what();
				logs << LogLevel::INFO << err;
				throw e;
			}
		}
		if (xdel_prepare(iter)) return xdel(iter);
		else return ++ iterator(iter);
	}
	/**
	 * Delete all objects from list.
	 * \return true: all objects has been deleted, 
	 * 		false: some objects couldn't be deleted.
	 */
	bool delAll()
	{
		bool isOK = true;
		int size = priority.size();
		/* deleting types in reverse.
		 */
		for (int i = 0; i < size; ++i)
			isOK = delType(priority.at(size - (1 + i)))
				&& isOK;
		/* delete remaining objects. 
		 */
		isOK = delAllTypes() && isOK;
		return isOK;
	}
	/**
	 * Delete all objects of specified type.
	 */
	bool delType(TypeLiteral type)
	{
		bool isOK = true;
		iterator iter = begin();
		while (iter != end()) {
			_XObject *xobj = (_XObject*)*iter;
			if (xobj->get_type() != type) {
				++ iter;
				continue;
			}
			try {
				iter = del(iter);
			} catch (Exception &e) {
				if (e.is_failed()) {
					isOK = false;
				}
				++ iter;
			}
		}
		return isOK;
	}
	/**
	 * Delete all objects of all types.
	 */
	bool delAllTypes() 
	{
		bool isOK = true;
		iterator iter = begin();
		while (iter != end()) {
			try {
				iter = del(iter);
			} catch (Exception &e) {
				if (e.is_failed()) {
					isOK = false;
				}
				++ iter;
			}
		}
		return isOK;
	}
	/**
	 * Disconnect all connection of objects in this list.
	 */
	void disconnectAll()
	{
		wrlock();
		for (iterator iter = list.begin();
					iter != list.end(); ++iter) {
			((_XObject *)*iter)->disconnectAll();
		}
		unlock();
	}
	/**
	 * Modifying specified object.
	 * Current object would be retrived by xo->key.
	 * 
	 * \param xo target object.
	 * \param reload Do reload object or modify? default is false, modify.
	 * \return iterator on the modified object.
	 */
	iterator mod(_XObject *xo, bool reload = false) throw (Exception)
	{
		rdlock();
		iterator old_obj_iter = query(xo->get_key());
		if (old_obj_iter == end()) {
			unlock();
			throw Exception("Object doesn't exist !",
							TracePoint("xobject"));
		}
		unlock();
		_XObject *old_obj = static_cast<_XObject *>(*old_obj_iter);
		pthread_mutex_lock(&dup_lock);
		try {
			verifyObjectDuplication(xo, old_obj);
		} catch (Exception &e) {
			pthread_mutex_unlock(&dup_lock);
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		if (reload) { 
			if (! old_obj->reload_prepare()) {
				pthread_mutex_unlock(&dup_lock);
				throw Exception("Can't prepare Object "
						"for reloading !",
							TracePoint("xobject"));
			}
		} else if (! old_obj->mod_prepare()) {
			pthread_mutex_unlock(&dup_lock);
			throw Exception("Can't prepare Object "
					"for modifying !",
						TracePoint("xobject"));
		}
		pthread_mutex_unlock(&dup_lock);
		try {
			if (reload) old_obj->reload(xo);
			else old_obj->mod(xo);
		} catch (Exception &e) {
			e.addTracePoint(TracePoint("xobject"));
			if (e.is_failed()) {
				string err = old_obj->get_name() + 
					" couldn't be modified: " +
					e.what();
				logs << LogLevel::ERROR << err;
				throw e;
			} else if (e.is_nok()) {
				string err = old_obj->get_name() + 
					" was modified but some errors\
					occured: " +
					e.what();
				logs << LogLevel::INFO << err;
				throw e;
			}
		}
		return old_obj_iter;
	}
	/**
	 * Query specified object.
	 *
	 * Use when you are sure about lock on list.
	 */
	iterator query(const string &objkey)
	{
		return list.find(objkey);
	}
	/**
	 * Lock the list and query specified object.
	 */
	iterator query_lock(const string &objkey)
	{
		rdlock();
		iterator iter = query(objkey);
		unlock();
		return iter;
	}
	/**
	 * Query all objects of specified type.
	 * 
	 * \return false: can't query all objects, true: all objects queried.
	 */
	bool query_type_lock(TypeLiteral _type, std::vector<iterator> &out)
	{
		rdlock();
		for (iterator iter = list.begin();
					iter != list.end(); ++iter) {
			if (((_XObject *)*iter)->get_type() == _type)
				try {
					out.push_back(iter);
				} catch (std::exception &e) {
					unlock();
					return false;	
				}
		}
		unlock();
		return true;
	}
	/**
	 * Load objects to the list from xml string.
	 * \return true: objects loaded, false: loading canceled.
	 */
	bool loadXmlStr(string xstr, XParam::XmlParser *parser = NULL) 
						throw (Exception)
	{
		if (repo->cancelLoading()) return false;
		wrlock();
		try {
			list.loadXmlStr(xstr, parser);
		} catch(Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
		return true;
	}
	/**
	 * Load objects to the list from xml document.
	 * \return true: objects loaded, false: loading canceled.
	 */
	bool loadXmlDoc(string xdoc, XParam::XmlParser *parser = NULL) 
						throw (Exception)
	{
		if (repo->cancelLoading()) return false;
		wrlock();
		set_xmlDoc(xdoc);
		try {
			list.loadXmlDoc(xdoc, parser);
		} catch(Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
		return true;
	}
	/** 
	 * Add loaded data by load*() functions.
	 *
	 * This function would be called by loadXml*() functions.
	 * \return true: All loaded data added, false: some objects 
	 * 			couldn't be added.
	 */
	bool addLoadedObjects() 
	{
		bool isOK = true;
		int size = priority.size();
		if (size) {
			/* If there is any periority, add based on.
			 */
			for (int i = 0; i < size; ++i) {
				if (repo->cancelLoading()) return false;
				isOK = addLoadedObjectsType(priority.at(i))
					&& isOK;
			}
		} else {
			/* Else add from first to last.
			 */
			isOK = addAllLoadedObjects() && isOK;
		}
		return isOK;
	}
	void saveXmlDoc(const string &xdoc, bool show_runtime = false, 
			const int &indent = 0, bool with_endl = false)
						throw (Exception)
	{
		try {
			list.saveXmlDoc(xdoc, 
					show_runtime, indent, with_endl);
		} catch (Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
	}
	void save() throw (Exception)
	{
		if (has_xmlDoc())
			saveXmlDoc(get_xmlDoc());
	}
	string xml(bool show_runtime = false, 
			const int &indent = 0, bool with_endl = false)
	{
		return list.xml(show_runtime, indent, with_endl);
	}
	string shell_xml()
	{
		iterator	listIterator;
		string		shellXml;
		_XObject	*object;

		for (listIterator = begin(); listIterator != end();
				listIterator++) {
			object = dynamic_cast<_XObject*>(*listIterator);
			if (!object)
				continue;
			shellXml += object->shell_xml();
		}

		return shellXml;
	}
	void set_xmlDoc(string xdoc)
	{
		xmlDoc = xdoc;
	}
	string get_xmlDoc()
	{
		return xmlDoc;
	}
	bool has_xmlDoc()
	{
		return ! get_xmlDoc().empty();
	}
	void set_priority(const Priority &p)
	{
		priority = p;
	}
	Priority get_priority()
	{
		return priority;
	}
	/**
	 * By this we can define type periorities by pushing 
	 * them from first to last.
	 */
	void pushPriority(TypeLiteral type)
	{
		priority.push_back(type);
	}
	void wrlock()
	{
		pthread_rwlock_wrlock(&list_lock);
	}
	void rdlock()
	{
		pthread_rwlock_rdlock(&list_lock);
	}
	void unlock()
	{
		pthread_rwlock_unlock(&list_lock);
	}
	iterator begin()
	{
		return list.begin();
	}
	iterator end()
	{
		return list.end();
	}
	const_iterator begin() const
	{
		return list.begin();
	}
	const_iterator end() const
	{
		return list.end();
	}
	const_iterator const_begin() const
	{
		return list.const_begin();
	}
	const_iterator const_end() const
	{
		return list.const_end();
	}
	bool load()
	{
		try {
			if (has_xmlDoc())
				return loadXmlDoc(get_xmlDoc());
		} catch (Exception &e) {
			string err = "Can't load " + get_name() + "list !:" +
					e.what();
			logs << LogLevel::ERROR << err;
			return false;
		}
		return true;
	}
	bool unload()
	{
		return delAll();
	}
	void set_repo(XObjectRepository<Type> *_repo)
	{
		repo = _repo;
	}

protected:
	/**
	 * List of XObjects.
	 */
	List list;
	/**
	 * lock to manage object list.
	 */
	pthread_rwlock_t list_lock;
	/**
	 * Type periorities to load objects.
	 * By default list would be loaded from first periority to last
	 * and would be unloaded from last to first if be any priority,
	 * else is priority list be empty, load process loads objects from 
	 * first object in the list to last.
	 */
	Priority priority;
	/**
	 * XML formatted document to load/save list.
	 */
	string xmlDoc;
	/**
	 * Log system of list.
	 */
	LogSystem logs;
	/**
	 * Repository containor of this list.
	 */
	XObjectRepository<Type> *repo;

private:
	/**
	 * Prepare deletion of object.
	 */
	bool xdel_prepare(iterator &iter) throw(Exception)
	{
		bool ret;
		wrlock();
		ret = list.xdel_prepare(iter);
		unlock();
		return ret;
	}
	/**
	 * Freeing object.
	 * \return iterator on the next element.
	 */
	iterator xdel(iterator &iter)
	{
		iterator ret = iter;
		++ ret;
		list.xdel(iter);
		return ret;
	}
	/**
	 * Verify to see "new_obj" is equal to any object in list.
	 * \param new_obj Pointer to new object would be added to list.
	 * \param old_obj Pointer to old version of "new_obj" that is 
	 * currently in list.
	 * 
	 * If any duplication found, Exception would arised.
	 */
	void verifyObjectDuplication(_XObject *new_obj, 
					_XObject *old_obj = NULL) 
							throw (Exception)
	{
		for (iterator iter = list.begin();
					iter != list.end(); ++iter) {
			_XObject *xobj = (_XObject *) *iter;
			if (xobj == old_obj) continue;
			if (xobj->chStatus(ObjStatus::QUERYING)) {
				if ((*xobj == *new_obj) || 
					(xobj->get_key() == 
							new_obj->get_key())) {
					xobj->bkStatus();
					throw Exception("Duplicated Object !"
							" Object name: "
							+ new_obj->get_name(),
							TracePoint("xobject"));
				}
				xobj->bkStatus();
			}
		}
	}
	bool addAllLoadedObjects()
	{
		bool isOK = true;
		for (iterator iter = list.begin();
					iter != list.end(); ++iter) {
			if (repo->cancelLoading()) return false;
			try {
				_add(iter);
			} catch (Exception &e) {
				/* error has been loged, so discards him.
				 */
				if (e.is_failed()) isOK = false;
			}
		}
		return isOK;
	}
	bool addLoadedObjectsType(TypeLiteral _type) 
	{
		bool isOK = true;
		for (iterator iter = list.begin();
					iter != list.end(); ++iter) {
			_XObject *xobj = (_XObject *)*iter;
			if (repo->cancelLoading()) return false;
			if (xobj->get_type() != _type) continue;
			try {
				_add(iter);
			} catch (Exception &e) {
				if (e.is_failed()) isOK = false;
			}
		}
		return isOK;
	}
	void _add(iterator &iter) throw (Exception)
	{
		_XObject *nobj = static_cast<_XObject *>(*iter);
		try {
			nobj->set_this_iter(iter);
			nobj->add();
		} catch (Exception &e) {
			e.addTracePoint(TracePoint("xobject"));
			if (e.is_failed()) {
				string err = nobj->get_name() + 
					" couldn't be added: " +
					e.what();
				logs << LogLevel::INFO << err;
				throw e;
			} else if (e.is_nok()) {
				nobj->set_this_iter(iter);
				string err = nobj->get_name() + 
					" was added with error: " +
					e.what();
				logs << LogLevel::INFO << err;
				throw e;
			}
		}
	}

	/**
	 * Lock to manage concurrency on object duplication verification.
	 * By this lock, we would have just one duplication verification
	 * process at a time.
	 */
	pthread_mutex_t dup_lock;
};

/**
 * \class XObjectRepository
 * Repository of XObjectLists.
 * 
 * Dependency on XObjectList means: May be exist some dependency between 
 * objects of different lists.
 * To manage this dependency without any side effect, we manage this set of
 * lists by one rwlock.
 * We may split objects in different lists, because of more speed in objects
 * search or access.
 */
template <typename Type>
class XObjectRepository
{
public:
	typedef XObjectList<Type> 			_XObjectList;
	typedef XObject<Type> 				_XObject;
	typedef std::map<XParam::XInt, _XObjectList *> 	XORepo;
	/* iterators to traverse objects in lists. */
	typedef typename _XObjectList::iterator		iterator;
	typedef typename _XObjectList::const_iterator	const_iterator;
	typedef typename _XObjectList::riterator	riterator;
	typedef typename _XObjectList::const_riterator	const_riterator;
	/* iterators to traverse object lists. */
	typedef typename XORepo::iterator 		list_iterator;
	typedef typename XORepo::reverse_iterator 	list_riterator;
	typedef typename Type::Literal 			TypeLiteral;
	typedef XParam::XInt 				ListID;
	typedef bool (*FT_cancelLoading)();

	/**
	 * \param _name Name of object repository.
	 * \param logName Subsystem name for log messages.
	 */
	XObjectRepository(string _name, string logName) : name(_name),
		logs(logName)
	{
		pthread_rwlock_init(&rw_lock, NULL);
		_cancelLoading = NULL;
	}
	/**
	 * \param listID ID of list in repository, must be unique.
	 * Repository uses this ID to achive order of loading/unloading lists.
	 * We would load lists from smallest ID to biggest and unload from 
	 * biggest to smallest.
	 */
	_XObjectList *regObjList(ListID listID, const string &name, 
			_XObjectList *_xolist = NULL) throw (Exception)
	{
		wrlock();
		if (repo.find(listID) != repo.end()) {
			unlock();
			throw Exception("Duplicate list !", 
						TracePoint("xobject"));
		}
		_XObjectList *xolist = (_xolist)? _xolist : 
			new _XObjectList(name, logs.get_subSystemName());
		if (xolist) {
			repo[listID] = xolist;
			xolist->set_repo(this);
		} else {
			unlock();
			throw Exception("Can't allocate new list !",
						TracePoint("xobject"));
		}
		unlock();
		return xolist;
	}
	void unregObjList(ListID listID) throw (Exception)
	{
		list_iterator iter;
		wrlock();
		try {
			iter = findList(listID);
			iter->second->unload();
		} catch (Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		delete iter->second;
		repo.erase(iter);
		unlock();
	}
	/**
	 * Load new object to the list.
	 * \return iterator on the loaded object.
	 */
	iterator loadObj(ListID listID, _XObject *xo, bool clone = true) 
							throw (Exception)
	{
		iterator objiter;
		rdlock();
		try {
			list_iterator iter = findList(listID);
			objiter = iter->second->loadObj(xo, clone);
		} catch (Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
		return objiter;
	}
	/**
	 * Reloading target object and add him to the system.
	 * \return iterator on reloded object.
	 */
	iterator reloadObj(ListID listID, _XObject *xo) throw (Exception)
	{
		iterator objiter;
		rdlock();
		try {
			list_iterator iter = findList(listID);
			/* reload. */
			iter->second->mod(xo, true);
			/* add. */
			objiter = iter->second->add(xo->get_key());
		} catch (Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
		return objiter;
	}
	/**
	 * Load and add object.
	 * \return iterator in added object.
	 */
	iterator add(ListID listID, _XObject *xo, bool clone = true) 
							throw (Exception)
	{
		iterator objiter;
		rdlock();
		try {
			list_iterator iter = findList(listID);
			objiter = iter->second->add(xo, clone);
		} catch (Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
		return objiter;
	}
	/**
	 * Add loaded object.
	 * \return iterator on added object.
	 */
	iterator add(ListID listID, const string &key) throw (Exception)
	{
		iterator objiter;
		rdlock();
		try {
			list_iterator iter = findList(listID);
			objiter = iter->second->add(key);
		} catch (Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
		return objiter;
	}
	/**
	 * Delete specified object from system.
	 * \return iterator on the next object of deleted object.
	 */
	iterator del(ListID listID, const string &key)
	{
		iterator objiter;
		rdlock();
		try {
			list_iterator iter = findList(listID);
			objiter = iter->second->del(key);
		} catch (Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
		return objiter;
	}
	/**
	 * Modify specified object.
	 * \return iterator in modified object.
	 */
	iterator mod(ListID listID, _XObject *xo)
	{
		iterator objiter;
		rdlock();
		try {
			list_iterator iter = findList(listID);
			objiter = iter->second->mod(xo);
		} catch (Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
		return objiter;
	}
	bool loadXmlStr(ListID listID, const string &xstr,
			XParam::XmlParser *parser = NULL) 
					throw (Exception)
	{
		bool ret;
		rdlock();
		try {
			list_iterator iter = findList(listID);
			ret = iter->second->loadXmlStr(xstr, parser);
		} catch(Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
		return ret;
	}
	bool loadXmlDoc(ListID listID, const string &xdoc,
			XParam::XmlParser *parser = NULL)
						throw (Exception)
	{
		bool ret;
		rdlock();
		try {
			list_iterator iter = findList(listID);
			ret = iter->second->loadXmlDoc(xdoc, parser);
		} catch(Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
		return ret;
	}
	/** 
	 * Add loaded data by load*() functions.
	 *
	 * This function would be called by loadXml*() functions.
	 * \return true: All loaded data added, false: some objects 
	 * 			couldn't be added.
	 */
	bool addLoadedObjects(ListID listID) throw (Exception)
	{
		bool ret;
		rdlock();
		try {
			list_iterator iter = findList(listID);
			ret = iter->second->addLoadedObjects();
		} catch(Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
		return ret;
	}
	bool addLoadedObjects()
	{
		bool isOK = true;
		rdlock();
		for (list_iterator iter = repo.begin(); 
					iter != repo.end(); ++iter)
			isOK = iter->second->addLoadedObjects() && isOK;
		unlock();
		return isOK;
	}
	void saveXmlDoc(ListID listID, const string &xdoc, 
			bool show_runtime = false, 
			const int &indent = 0, bool with_endl = false)
						throw (Exception)
	{
		rdlock();
		try {
			list_iterator iter = findList(listID);
			iter->second->saveXmlDoc(xdoc, 
					show_runtime, indent, with_endl);
		} catch(Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
	}
	string xml(ListID listID, bool show_runtime = false, 
			const int &indent = 0, bool with_endl = false)
	{
		string ret;
		rdlock();
		try {
			list_iterator iter = findList(listID);
			ret = iter->second->xml(show_runtime, 
							indent, with_endl);
		} catch(Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
		return ret;
	}
	string xml(bool show_runtime = false, 
			const int &indent = 0, bool with_endl = false)
	{
		rdlock();
		string str = string("<") + name + ">";
		try {
			for (list_iterator iter = repo.begin(); 
						iter != repo.end(); ++iter)
				str += iter->second->xml(show_runtime, 
							indent, with_endl);
		} catch (Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		str += string("</") + name + ">";
		unlock();
		return str;
	}
	string shell_xml()
	{
		rdlock();
		string str = "<rows>";
		try {
			for (list_iterator iter = repo.begin(); 
						iter != repo.end(); ++iter)
				str += iter->second->shell_xml();
		} catch (Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		str += "</rows>";
		unlock();
		return str;
	}
	void save()
	{
		rdlock();
		for (list_iterator iter = repo.begin(); 
					iter != repo.end(); ++iter) {
			try {
				iter->second->save();
			} catch (Exception &e) {
				string error = "can't save " + 
					iter->second->get_name() + ": " +
					e.what();
				logs << LogLevel::ERROR << error;
			}
		}
		unlock();
	}
	void save(ListID listID) throw (Exception)
	{
		rdlock();
		try {
			list_iterator iter = findList(listID);
			iter->second->save();
		} catch (Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("mngr"));
			throw e;
		}
		unlock();
	}
	/**
	 * Loading object repository.
	 *
	 * This function would first load all objects and then add them.
	 * Loading/Adding from smallest listID to biggest.
	 */
	bool load()
	{
		bool isOK = true;
		rdlock();
		for (list_iterator iter = repo.begin(); 
					iter != repo.end(); ++iter)
			isOK = iter->second->load() && isOK;
		for (list_iterator iter = repo.begin(); 
					iter != repo.end(); ++iter)
			isOK = iter->second->addLoadedObjects() && isOK;
		unlock();
		return isOK;
	}
	/**
	 * Unloading object repository.
	 *
	 * Unloading from biggest listID to smallest (reverse order 
	 * of loading).
	 * Unloading would be done in two step:
	 * 	1: All connections would be disconnected.
	 * 	2: All objects would be deleted.
	 */
	bool unload()
	{
		bool isOK = true;
		rdlock();
		/* disconnect connections.
		 */
		for (list_riterator riter = repo.rbegin();
					riter != repo.rend(); ++riter)
			riter->second->disconnectAll();
		/* delete objects.
		 */
		for (list_riterator riter = repo.rbegin();
					riter != repo.rend(); ++riter)
			isOK = riter->second->unload() && isOK;

		rdlock();
		return isOK;
	}
	/**
	 * Unload specified list.
	 */
	bool unload(ListID listID) throw (Exception)
	{
		bool ret;
		rdlock();
		try {
			list_iterator iter = findList(listID);
			ret = iter->second->unload();
		} catch (Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
		return ret;
	}
	/**
	 * Return list of object connections in "XML" format.
	 */
	string xml_objConnectionsList(const string &objKey) 
							throw (Exception)
	{
		iterator iter;
		if (query_lock_locklst(objKey, iter))
			return ((_XObject *)*iter)->xml_connectionsList();
		else
			throw Exception("Object doesn't exist",
						TracePoint("xobject"));
	}
	/** 
	 * Query listID without any lock on repository.
	 * \return iterator on found object.
	 */
	iterator query(ListID listID, const string &objkey) throw (Exception)
	{
		try {
			list_iterator iter = findList(listID);
			return iter->second->query(objkey);
		} catch(Exception &e) {
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
	}
	/** 
	 * Query listID with read lock on repository.
	 * \return iterator on found object.
	 */
	iterator query_lock(ListID listID, const string &objkey) 
						throw (Exception)
	{
		iterator ret;
		rdlock();
		try {
			ret = query(listID, objkey);
		} catch(Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
		return ret;		
	}
	/** 
	 * Query listID without any lock on repository and read lock on 
	 * list.
	 * \return iterator on found object.
	 */
	iterator query_locklst(ListID listID, const string &objkey)
							throw (Exception)
	{
		try {
			list_iterator iter = findList(listID);
			return iter->second->query_lock(objkey);
		} catch(Exception &e) {
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
	}
	/** 
	 * Query listID with read lock on repository & list.
	 * \param [in] objkey Key of target object to be found.
	 * \return iterator on found object, if iterator.is_end() be true, 
	 * means object not found, else if be false, iterator locates on 
	 * found object.
	 */
	iterator query_lock_locklst(ListID listID, const string &objkey)
							throw (Exception)
	{
		iterator ret;
		rdlock();
		try {
			ret = query_locklst(listID, objkey);
		} catch(Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
		return ret;
	}
	/**
	 * Query hole of object repository for specified object.
	 * \param [in] objkey Key of target object.
	 * \param [out] objiter iterator on target object if found.
	 * \return true: object found, false: object not found.
	 */
	bool query_lock_locklst(const string &objkey, iterator &objiter)
	{
		rdlock();
		for (list_iterator iter = repo.begin(); iter != repo.end();
								++ iter) {
			objiter = iter->second->query_lock(objkey);
			if (objiter != iter->second->end()) {
				unlock();
				return true;
			}
		}
		unlock();
		return false;
	}
	/**
	 * Query hole of object repository for specified object.
	 * \param [in] objkey Key of target object.
	 * \return iterator on found object, if iterator.is_end() be true, 
	 * means object not found, else if be false, iterator locates on 
	 * found object.
	 */
	iterator query_lock_locklst(const string &objkey)
	{
		iterator objiter;
		rdlock();
		for (list_iterator iter = repo.begin(); iter != repo.end();
								++ iter) {
			objiter = iter->second->query_lock(objkey);
			if (! objiter.is_end()) {
				unlock();
				return objiter;
			}
		}
		unlock();
		return objiter;
	}
	/** 
	 * Query objects of specified type in listID by read lock on list.
	 * \param [in] _type defines type of target objects.
	 * \param [out] out list of found objects of type "_type".
	 */
	void query_type_locklst(ListID listID, TypeLiteral _type, 
						std::vector<iterator> &out)
	{
		try {
			list_iterator iter = findList(listID);
			iter->second->query_type_lock(_type, out);
		} catch(Exception &e) {
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
	}
	/**
	 * Query objects of specified type in listID by read lock on 
	 * repository & list.
	 * \param [in] _type defines type of target objects.
	 * \param [out] out list of found objects of type "_type".
	 */
	void query_type_lock_locklst(ListID listID, TypeLiteral _type, 
						std::vector<iterator> &out)
	{
		rdlock();
		try {
			list_iterator iter = findList(listID);
			iter->second->query_type_lock(_type, out);
		} catch(Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		unlock();
	}
	void unlock()
	{
		pthread_rwlock_unlock(&rw_lock);
	}
	void rdlock()
	{
		pthread_rwlock_rdlock(&rw_lock);
	}
	void wrlock()
	{
		pthread_rwlock_wrlock(&rw_lock);
	}
	list_iterator begin() { return repo.begin(); }
	list_iterator end() { return repo.end(); }
	/**
	 * Return iterator to the begin of the list.
	 * \param listID ID of target list.
	 */
	iterator begin(ListID listID) throw (Exception)
	{
		rdlock();
		list_iterator iter;
		try {
			iter = findList(listID);
		} catch (Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		iterator objiter = iter->second->begin();
		unlock();
		return objiter;
	}
	/**
	 * Return iterator to the end of the list.
	 * \param listID ID of target list.
	 */
	iterator end(ListID listID) throw (Exception)
	{
		rdlock();
		list_iterator iter;
		try {
			iter = findList(listID);
		} catch (Exception &e) {
			unlock();
			e.addTracePoint(TracePoint("xobject"));
			throw e;
		}
		iterator objiter = iter->second->end();
		unlock();
		return objiter;
	}
	/**
	 * Would we cancel loading process of repository?
	 */
	bool cancelLoading()
	{
		if (_cancelLoading) return _cancelLoading();
		else return false;
	}
	void set_cancelLoading(FT_cancelLoading n_cancelLoading)
	{
		_cancelLoading = n_cancelLoading;
	}

protected:
	/**
	 * Find list with specified ID (listID).
	 */
	list_iterator findList(ListID listID) throw (Exception)
	{
		list_iterator iter = repo.find(listID);
		if (iter == repo.end()) {
			throw Exception("Object List doesn't exist !",
							TracePoint("xobject"));
		}
		return iter;
	}

	/**
	 * Name of object repository.
	 */
	string name;
	/**
	 * Log-system to send log messages.
	 */
	LogSystem logs;
	/**
	 * Repository of XObjectsLists.
	 */
	XORepo repo;
	/**
	 * Read/Write lock to manager "repo".
	 */
	pthread_rwlock_t rw_lock;
	/**
	 * User defined function to decide cancel of loading process.
	 */
	FT_cancelLoading _cancelLoading;
};

} // namespace pparam

#endif // _PDN_XOBJECT_HPP_
