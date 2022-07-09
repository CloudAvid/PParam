/**
 * @file exception.hpp
 *
 * Copyright 2010-2022 Cloud Avid Co. (www.cloudavid.com)
 * @author hamid jafaian (hamid.jafarian@cloudavid.com)
 * @author Hamed Haji Hussuaini (hajihussaini@cloudavid.com)
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
#pragma once

#include "logs.hpp"

#include <deque>
#include <string>
using std::string;

namespace pparam
{

/**
 * Stores a trace point info of function sequence call.
 */
struct TraceInfo {
    string SubSystemName;
    string FunctionName;
    string FunctionParameters;

    TraceInfo(string sName, string fName, string fParams = "") :
        SubSystemName(sName), FunctionName(fName), FunctionParameters(fParams)
    {
    }
};
/**
 * These are for easy Trace point information creation.
 */
#define TracePoint(SystemName) TraceInfo(SystemName, __FUNCTION__, static_cast<string>(""))

#define TracePoint_WithParams(SystemName, Params) TraceInfo(SystemName, __FUNCTION__, Params)

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
        NOK,    /**< operation is ok but some errors occured */
        FAILED, /**< operation failed */
    };

    Exception(const string &des, const TraceInfo &tInfo);
    Exception(const exStatus _status, const string &des, const TraceInfo &tInfo);
    Exception(const std::string module, const Int _err, const string &des, const TraceInfo &tInfo);

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
    virtual void log(LogSystem &logSys, LogLevel::Levels level = LogLevel::ERROR,
                     LogLevel::Levels callTraceLevel = LogLevel::DEBUG);
    /**
     * Push another call point at the front of CallSequence.
     *
     * This function can be used by upper functions in the call sequence
     * to record trace points of call sequence.
     * This function may be called in catch blocks.
     */
    void addTracePoint(const TraceInfo &tInfo);
    void set_status(exStatus _status);
    exStatus get_status() const;
    ;
    std::string get_module() const;
    void set_errno(Int _err);
    Int get_errno() const;
    void set_description(const char *description);
    void set_description(const string &description);
    void set_nokDesc(const string &_nokDesc);
    string get_nokDesc() const;
    bool is_failed() const;
    bool is_nok() const;
    void failed();
    void nok();

private:
    /**
     * Exception status.
     */
    exStatus status;
    /**
     * Name of module that throws this exception.
     */
    std::string module;
    /**
     * Error number of this exception.
     */
    Int err;
    /**
     * Description for throwed exception.
     */
    string excDesc;
    /**
     * Description about nok status.
     *
     * When exception status is nok, you may want to send more information
     * to exception catcher, use this.
     */
    string nokDesc;
    /**
     * Stores funcations call sequence, from exception generator to
     * the outer caller function.
     */
    std::deque<TraceInfo> callSequence;
};

} // namespace pparam
