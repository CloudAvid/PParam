#include "xdbengine.hpp"
#include <sqlite3.h>
#include <iostream>
#include <pthread.h>

using std::stringstream;
using std::cout;

//#define SQLDEBUG 0;

namespace pparam
{
//impelemtation of SQLiteDBEngine

void cleanTBuffer(void *ptr)
{
	stringList *clist = (stringList*) ((ptr));
	delete clist;
}

SQLiteDBEngine::SQLiteDBEngine()
{
	onTransaction = isconnected = false;
	//init (thread specific) transaction buffer
	pthread_key_create(&transactionBufferTSMKey, cleanTBuffer);
}

SQLiteDBEngine::~SQLiteDBEngine()
{
	pthread_key_delete(transactionBufferTSMKey);
}

void SQLiteDBEngine::connect(string fileName) throw (Exception)
{
	if (sqlite3_open(fileName.c_str(), &dbp)) {
		stringstream buff;
		buff << "Can't open database: %s\n" << sqlite3_errmsg(dbp);
		sqlite3_close(dbp);
		throw Exception(buff.str(), TracePoint("SQLiteDBEngine"));
	}
	isconnected = true;
}

void SQLiteDBEngine::disconnect()
{
	if (onTransaction)
		rollbackTransaction();
	sqlite3_close(dbp);
	isconnected = false;
}
void SQLiteDBEngine::execute(string command) throw (Exception)
{
	if (onTransaction) {
#ifdef SQLDEBUG
		cout << "\nDB q :" << command.c_str();
#endif
		void* raw = pthread_getspecific(transactionBufferTSMKey);
		stringList *clist;
		if (raw == NULL)
			clist = new stringList;
		else
			clist = (stringList*) raw;
		clist->push_back(command);
		pthread_setspecific(transactionBufferTSMKey, clist);
	} else {
#ifdef SQLDEBUG
		cout << "\nDB X :" << command.c_str();
#endif
		char *zErrMsg = 0;
		int rc;
		stringstream buff;
		rc = sqlite3_exec(dbp, command.c_str(), NULL, 0, &zErrMsg);
		if (rc != SQLITE_OK) {
			buff << "SQL error: " << zErrMsg;
			sqlite3_free(zErrMsg);
			throw Exception(buff.str(),
				TracePoint("SQLiteDBEngine"));
		}
	}
}

void SQLiteDBEngine::startTransaction()
{
#ifdef SQLDEBUG
	cout << "\nDB q s";
#endif
	onTransaction = true;
}

void SQLiteDBEngine::commitTransaction() throw (Exception)
{
#ifdef SQLDEBUG
	cout << "\nDB q c";
#endif
	if (onTransaction == false)
		return;

	void* raw = pthread_getspecific(transactionBufferTSMKey);
	stringList* clist;
	if (raw != NULL) {
		clist = (stringList*) raw;
		char *zErrMsg = 0;
		int rc;
		stringstream cmds, buff;
		cmds << "BEGIN TRANSACTION; ";
		for (unsigned int i = 0; i < clist->size(); i++) {
			string cmd = (*clist)[i];
			cmds << cmd
				<< (cmd.find(";") == string::npos ? ";" : "");
		}
		cmds << "COMMIT TRANSACTION; ";
		rc = sqlite3_exec(dbp, cmds.str().c_str(), NULL, 0, &zErrMsg);
#ifdef SQLDEBUG
		cout << "\n------------\n" << cmds.str().c_str()
		<< "\n------------";
#endif
		clist->clear();
		pthread_setspecific(transactionBufferTSMKey, clist);
		if (rc != SQLITE_OK) {
			onTransaction = false;
			buff << "SQL error: " << zErrMsg;
			sqlite3_free(zErrMsg);
			sqlite3_exec(dbp, "ROLLBACK TRANSACTION", NULL, 0,
				NULL);
			throw Exception(buff.str(),
				TracePoint("SQLiteDBEngine"));
		}
	}
	onTransaction = false;
}
void SQLiteDBEngine::rollbackTransaction() throw (Exception)
{
#ifdef SQLDEBUG
	cout << "\nDB q r";
#endif
	if (onTransaction == false)
		return;

	void* raw = pthread_getspecific(transactionBufferTSMKey);
	if (raw != NULL) {
		stringList *clist = (stringList*) raw;
		clist->clear();
		pthread_setspecific(transactionBufferTSMKey, clist);
	}
	onTransaction = false;
	sqlite3_exec(dbp, "ROLLBACK TRANSACTION", NULL, 0, NULL);
}
void SQLiteDBEngine::saveXParam(string pname, string pkey, string parentName,
	string parentKey, stringList fields, stringList values)
		throw (Exception)
{
	if (fields.size() != values.size())
		throw Exception("size of 'fields' and 'values' is not equal.",
			TracePoint("SQLiteDBEngine"));

	stringstream buff, buffv;
	buff << "INSERT INTO " << pname << "(" << pname << "_key,";
	buffv << ") VALUES (";
	if (pkey == "")
		buffv << "NULL,";
	else
		buffv << "\"" << pkey << "\",";

	if (!parentName.empty()) {
		buff << parentName << "_key,";
		buffv << "\"" << parentKey << "\",";
	}
	for (unsigned int i = 0; i < fields.size(); i++) {
		buff << (i == 0 ? "" : ",") << fields[i];
		buffv << (i == 0 ? "\"" : ",\"") << values[i] << "\"";
	}
	buff << buffv.str() << ");";
	this->execute(buff.str());
}

void SQLiteDBEngine::saveXParam(string pname, string pkey, stringList fields,
	stringList values)
{
	saveXParam(pname, pkey, "", "", fields, values);
}

void SQLiteDBEngine::updateXParam(string pname, string pkey, string parentName,
	string parentKey, stringList fields, stringList values)
		throw (Exception)
{
	if (fields.size() != values.size())
		throw Exception("size of 'fields' and 'values' is not equal.",
			TracePoint("SQLiteDBEngine"));

	stringstream buff;
	buff << "UPDATE " << pname << " SET ";
	for (unsigned int i = 0; i < fields.size(); i++)
		buff << (i == 0 ? "" : ",") << fields[i] << "=\"" << values[i]
			<< "\"";
	buff << " WHERE " << pname << "_key=\"" << pkey << "\"";
	if (!parentName.empty())
		buff << " AND " << parentName << "_key=\"" << parentKey << "\"";

	buff << ";";
	this->execute(buff.str());
}
void SQLiteDBEngine::updateXParam(string pname, string pkey, stringList fields,
	stringList values)
{
	updateXParam(pname, pkey, "", "", fields, values);
}

void SQLiteDBEngine::removeXParam(string pname, string pkey, string parentName,
	string parentKey)
{
	stringstream buff;
	buff << "DELETE FROM " << pname << " WHERE " << pname << "_key=\""
		<< pkey << "\"";
	if (!parentName.empty())
		buff << " AND " << parentName << "_key=\"" << parentKey
			<< "\";";

	this->execute(buff.str());
}
void SQLiteDBEngine::removeXParam(string pname, string pkey)
{
	this->removeXParam(pname, pkey, "", "");
}

void SQLiteDBEngine::removeXParamByParent(string pname, string parentName,
	string parentKey)
{
	stringstream buff;
	buff << "DELETE FROM " << pname << " WHERE " << parentName << "_key=\""
		<< parentKey << "\";";
	this->execute(buff.str());
}

void SQLiteDBEngine::createXParamStructure(string pname, string parentName,
	stringList fields, vector<DBFieldTypes> fieldTypes)
		throw (Exception)
{
	if (fields.size() != fieldTypes.size())
		throw Exception(
			"size of 'fields' and 'datatypes' list is not equal.",
			TracePoint("SQLiteDBEngine"));

	stringstream buff;
	buff << "CREATE TABLE IF NOT EXISTS " << pname << " (" << pname
		<< "_key TEXT, ";
	if (!parentName.empty())
		buff << parentName << "_key TEXT NOT NULL,";

	for (unsigned int i = 0; i < fields.size(); i++)
		buff << (i == 0 ? "" : ",") << fields[i] << " "
			<< DBTypetoString(fieldTypes[i]);
	buff << ", CONSTRAINT " << pname << "_pkey PRIMARY KEY (" << pname
		<< "_key"
		<< (parentName.empty() ? "" : ", " + parentName + "_key")
		<< ") );";
	this->execute(buff.str());
}
void SQLiteDBEngine::createXParamStructure(string pname, stringList fields,
	vector<DBFieldTypes> fieldTypes)
{
	this->createXParamStructure(pname, "", fields, fieldTypes);
}

void SQLiteDBEngine::destroyXParamStructure(string pname)
{
	stringstream buff;
	buff << "DROP TABLE IF EXISTS " << pname << ";";
	this->execute(buff.str());
}

int SQLiteDBEngine::loadXParamRow(string pname, string pkey, string parentName,
	string parentKey, stringList& fields, stringList& values)
		throw (Exception)
{
	string sqls = "SELECT * FROM " + pname + " WHERE " + pname + "_key=\""
		+ pkey + "\"";
	if (!parentName.empty())
		sqls += " AND " + parentName + "_key=\"" + parentKey + "\"\0";

	int res;
	sqlite3_stmt* stmt;
	fields.clear();
	values.clear();
	res = sqlite3_prepare_v2(dbp, sqls.c_str(), -1, &stmt, NULL);
	if (res == SQLITE_OK) {
		int cols = sqlite3_column_count(stmt);
		res = sqlite3_step(stmt);
		if (res != SQLITE_ROW) {
			if (res != SQLITE_DONE)
				throw Exception("Error in loading data.",
					TracePoint("SQLiteDBEngine"));
			sqlite3_finalize(stmt);
			return 0;
		}
		for (int i = 0; i < cols; i++) {
			if ((char*) sqlite3_column_name(stmt, i)
				== (parentName + "_key")
				|| (char*) sqlite3_column_name(stmt, i)
					== (pname + "_key"))
				continue;
			fields.push_back((char*) sqlite3_column_name(stmt, i));
			values.push_back((char*) sqlite3_column_text(stmt, i));
		}
		sqlite3_finalize(stmt);
		return 1;
	} else {
		throw Exception("Error in loading statement.",
			TracePoint("SQLiteDBEngine"));
	}
}
int SQLiteDBEngine::loadXParamRow(string pname, string pkey, stringList& fields,
	stringList& values)
{
	return this->loadXParamRow(pname, pkey, "", "", fields, values);
}

int SQLiteDBEngine::loadXParamValueListByParent(string pname, string parentName,
	string parentKey, string fieldName, stringList& values)
		throw (Exception)
{
	string sqls = "SELECT " + fieldName + " FROM " + pname + " WHERE "
		+ parentName + "_key=\"" + parentKey + "\"";
	int res, count;
	sqlite3_stmt* stmt;
	count = 0;
	values.clear();
	res = sqlite3_prepare_v2(dbp, sqls.c_str(), -1, &stmt, NULL);
	if (res == SQLITE_OK) {
		while (true) {
			res = sqlite3_step(stmt);
			if (res != SQLITE_ROW) {
				if (res == SQLITE_DONE)
					break;
				else
					throw Exception(
						"Error in loading data.",
						TracePoint("SQLiteDBEngine"));
			}
			values.push_back((char*) sqlite3_column_text(stmt, 0));
			count++;
		}
		sqlite3_finalize(stmt);
		return count;
	} else {
		throw Exception("Error in loading statement.",
			TracePoint("SQLiteDBEngine"));
	}
}

void SQLiteDBEngine::getData(string selectstmt,
	vector<vector<string> > &results, vector<string> &columns)
		throw (Exception)
{
	sqlite3_stmt* stmt;
	int res = sqlite3_prepare_v2(dbp, selectstmt.c_str(), -1, &stmt, NULL);
#ifdef SQLDEBUG
	cout << "\n SELECT :: " << selectstmt.c_str() << " R:" << res << std::endl;
#endif
	if (res == SQLITE_OK) {

		int cols = sqlite3_column_count(stmt);
		while (true) {
			res = sqlite3_step(stmt);
			if (res != SQLITE_ROW) {
				if (res == SQLITE_DONE)
					break;
				else {
					sqlite3_finalize(stmt);
					throw Exception(
						"Error in loading data.",
						TracePoint("SQLiteDBEngine"));
				}
			}
			vector<string> vrow;
			for (int i = 0; i < cols; i++)
				vrow.push_back(
					(char*) sqlite3_column_text(stmt, i));
			results.push_back(vrow);
		}
		sqlite3_finalize(stmt);
	} else {
		throw Exception("Error in loading statement.",
			TracePoint("SQLiteDBEngine"));
	}
}

string SQLiteDBEngine::DBTypetoString(DBFieldTypes t)
{
	switch (t) {
		case DBBOOLEAN:
			return "INTEGER";
			break;
		case DBFLOAT:
			return "REAL";
			break;
		case DBTEXT:
			return "TEXT";
			break;
		case DBDATETIME:
			return "TEXT";
			break;
		case DBINTEGER:
			return "INTEGER";
			break;
		default:
			return "";
	}
}

bool SQLiteDBEngine::backup(string dest)
{
	  int rc;                     /* Function return code */
	  sqlite3 *pFile;             /* Database connection opened on zFilename */
	  sqlite3_backup *pBackup;    /* Backup handle used to copy data */

	  rc = sqlite3_open(dest.c_str(), &pFile);
	  if( rc==SQLITE_OK ){
	    pBackup = sqlite3_backup_init(pFile, "main", this->dbp, "main");
	    if( pBackup ){
	      do {
	        rc = sqlite3_backup_step(pBackup, 5);
	        if( rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED ){
	          sqlite3_sleep(10);
	        }
	      } while( rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED );
	      (void)sqlite3_backup_finish(pBackup);
	    }
	    rc = sqlite3_errcode(pFile);
	  }
	  (void)sqlite3_close(pFile);
	  return rc==SQLITE_OK;
}

void SQLiteDBEngine::cleanup()
{
	this->execute("VACUUM");
}

}
//end namespace pparam
