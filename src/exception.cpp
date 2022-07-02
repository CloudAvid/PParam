#include "exception.hpp"

#include <string>
using std::string;

#include <sstream>
using std::stringstream;

namespace pparam
{

Exception::Exception(const string &des, const TraceInfo &tInfo) : excDesc(des)
{
    status = FAILED;
    err = -1;
    callSequence.push_front(tInfo);
}

Exception::Exception(const exStatus _status, const string &des, const TraceInfo &tInfo) :
    status(_status), excDesc(des)
{
    callSequence.push_front(tInfo);
    err = -1;
}

Exception::Exception(const std::string module, const Exception::Int _err, const string &des,
                     const TraceInfo &tInfo) :
    module(module),
    err(_err), excDesc(des)
{
    status = FAILED;
    callSequence.push_front(tInfo);
}

string Exception::what() const { return excDesc; }

string Exception::xml(bool withCallTrace) const
{
    std::ostringstream oss;
    std::deque<TraceInfo>::const_iterator iter;

    oss << "<mid>" << get_module() << "</mid>";
    oss << "<aid></aid>";
    oss << "<errno>" << get_errno() << "</errno>";
    oss << "<what>" << what() << "</what>";
    if (withCallTrace) {
        oss << "<call_trace>";
        for (iter = callSequence.begin(); iter != callSequence.end(); ++iter) {
            oss << "<call>";
            oss << "<system_name>" << (*iter).SubSystemName << "</system_name>";
            oss << "<function>" << (*iter).FunctionName << "</function>";
            oss << "<params>" << (*iter).FunctionParameters << "</params>";
            oss << "</call>";
        }
        oss << "</call_trace>";
    }
    return oss.str();
}

void Exception::log(LogSystem &logSys, LogLevel::Levels level, LogLevel::Levels callTraceLevel)
{
    logSys << level;
    logSys << "what:" + what();

    logSys << callTraceLevel;
    std::deque<TraceInfo>::const_iterator iter;
    for (iter = callSequence.begin(); iter != callSequence.end(); ++iter)
        logSys << "->" + (*iter).SubSystemName + ": " + (*iter).FunctionName + "(" +
                      (*iter).FunctionParameters + ")";
}

void Exception::addTracePoint(const TraceInfo &tInfo) { callSequence.push_front(tInfo); }

void Exception::set_status(exStatus _status) { status = _status; }

Exception::exStatus Exception::get_status() const { return status; }

std::string Exception::get_module() const { return module; }

void Exception::set_errno(Int _err) { err = _err; }

Exception::Int Exception::get_errno() const { return err; }

void Exception::set_description(const char *description) { excDesc.assign(description); }

void Exception::set_description(const string &description) { excDesc = description; }

void Exception::set_nokDesc(const string &_nokDesc) { nokDesc = _nokDesc; }

string Exception::get_nokDesc() const { return nokDesc; }

bool Exception::is_failed() const { return status == FAILED; }

bool Exception::is_nok() const { return status == NOK; }

void Exception::failed() { status = FAILED; }

void Exception::nok() { status = NOK; }

} // namespace pparam
