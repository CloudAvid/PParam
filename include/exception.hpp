/**
 * @file exception.hpp
 *
 * Copyright 2010 PDNSoft Co. (www.pdnsoft.com)
 * @author hamid jafaian (hamid.jafarian@pdnsoft.com)
 *
 * exception is part of PParam.
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
#ifndef _PDN_EXCEPTION_HPP_
#define _PDN_EXCEPTION_HPP_

#include "logs.hpp"

#include <deque>
#include <string>
using std::string;

namespace pparam
{

/**
 * Stores a trace point info of function sequence call.
 */
struct TraceInfo
{
	string SubSystemName;
	string FunctionName;
	string FunctionParameters;

	TraceInfo(string sName, string fName,
			string fParams = "") : SubSystemName(sName), 
						FunctionName(fName), 
						FunctionParameters(fParams)
	{
	}
};
/**
 * These are for easy Trace point information creation.
 */
#define TracePoint(SystemName) \
	TraceInfo(SystemName, __FUNCTION__, static_cast< string >(""))

#define TracePoint_WithParams(SystemName, Params) \
	TraceInfo(SystemName, __FUNCTION__, Params)


class Exception
{
public:
	typedef unsigned int Int;
	/**
	 * \enum exStatus
	 * Exception Status.
	 *
	 * One operation may throw exception when failed or may be all 
	 * is ok but some errors has been occured.
	 */
	enum exStatus {
		NOK,	/**< operation is ok but some errors occured */
		FAILED,	/**< operation failed */
	};

	Exception(const string &des, const TraceInfo &tInfo);
	Exception(exStatus _status,const string &des, const TraceInfo &tInfo);
	Exception(Int _mid, Int _aid, Int _errno, 
			Exception::exStatus _status,
			const string &des, const TraceInfo &tInfo);

	virtual string what() const;
	/**
	 * Generate an xml document from exception data.
	 * @param withCallTrace Does include Function Call sequence in 
	 * 	the document?
	 */
	virtual string xml(bool withCallTrace = false) const;
	/**
	 * Send Log messages to the log system.
	 * @param level overall level of log messages
	 * @param callTraceLevel level of Function Call sequence message.
	 */
	virtual void log(LogSystem &logSys, 
			LogLevel::Levels level = LogLevel::ERROR,
			LogLevel::Levels callTraceLevel = LogLevel::DEBUG);
	/**
	 * Push another call point at the front of CallSequence.
	 *
	 * This function can be used by upper functions in the call sequence 
	 * to record trace points of call sequence.
	 * This function may be called in catch blocks.
	 */
	void addTracePoint(const TraceInfo &tInfo);
	void set_status(exStatus _status)
	{
		status = _status;
	}
	exStatus get_status() const
	{
		return status;
	}
	void set_errno(Int _errno)
	{
		errno = _errno;
	}
	Int get_errno() const
	{
		return errno;
	}
	void set_description(const char *description)
	{
		excDesc.assign(description);
	}
	void set_description(const string &description)
	{
		excDesc = description;
	}
	void set_nokDesc(const string &_nokDesc)
	{
		nokDesc = _nokDesc;
	}
	string get_nokDesc() const
	{
		return nokDesc;
	}
	void set_mid(Int _mid)
	{
		mid = _mid;
	}
	Int get_mid() const
	{
		return mid;
	}
	void set_aid(Int _aid)
	{
		aid = _aid;
	}
	Int get_aid() const
	{
		return aid;
	}
	bool is_failed() const
	{
		return status == FAILED;
	}
	bool is_nok() const
	{
		return status == NOK;
	}
	void failed()
	{
		status = FAILED;
	}
	void nok() 
	{
		status = NOK;
	}
private:
	/**
	 * Stores funcations call sequence, from exception generator to
	 * the outer caller function.
	 */
	std::deque<TraceInfo> callSequence;
	/**
	 * Description for throwed exception.
	 */
	string excDesc;
	/**
	 * Exception status.
	 */
	exStatus status;
	/**
	 * Error number of this exception.
	 */
	Int err;
	/**
	 * ID of submodule that throws this exception.
	 */
	Int mid;
	/**
	 * ID of action in submodule that throws this exception.
	 */
	Int aid;
	/**
	 * Description about nok status.
	 *
	 * When exception status is nok, you may want to send more information
	 * to exception catcher, use this.
	 */
	string nokDesc;
};

} //namespace pparam

#endif
