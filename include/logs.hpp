/**
 * @file logs.hpp
 * LogSystem(logs), a simple wrapper to work with rsyslog.
 * Copyright 2010 PDNSoft Co. (www.pdnsoft.com)
 * @author hamid jafaian (hamid.jafarian@pdnsoft.com)
 *
 * logs is part of PParam.
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


#ifndef _PDN_LOGSYSTEM_HPP_
#define _PDN_LOGSYSTEM_HPP_

#include <syslog.h>
#include <string>
using std::string;

namespace pparam
{
/**
 * These are different loglevels for different situations.
 *
 * Each level defines specific system message type.
 */
class LogLevel 
{
public:
	enum Levels {
		EMERG = LOG_EMERG, /**< This is a critical state, \
					the system will hangup. */
		ALERT = LOG_ALERT,
		CRIT = LOG_CRIT,
		ERROR = LOG_ERR, /**< Some Error have been occured, \
					but the system can continue */
		WARNING = LOG_WARNING,
		NOTICE = LOG_NOTICE,
		INFO  = LOG_INFO, /**< Report info about system state */
		DEBUG = LOG_INFO, /**< Debug information about structures
								and data */
		MAX = 8,
	};
	static const string typeString[MAX];
};

/**
 * LogSystem Manages Log Messages.
 *
 * This class sends Log Messages to the "rsyslog" package.
 * Configs of "/etc/rsyslog.conf" must be in this form:
 * 	# Sends all messages "All.log"
 * 	local0.*	/var/log/All.log
 * 	# Sends System Specific Messages to their Specific files
 * 	:msg, startswith, "subsytem name" /var/log/SubSystem-Specific-file
 * @see LogLevel
 */
class LogSystem
{
	/**
	 * This special Overloading sets the LogLevel of the 
	 * future comming log messages.
	 *
	 * By thid function, subsystems will set the log level (e.g.
	 * for DEBUG level)	 in this form:
	 *	logSystem << LogLevel::DEBUG;
	 */
	friend LogSystem &operator << (LogSystem &, 
					   const LogLevel::Levels &logLevel);
	/**
	 * Sends logs messages to the "rsyslog".
	 *
	 * by this Overloaded functions, subsystems will send their
	 * log messages in this form:
	 * 	logSystem << "the log messages";
	 */	
	friend LogSystem &operator << (LogSystem &, const string &logMsg);
	friend LogSystem &operator << (LogSystem &, const char *logMsg);
	friend LogSystem &operator << (LogSystem &, const int &intMsg);
public:
	/**
	 * He just Sets the SubSystemName.
	 */
	LogSystem(const string &_subSystemName);
	/**
	 * Resets the overall log facility.
	 * @param logFacility defines the sender of log messages (LOG_LOCAL0-7)
	 */
	static void setLogFacility(int _logFacility);
	/**
	 * Resets The overall logmask.
	 */
	static void setLogMask(int _mask);
	string get_subSystemName()
	{
		return subSystemName;
	}
	void set_subSystemName(const string &_subSystemName);
	LogLevel::Levels get_subSystemLogLevel()
	{
		return subSystemLogLevel;
	}
private:
	/**
	 * Private function that sends log messages to log system.
	 */
	void _sendLogMsg(const char *logMsg);
	/**
	 * The Default facility to work with "rsyslog".
	 *
	 * facility defines the type of the program that sends logs to rsyslog.
	 * He may be LOG_LOCAL0 through LOG_LOCAL7 
	 *  (they reserved for local uses)
	 */
	static int logFacility;
	/**
	 * Defines "Whome messages should be logged?".
	 */
	static int mask;
	/**
	 * Name of the SubSystem sends log messages.
	 *
	 * This name will be prepend to the messages of caller.
	 */
	string subSystemName;
	/**
	 * C-Style string pointer to the SubSystemName.
	 *
	 * This is for performance issues.
	 */
	const char *subSystemName_c_str;
	/**
	 * Level(type) of log messages from "SubSystemName"
	 */
	LogLevel::Levels subSystemLogLevel;
};

} // namespace pparam

#endif // _PDN_LOGSYSTEM_HPP_
