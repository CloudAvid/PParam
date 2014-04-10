#include <sstream>

#include "logs.hpp"


namespace pparam
{


int LogSystem::logFacility = LOG_LOCAL0;
int LogSystem::mask = LogLevel::DEBUG;
const string LogLevel::typeString[LogLevel::MAX] = { "emergency", "alert",
	"critical", "error", "warning", "notice", "info", "debug" };


LogSystem::LogSystem(const string &_subSystemName)
{
	set_subSystemName(_subSystemName);
}

void LogSystem::set_subSystemName(const string &_subSystemName)
{
	subSystemName = _subSystemName;
	subSystemName_c_str = subSystemName.c_str();
}

void LogSystem::setLogFacility(int _logFacility)
{
	logFacility = _logFacility;
}

void LogSystem::setLogMask(int _mask)
{
	mask = _mask;
}

LogSystem &operator << (LogSystem &ls, const LogLevel::Levels &logLevel)
{
	ls.subSystemLogLevel = logLevel;
	return ls;
}

void LogSystem::_sendLogMsg(const char *logMsg)
{
        if (mask >= subSystemLogLevel) {
                syslog(logFacility | subSystemLogLevel,
                                "%s: %s",
                                subSystemName_c_str,
                                logMsg);
        }
}

LogSystem &operator << (LogSystem &ls, const int &intMsg)
{
	std::stringstream ss;

	ss << intMsg;
	ls._sendLogMsg(ss.str().c_str());
	return ls;
}

LogSystem &operator << (LogSystem &ls, const string &logMsg)
{
	ls._sendLogMsg(logMsg.c_str());
	return ls;
}

LogSystem &operator << (LogSystem &ls, const char *logMsg)
{
	ls._sendLogMsg(logMsg); 
	return ls;
}

} // namespace pparam



