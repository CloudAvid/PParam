#include "exception.hpp"

#include <string>
using std::string;

#include <sstream>
using std::stringstream;

namespace pparam
{

Exception::Exception(const string &des, const TraceInfo &tInfo) : excDesc(des)
{
	callSequence.push_front(tInfo);
	status = FAILED;
	mid = aid = err = -1;
}

Exception::Exception(exStatus _status,const string &des, const TraceInfo &tInfo)
		: excDesc(des), status(_status)
{
	callSequence.push_front(tInfo);
	mid = aid = err = -1;
}

Exception::Exception(Exception::Int _mid, Exception::Int _aid, 
			Exception::Int _err, Exception::exStatus _status,
			const string &des, const TraceInfo &tInfo) : 
			excDesc(des),
			status(_status),
			err(_err),
			mid(_mid), aid(_aid)
{
	callSequence.push_front(tInfo);
}

string Exception::what() const
{
	return excDesc;
}

string Exception::xml(bool withCallTrace) const
{
	string str = "<what>" + what() + "</what>";
	std::ostringstream oss;

	oss << get_errno();
	str += "<errno>"+ oss.str() + "</errno>";
	oss << get_mid();
	str += "<mid>" + oss.str() + "</mid>";
	oss << get_aid();
	str += "<aid>" + oss.str() + "</aid>";
	if (withCallTrace) {
		str += "<call_trace>";
		std::deque< TraceInfo >::const_iterator iter;
		for (iter = callSequence.begin();
			iter != callSequence.end(); ++iter) {
			str += "<call>";
			str += "<system_name>" + 
					(*iter).SubSystemName + 
					"</system_name>";
			str += "<function>" + 
					(*iter).FunctionName + 
					"</function>";
			str += "<params>" + 
					(*iter).FunctionParameters + 
					"</params>";
			str += "</call>";
		}
		str += "</call_trace>";
	}
	return str;
}

void Exception::log(LogSystem &logSys, 
			LogLevel::Levels level,
			LogLevel::Levels callTraceLevel)
{
	logSys << level;
	logSys << "what:" + what();

	logSys << callTraceLevel;
	std::deque< TraceInfo >::const_iterator iter;
	for (iter = callSequence.begin();
		iter != callSequence.end(); ++iter) 
		logSys << "->" + (*iter).SubSystemName + ": " +
				(*iter).FunctionName + 
				"(" + (*iter).FunctionParameters + ")";
}

void Exception::addTracePoint(const TraceInfo &tInfo)
{
	callSequence.push_front(tInfo);
}
} // namespace pparam
