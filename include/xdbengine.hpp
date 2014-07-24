/**
 * \file xdbengine.hpp
 * defines classes and structures required for storing xparam in database.
 *
 * Copyright 2010 PDNSoft Co. (www.pdnsoft.com)
 * \author ali esmaeilpour (esmaeilpour@pdnsoft.com)
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

#ifndef _PDN_XDBENGINE_HPP_
#define _PDN_XDBENGINE_HPP_

#include <iostream>
#include <pthread.h>
#include "sqlite3.h"

#include "exception.hpp"

#include <stdio.h>
#include <string>
#include <sstream>
using std::string;

#include <vector>
using std::vector;

namespace pparam
{

typedef vector<string> stringList;

enum DBFieldTypes
{
	DBINTEGER, DBFLOAT, DBTEXT, DBDATETIME, DBBOOLEAN
};

/**
 * \class XDBEngine
 * abstract class, defines common attributes/functions of database engines.
 */
class XDBEngine
{
public:
	virtual void connect(string connectionString) = 0;
	virtual void disconnect() = 0;
	virtual void execute(string command) = 0;

	virtual void startTransaction() = 0;
	virtual void commitTransaction() = 0;
	virtual void rollbackTransaction() throw (Exception) = 0;

	virtual void saveXParam(string pname, string pkey, string parentName,
		string parentKey, stringList fields, stringList values) = 0;
	virtual void saveXParam(string pname, string pkey, stringList fields,
		stringList values) = 0;
	virtual void updateXParam(string pname, string pkey, string parentName,
		string parentKey, stringList fields, stringList values) = 0;
	virtual void updateXParam(string pname, string pkey, stringList fields,
		stringList values) = 0;
	virtual void removeXParam(string pname, string pkey, string parentName,
		string parentKey) = 0;
	virtual void removeXParam(string pname, string pkey) = 0;
	virtual void removeXParamByParent(string pname, string parentName,
		string parentKey) = 0;
	virtual void createXParamStructure(string pname, string parentName,
		stringList fields, vector<DBFieldTypes> fieldTypes) = 0;
	virtual void createXParamStructure(string pname, stringList fields,
		vector<DBFieldTypes> fieldTypes) = 0;
	virtual void destroyXParamStructure(string pname) = 0;

	virtual int loadXParamRow(string pname, string pkey, string parentName,
		string parentKey, stringList &fields, stringList &values) = 0;
	virtual int loadXParamRow(string pname, string pkey, stringList &fields,
		stringList &values) = 0;
	virtual int loadXParamValueListByParent(string pname, string parentName,
		string parentKey, string fieldName, stringList &values) = 0;
	virtual int loadXParamKeyListByParent(string pname, string parentName,
		string parentKey, stringList &values)
	{
		return loadXParamValueListByParent(pname, parentName, parentKey,
			pname + "_key", values);
	}
	virtual void getData(string selectstmt,
		vector<vector<string> > &results, vector<string> &columns) = 0;
	virtual bool backup(string dest) = 0;
	virtual void cleanup() = 0;
	virtual bool isOnTransaction() = 0;
	virtual string DBTypetoString(DBFieldTypes t) = 0;
	virtual bool isConnected() = 0;
};

class SQLiteDBEngine: public XDBEngine
{
public:
	SQLiteDBEngine();
	virtual ~SQLiteDBEngine();

	virtual void connect(string connectionString) throw (Exception);
	virtual void disconnect();
	virtual void execute(string command) throw (Exception);

	virtual void startTransaction();
	virtual void commitTransaction() throw (Exception);
	virtual void rollbackTransaction() throw (Exception);

	virtual void saveXParam(string pname, string pkey, string parentName,
		string parentKey, stringList fields, stringList values)
			throw (Exception);
	virtual void saveXParam(string pname, string pkey, stringList fields,
		stringList values);
	virtual void updateXParam(string pname, string pkey, string parentName,
		string parentKey, stringList fields, stringList values)
			throw (Exception);
	virtual void updateXParam(string pname, string pkey, stringList fields,
		stringList values);
	virtual void removeXParam(string pname, string pkey, string parentName,
		string parentKey);
	virtual void removeXParam(string pname, string pkey);
	virtual void removeXParamByParent(string pname, string parentName,
		string parentKey);
	virtual void createXParamStructure(string pname, string parentName,
		stringList fields, vector<DBFieldTypes> fieldTypes)
			throw (Exception);
	virtual void createXParamStructure(string pname, stringList fields,
		vector<DBFieldTypes> fieldTypes);
	virtual void destroyXParamStructure(string pname);

	virtual int loadXParamRow(string pname, string pkey, string parentName,
		string parentKey, stringList &fields, stringList &values)
			throw (Exception);
	virtual int loadXParamRow(string pname, string pkey, stringList &fields,
		stringList &values);
	virtual int loadXParamValueListByParent(string pname, string parentName,
		string parentKey, string fieldName, stringList &values)
			throw (Exception);
	virtual void getData(string selectstmt,
		vector<vector<string> > &results, vector<string> &columns)
			throw (Exception);
	virtual bool isOnTransaction()
	{
		return onTransaction;
	}
	virtual bool isConnected()
	{
		return isconnected;
	}
	virtual bool backup(string dest);
	virtual void cleanup();
	virtual string DBTypetoString(DBFieldTypes t);
protected:
	sqlite3 *dbp;
	pthread_key_t transactionBufferTSMKey;
	bool onTransaction, isconnected;
};

class XDBCondition
{
public:
	XDBCondition()
	{
		this->clearConditions();
	}
	virtual void addConditionEqual(string field, string value,
		bool inverse = false)
	{
		if (!inverse)
			addCondition(field + "='" + value + "'");
		else
			addCondition(field + "<>'" + value + "'");
	}
	virtual void addConditionGreaterThan(string field, string value,
		bool inverse = false)
	{
		if (!inverse)
			addCondition(field + ">'" + value + "'");
		else
			addCondition(field + "<='" + value + "'");
	}
	virtual void addConditionLessThan(string field, string value,
		bool inverse = false)
	{
		if (!inverse)
			addCondition(field + "<'" + value + "'");
		else
			addCondition(field + ">='" + value + "'");
	}
	virtual void addConditionGreaterThanOrEqual(string field, string value,
		bool inverse = false)
	{
		if (!inverse)
			addCondition(field + ">='" + value + "'");
		else
			addCondition(field + "<'" + value + "'");
	}
	virtual void addConditionLessThanOrEqual(string field, string value,
		bool inverse = false)
	{
		if (!inverse)
			addCondition(field + "<='" + value + "'");
		else
			addCondition(field + ">'" + value + "'");
	}
	virtual void addConditionLike(string field, string value, bool inverse =
		false)
	{
		if (!inverse)
			addCondition(field + " LIKE '" + value + "'");
		else
			addCondition(field + " NOT LIKE '" + value + "'");
	}
	virtual void addConditionBetween(string field, string startv,
		string endv, bool inverse = false)
	{
		if (!inverse)
			addCondition(
				field + " BETWEEN '" + startv + "' AND '" + endv
					+ "'");
		else
			addCondition(
				field + " NOT BETWEEN '" + startv + "' AND '"
					+ endv + "'");
	}
	virtual void addConditionIn(string field, vector<string> values,
		int num, bool inverse = false)
	{
		std::stringstream buff;
		for (unsigned int i = 0; i < values.size(); i++)
			buff << values[i];
		this->addConditionIn(field, buff.str(), inverse);
	}
	virtual void addConditionIn(string field, string *values, int num,
		bool inverse = false)
	{
		std::stringstream buff;
		for (int i = 0; i < num; i++)
			buff << values[i];
		this->addConditionIn(field, buff.str(), inverse);
	}
	virtual void addConditionIn(string field, string values, bool inverse =
		false)
	{
		if (!inverse)
			addCondition(field + " IN (" + values + ")");
		else
			addCondition(field + " NOT IN (" + values + ")");
	}
	virtual void addCondition(string condition)
	{
		if (_conditions.tellp())
			_conditions << " AND " << condition;
		else
			_conditions << condition;
	}
	virtual void setConditions(string conditions)
	{
		_conditions.clear();
		_conditions << conditions;
	}
	virtual string getConditions()
	{
		return _conditions.str();
	}
	virtual void clearConditions()
	{
		_conditions.clear();
	}
protected:
	std::stringstream _conditions;
};
} //namespace pparam

#endif //_PDN_XDBENGINE_HPP_
