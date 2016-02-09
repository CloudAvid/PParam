#include "sparam.hpp"
#include "math.h"
#include <iostream>
#define checkByteRange(n) (n<0?false:(n>255?false:true))
#define checkWordRange(n) (n<0?false:(n>65535?false:true))

using std::hex;

namespace pparam
{

const string DBEngineTypes::typeString[DBEngineTypes::MAX]={"sqlite"};

/* Implementation of "UUIDParam" Class
 */
UUIDParam::UUIDParam(const UUIDParam &uuidp) : XSingleParam(uuidp.get_pname())
{
	*this = uuidp;
}

UUIDParam::UUIDParam(UUIDParam &&_uuidp) : XSingleParam(std::move(_uuidp))
{
	*this = _uuidp;
}

UUIDParam &UUIDParam::operator = (const UUIDParam &uuidp)
{
	uuid_copy(uuid, uuidp.uuid);
	return *this;
}

XParam &UUIDParam::operator = (const string &str) throw (Exception)
{
	int ret = uuid_parse(str.c_str(), uuid);
	if (ret == -1)
		throw Exception("Bad uuid !", TracePoint("sparam"));
	return *this;
}

XParam &UUIDParam::operator = (const XParam &xp) throw (Exception)
{
	const UUIDParam *uuidp = dynamic_cast<const UUIDParam *>(&xp);
	if (uuidp == NULL)
		throw Exception("Bad uuid param in assginment",
							TracePoint("sparam"));
	if (get_pname() != uuidp->get_pname())
		throw Exception("Different uuid parameters in"
					" assginment !", TracePoint("sparam"));
	uuid_copy(uuid, uuidp->uuid);
	return *this;
}

string UUIDParam::value() const
{
	char uuid_str[36];
	uuid_unparse(uuid, uuid_str);
	return uuid_str;
}

/** Implementation of "CryptoParam" class */

string CryptoParam::md5(const string &text)
{
	GString*	gString;
	MD5_CTX		md5Context;
	string		cryptedText;
	unsigned char	mdText[32];
	unsigned char	index;

	MD5_Init(&md5Context);
	MD5_Update( &md5Context, text.c_str(), text.length());
	MD5_Final( mdText, &md5Context);
	gString = (GString*) g_malloc0(sizeof(GString));
	for (index = 0; index < 16; index++)
		g_string_append_printf(gString, "%02x", mdText[index]);
	cryptedText.assign(gString->str);
	g_free(gString);

	return cryptedText;
}

/* Implementation of Bool/BoolParam classes.
 */

const string Bool::typeString[MAX] = {
		"yes",
		"no",
		"on",
		"off",
		"enable",
		"disable",
		"enabled",
		"disabled",
		"up",
		"down",
		"set",
		"unset"
};

BoolParam::BoolParam(const string &pname,const unsigned short _default) :
	XEnumParam<Bool>(pname, _default)
{
}

BoolParam::BoolParam(BoolParam &&_bp) : _XEnumParam(std::move(_bp))
{ }

bool BoolParam::is_enable() const
{
	return ! is_disable();
}

bool BoolParam::is_disable() const
{
	return get_value() % 2;
}

void BoolParam::yes()
{
	enable(Bool::YES);
}

void BoolParam::no()
{
	disable(Bool::NO);
}

void BoolParam::on()
{
	enable(Bool::ON);
}

void BoolParam::off()
{
	disable(Bool::OFF);
}

void BoolParam::enable(int val)
{
	set_value((val + (val % 2)) % Bool::MAX);
}

void BoolParam::disable(int val)
{
	set_value((val + !(val % 2)) % Bool::MAX);
}

void BoolParam::enabled()
{
	enable(Bool::ENABLED);
}

void BoolParam::disabled()
{
	disable(Bool::DISABLED);
}

void BoolParam::up()
{
	enable(Bool::UP);
}

void BoolParam::down()
{
	disable(Bool::DOWN);
}

void BoolParam::set()
{
	enable(Bool::SET);
}

void BoolParam::unset()
{
	disable(Bool::UNSET);
}

BoolParam &BoolParam::operator=(const bool &value)
{
	if (value)
		enable();
	else disable();
	return *this;
}

BoolParam &BoolParam::operator=(const XInt &value) throw (Exception)
{
	set_value(value);
	return *this;
}

bool BoolParam::operator==(const bool &value)
{
	if (value)
		return is_enable();
	else return is_disable();
}

bool BoolParam::operator==(const XInt &value)
{
	return val == value;
}

/* Implementation of "DateParam" class
 */
DateParam &DateParam::operator = (const DateParam &dateParam)
					throw (Exception)
{
	year = dateParam.year;
	month = dateParam.month;
	day = dateParam.day;

	return *this;
}

XParam &DateParam::operator = (const string &date) throw (Exception)
{
	int	result;
	int	_day;
	int	_month;
	int	_year;

	result = sscanf( date.c_str(), "%d/%d/%d", &_year, &_month, &_day);
	if ((result != 3) || (result == EOF))
		throw Exception( "Bad-formatted date string",
					TracePoint("sparam"));
	year = _year;
	month = _month;
	day = _day;

	return *this;
}

XParam &DateParam::operator = (const XParam &parameter) throw (Exception)
{
	const DateParam	*date = dynamic_cast<const DateParam*>(&parameter);

	if (date == NULL)
		throw Exception( "Bad DateParam in assignment",
					TracePoint("sparam"));
	year = date->year;
	month = date->month;
	day = date->day;

	return *this;
}

bool DateParam::operator < (const DateParam &date)
{
	if (year < date.year)
		return true;
	if ((year == date.year) && (month < date.month))
		return true;
	if ((year == date.year) && (month == date.month) && (day < date.day))
		return true;

	return false;
}

bool DateParam::operator == (const DateParam &date)
{
	if ((year == date.year) && (month == date.month)
			&& (day == date.day))
		return true;

	return false;
}

bool DateParam::operator != (const DateParam &date)
{
	if ((year != date.year) || (month != date.month)
			|| (day != date.day))
		return true;

	return false;
}

bool DateParam::operator > (const DateParam &date)
{
	if (year > date.year)
		return true;
	if ((year == date.year) && (month > date.month))
		return true;
	if ((year == date.year) && (month == date.month) && (day > date.day))
		return true;

	return false;
}

bool DateParam::operator >= (const DateParam &date)
{
	if (*this < date)
		return false;

	return true;
}

bool DateParam::operator <= (const DateParam &date)
{
	if (*this > date)
		return false;

	return true;
}

DateParam DateParam::operator + (const DateParam &date)
{
	DateParam	result(get_pname());
	unsigned short	days;
	unsigned char	monthDays;

	result.year = year + date.year;
	result.month = month;
	result.day = days = day + date.day;
	monthDays = result.daysOfMonth();
	if (days > monthDays) {
		result.month += (days / monthDays);
		result.day = days % monthDays;
		result.day++;
	}
	result.month += date.month;
	if (result.month > 12) {
		result.year += (result.month / 12);
		result.month = result.month % 12;
	}

	return result;
}

long DateParam::operator - (DateParam &date)
{
	return daysOfDate() - date.daysOfDate();
}

unsigned short DateParam::get_weekday()
{
	DateParam	date(get_pname());

	date = "1924/12/19";

	return (*this - date) % 7;
}

void DateParam::get_date(unsigned short &_year,
			unsigned short &_month,
			unsigned short &_day) const
{
	_year = year;
	_month = month;
	_day = day;
}

bool DateParam::isValid()
{
	if (year < 1900)
		return false;

	if ((!month) || (month > 12))
		return false;

	if ((!day) || (day > daysOfMonth()))
		return false;

	return true;
}

void DateParam::set_date(unsigned short _year,
			unsigned short _month,
			unsigned short _day)
{
	year = _year;
	month = _month;
	day = _day;
}

string DateParam::value() const
{
	char	date[11];
	string	dateString;

	sprintf( date, "%04d/%02d/%02d", year, month, day);
	dateString.assign(date);

	return dateString;
}

string DateParam::formattedValue(const string format) const
{
	char	*date;
	string	dateString;

	try {
		date = new char[format.length()];
	} catch (std::bad_alloc &exception) {
	}
	sprintf( date, format.c_str(), year, month, day);
	dateString.assign(date);
	delete date;

	return dateString;
}

void DateParam::now()
{
	char	buffer[11];
	time_t	rawTime;

	std::time(&rawTime);
	strftime(buffer, sizeof(buffer), "%Y/%m/%d",  std::localtime(&rawTime));
	*this = string(buffer);
}

unsigned char DateParam::daysOfMonth() const
{
	switch (month) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			return 31;
		case 4:
		case 6:
		case 9:
		case 11:
			return 30;
	}
	if (isLeapYear())
		return 29;

	return 28;
}

bool DateParam::isLeapYear() const
{
	if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
		return true;

	return false;
}

unsigned short DateParam::daysOfYear() const
{
	if (isLeapYear())
		return 366;

	return 365;
}


unsigned long DateParam::daysOfDate()
{
	unsigned short	_month;
	unsigned char	index;
	unsigned long	days;
	unsigned short	_year;

	_year = year;
	if (!_year)
		_year = 1;
	days = (_year - 1) * 365;
	days += ((year - 1) / 4);
	year--;
	if (isLeapYear())
		days++;
	year++;
	_month = month;
	for ( index = 1; index < _month; index++) {
		month = index;
		days += daysOfMonth();
	}
	month  = _month;
	days += day;

	return days;
}

/* Implementation of "TimeParam" class */

TimeParam::TimeParam(const string &name) :
	XSingleParam(name)
{
	hour = minute = second = 0;
}

TimeParam::TimeParam(const TimeParam &time) :
	XSingleParam(time.get_pname())
{
	hour = time.hour;
	minute = time.minute;
	second = time.second;
}

TimeParam::TimeParam(TimeParam &&_tp) :
	XSingleParam(std::move(_tp)),
	hour(_tp.hour),
	minute(_tp.minute),
	second(_tp.second)
{
}
 
TimeParam &TimeParam::operator = (const TimeParam &timeParam)
					throw (Exception)
{
	hour = timeParam.hour;
	minute = timeParam.minute;
	second = timeParam.second;

	return *this;
}

XParam &TimeParam::operator = (const string &time) throw (Exception)
{
	int		result;
	unsigned int	_hour;
	unsigned int	_minute;
	unsigned int	_second;

	result = sscanf( time.c_str(), "%d:%d:%d", &_hour, &_minute, &_second);
	if ((result > 3) || (result == EOF))
		throw Exception( "Bad-formatted time string",
					TracePoint("sparam"));
	hour = _hour;
	minute = _minute;
	second = _second;

	return *this;
}

XParam &TimeParam::operator = (const XParam &parameter) throw (Exception)
{
	const TimeParam	*time = dynamic_cast<const TimeParam*>(&parameter);

	if (time == NULL)
		throw Exception( "Bad TimeParam in assignment",
					TracePoint("sparam"));
	hour = time->hour;
	minute = time->minute;
	second = time->second;

	return *this;
}

bool TimeParam::operator < (const TimeParam &time)
{
	if (hour < time.hour)
		return true;
	if ((hour == time.hour) && (minute < time.minute))
		return true;
	if ((hour == time.hour)
			&& (minute == time.minute)
			&& (second < time.second))
		return true;

	return false;
}

bool TimeParam::operator == (const TimeParam &time)
{
	if ((hour == time.hour) && (minute == time.minute)
			&& (second == time.second))
		return true;

	return false;
}

bool TimeParam::operator != (const TimeParam &time)
{
	if ((hour != time.hour) || (minute != time.minute)
			|| (second != time.second))
		return true;

	return false;
}

bool TimeParam::operator > (const TimeParam &time)
{
	if (hour > time.hour)
		return true;
	if ((hour == time.hour) && (minute > time.minute))
		return true;
	if ((hour == time.hour)
			&& (minute == time.minute)
			&& (second > time.second))
		return true;

	return false;
}

bool TimeParam::operator >= (const TimeParam &time)
{
	if (*this < time)
		return false;

	return true;
}

bool TimeParam::operator <= (const TimeParam &time)
{
	if (*this > time)
		return false;

	return true;
}

TimeParam TimeParam::operator + (const TimeParam &time)
{
	TimeParam	result(get_pname());

	result.second = second + time.second;
	if (result.second > 59) {
		result.minute = result.second / 60;
		result.second = result.second % 60;
	}
	result.minute += (minute + time.minute);
	if (result.minute > 59) {
		result.hour = result.minute / 60;
		result.minute = result.minute % 60;
	}
	result.hour += (hour + time.hour);
	result.hour = result.hour % 24;

	return result;
}

long TimeParam::operator - (const TimeParam &time)
{
	int	midnight = (*this > time) ? 1 : 0;

	/*
	 * If this is less than or equal to time, we just calculate the
	 * difference of them, otherwise, we calculate the difference of this
	 * and midnight and difference of time and midnight and then sum these
	 * two differences.
	 */
	return midnight * 86400 - secondsOfTime() + time.secondsOfTime();
}

TimeParam TimeParam::add(const TimeParam &time,unsigned char &day)
{
	TimeParam	result(get_pname());

	result.second = second + time.second;
	if (result.second > 59) {
		result.minute = result.second / 60;
		result.second = result.second % 60;
	}
	result.minute += (minute + time.minute);
	if (result.minute > 59) {
		result.hour = result.minute / 60;
		result.minute = result.minute % 60;
	}
	result.hour += (hour + time.hour);
	day = result.hour / 24;
	result.hour = result.hour % 24;

	return result;

}

unsigned short TimeParam::get_hour() const
{
	return hour;
}

void TimeParam::set_hour(unsigned short _hour)
{
	hour = _hour;
}

unsigned short TimeParam::get_minute() const
{
	return minute;
}

void TimeParam::set_minute(unsigned short _minute)
{
	minute = _minute;
}

unsigned int TimeParam::get_second() const
{
	return second;
}

void TimeParam::set_second(unsigned int _second)
{
	second = _second;
}

void TimeParam::get_time(unsigned short &_hour,
			unsigned short &_minute,
			unsigned int &_second) const
{
	_hour = hour;
	_minute = minute;
	_second = second;
}

void TimeParam::set_time(unsigned short _hour,
			unsigned short _minute,
			unsigned int _second)
{
	hour = _hour;
	minute = _minute;
	second = _second;
}

bool TimeParam::isValid()
{
	if ((hour > 23) || (minute > 59) || (second > 59))
		return false;

	return true;
}

string TimeParam::value() const
{
	char	time[9];
	string	timeString;

	sprintf( time, "%02d:%02d:%02d", hour, minute, second);
	timeString.assign(time);

	return timeString;
}

void TimeParam::reset()
{
	hour = minute = second = 0;
}

string TimeParam::formattedValue(const string format) const
{
	char	*time;
	string	timeString;

	try {
		time = new char[format.length()];
	} catch (std::bad_alloc &exception) {
		return "";
	}
	sprintf( time, format.c_str(), hour, minute, second);
	timeString.assign(time);
	delete time;

	return timeString;
}

void TimeParam::now()
{
	char	buffer[9];
	time_t	rawTime;

	std::time(&rawTime);
	strftime(buffer, sizeof(buffer), "%H:%M:%S", std::localtime(&rawTime));
	*this = string(buffer);
}

unsigned long TimeParam::secondsOfTime() const
{
	return ((hour * 3600) + (minute * 60) + second);
}

/** Implementation of "DateTime" class */

XParam &DateTime::operator = (const string &strdate)
{
	size_t spos = strdate.find(" ");
	if (spos == string::npos)
		date = strdate;
	else {
		string datepart = strdate.substr(0, spos);
		string timepart = strdate.substr(spos + 1);
		date = datepart;
		time = timepart;
	}
	return *this;
}

XParam &DateTime::operator = (const XParam &idate)
{
	DateTime *dt =
		const_cast<DateTime*>
		(dynamic_cast<const DateTime*>(&idate));
	if (dt == NULL)
		throw Exception("NULL in assignment",
			TracePoint("sparam"));
	if (dt->get_pname() != this->get_pname())
		throw Exception("different pnames in assignment",
			TracePoint("sparam"));
	this->set_date(dt->get_date());
	this->set_time(dt->get_time());
	return *this;
}

XParam &DateTime::operator = (DateTime &idate)
{
	if (idate.get_pname() != this->get_pname())
		throw Exception("different pnames in assignment",
			TracePoint("sparam"));
	this->set_date(idate.get_date());
	this->set_time(idate.get_time());
	return *this;
}

bool DateTime::operator < (DateTime &dateTime)
{
	if ((date < dateTime.get_date()) || ((date == dateTime.get_date()) &&
				(time < dateTime.get_time())))
		return true;

	return false;
}

bool DateTime::operator > (DateTime &dateTime)
{
	if ((date > dateTime.get_date()) || ((date == dateTime.get_date()) &&
				(time > dateTime.get_time())))
		return true;

	return false;
}

bool DateTime::isValid()
{
	return date.isValid() && time.isValid();
}

string DateTime::value() const
{
	std::stringstream ss;
	ss << date.value() << " " << time.value();
	return ss.str();
}

string DateTime::formattedValue(const string dateFormat,
				const string timeFormat,
				const char separator) const
{
	std::stringstream ss;
	ss << date.formattedValue(dateFormat) << separator
		<< time.formattedValue(timeFormat);
	return ss.str();
}

void DateTime::now()
{
	date.now();
	time.now();
}

/** implementaion of "IPType" class */

IPParam *IPType::newT() throw (Exception)
{
	if (!empty())
		return IPParam::getIP(get_pname(), value());
	else if (version == IPv4)
		return new IPv4Param(get_pname());
	else if (version == IPv6)
		return new IPv6Param(get_pname());

	return NULL;
}

XParam &IPType::operator = (const XmlNode *node) throw (Exception)
{
	const xmlpp::TextNode*		nodeText;
	XmlNode::NodeList		nodeList;
	XmlNode::NodeList::iterator	iterator;

	set_pname(node->get_name());
	nodeList = node->get_children();
	for (iterator = nodeList.begin(); iterator != nodeList.end();
			iterator++) {
		nodeText = dynamic_cast<const xmlpp::TextNode*>(*iterator);
		if (nodeText)
			val = stripBlanks(nodeText->get_content());
	}

	return *this;
}

/* Implementation of "IPParam" class 
 */

string* IPParam::split(string str, char splitter, int& count)
{
	//count splitters
	int splitterCount = 0;
	for (unsigned int i = 0; i < str.size(); i++)
		if (str[i] == splitter)
			splitterCount++;
	//find splitter positions
	int splitterPoses[splitterCount];
	splitterCount = 0;
	for (unsigned int i = 0; i < str.size(); i++)
		if (str[i] == splitter)
			splitterPoses[splitterCount++] = i;
	//extract string parts
	count = splitterCount + 1;
	string* parts = new string[splitterCount + 1];
	parts[0] = str.substr(0, splitterPoses[0]);
	if ((unsigned int) ((((((splitterPoses[splitterCount - 1] + 1))))))
		>= str.size())
		parts[splitterCount] = "";
	else
		parts[splitterCount] = str.substr(
			splitterPoses[splitterCount - 1] + 1);

	for (int i = 1; i < splitterCount; i++)
		if ((unsigned int) ((((((splitterPoses[i - 1] + 1))))))
			>= str.size())
			parts[i] = "";
		else
			parts[i] = str.substr(splitterPoses[i - 1] + 1,
				splitterPoses[i] - splitterPoses[i - 1] - 1);
	return parts;
}
bool IPParam::convertCompactNetmaskToSimple(unsigned int& exNetmask,
	int& simpleForm)
{
	int threshold = 0;
	for (int i = 31; i >= 0; i--)
		if ((exNetmask & (unsigned int) (((((pow(2, i))))))) == 0) {
			threshold = i;
			break;
		}
	for (int i = threshold - 1; i >= 0; i--)
		if ((exNetmask & (unsigned int) (((((pow(2, i))))))) != 0)
			return false;
	simpleForm = 31 - threshold;
	return true;
}
void IPParam::convertSimpleNetmaskToCompact(const int& simpleForm,
	unsigned int& exNetmask) const
{
	exNetmask = 0;
	for (int i = 0; i < simpleForm; i++)
		exNetmask |= (unsigned int) (((((pow(2, i))))));
	exNetmask <<= (32 - simpleForm);
}
bool IPParam::validateString(const string& str, string& allowed)
{
	for (unsigned int i = 0; i < str.size(); i++)
		if (allowed.find(str[i], 0) == str.npos)
			return false;
	return true;
}
string IPParam::getNetmaskString() const
{
	std::ostringstream buff;
	buff << getNetmask();
	return buff.str();
}

IPParam* IPParam::getIP(string name, string iIP)
{
	try {
		IPv4Param *ip4 = new IPv4Param(name);
		*ip4 = iIP;
		return ip4;
	} catch (std::bad_alloc &exception) {
		throw Exception(exception.what(), TracePoint("sparam"));
	} catch (Exception &exp) {
		try {
			IPv6Param *ip6 = new IPv6Param(name);
			*ip6 = iIP;
			return ip6;
		} catch (std::bad_alloc &exception) {
			throw Exception(exception.what(), TracePoint("sparam"));
		} catch (Exception &e) {
		//	throw Exception("IP address is not valid",
			throw Exception("IP '" + iIP + "' is not valid",
				TracePoint("sparam"));
		}
	}
	return NULL;
}

bool IPParam::stringContain(string &str, string &charList)
{
	for (unsigned int i = 0; i < str.size(); i++)
		if (charList.find(str[i], 0) != str.npos)
			return true;
	return false;
}

/** implementaion of "IPv4Param" class */

IPv4Param &IPv4Param::operator =(const IPv4Param &iIP)
{
	set(iIP);
	return *this;
}

IPv4Param &IPv4Param::operator =(const unsigned int &iIP)
{
	set(iIP);
	return *this;
}

IPv4Param &IPv4Param::operator =(const string &iIP)
{
	set(iIP);
	return *this;
}

XParam &IPv4Param::operator =(const XParam &iIP)
{
	set(iIP);
	return *this;
}

void IPv4Param::set(const IPv4Param &iIP) throw (Exception)
{
	if (get_pname() != iIP.get_pname())
		throw Exception(
			"Assigned XParam or IPv4Param has different name",
			TracePoint("sparam"));
	address[0] = iIP.address[0];
	address[1] = iIP.address[1];
	address[2] = iIP.address[2];
	address[3] = iIP.address[3];
	netmask = iIP.netmask;
	containNetmask = iIP.containNetmask;
}

void IPv4Param::set(const unsigned int &iIP)
{
	setAddress(iIP);
}

void IPv4Param::set(const string &iIP) throw (Exception)
{
	char type = 0;
	if (iIP.find('\\', 0) != iIP.npos)
		type = '\\';
	else if (iIP.find('/', 0) != iIP.npos)
		type = '/';
	else
		// if it contains only IP part
		setAddress(iIP);
	if (type != 0) { // so it containt IP and Netmask. SPLIT IT!
		int count;
		string *sparts = split(iIP, type, count);
		if (count != 2 || sparts[0].empty() || sparts[1].empty()) {
			delete[] sparts;
			throw Exception("IP is not valid",
				TracePoint("sparam"));
		} else {
			setAddress(sparts[0]);
			setNetmask(sparts[1]);
			delete[] sparts;
		}
	}
}

void IPv4Param::set(const XParam &iIP) throw (Exception)
{
	const IPv4Param *ip = dynamic_cast<const IPv4Param*>(&iIP);
	if (ip == NULL)
		throw Exception("Bad IP in assignment",
			TracePoint("sparam"));
	set(*ip);
}

void IPv4Param::set(int part1, int part2, int part3, int part4, int netmask)
{
	setAddress(part1, part2, part3, part4);
	setNetmask(netmask);
}

void IPv4Param::set(int addressPart1, int addressPart2, int addressPart3,
	int addressPart4, int netmaskPart1, int netmaskPart2, int netmaskPart3,
	int netmaskPart4)
{
	setAddress(addressPart1, addressPart2, addressPart3, addressPart4);
	setNetmask(netmaskPart1, netmaskPart2, netmaskPart3, netmaskPart4);
}

void IPv4Param::setAddress(const unsigned int &iIP)
{
	setAddress((iIP & 0xFF000000) >> 24, (iIP & 0x00FF0000) >> 16,
		(iIP & 0x0000FF00) >> 8, (iIP & 0x000000FF));
}

void IPv4Param::setAddress(int part1, int part2, int part3, int part4)
	throw (Exception)
{
	if (checkByteRange(part1) && checkByteRange(part2)
		&& checkByteRange(part3) && checkByteRange(part4)) {
		address[0] = part1;
		address[1] = part2;
		address[2] = part3;
		address[3] = part4;
	} else {
		throw Exception("IP is not valid", TracePoint("sparam"));
	}
}

void IPv4Param::setAddress(const string &iIP) throw (Exception)
{
	//validate
	string allowedchars = "0123456789.";
	string hexAllowedchars = "0123456789.abcdefABCDEF";
	int base = 10;
	if (!validateString(iIP, allowedchars)) {
		if (!validateString(iIP, allowedchars))
			throw Exception("IP is not valid",
				TracePoint("sparam"));
		else
			base = 16;
	}
	//is extended?
	if (iIP.find('.', 0) != iIP.npos) {
		int pcount;
		string *sparts = split(iIP, '.', pcount);
		if (pcount != 4 || sparts[0].empty() || sparts[1].empty()
			|| sparts[2].empty() || sparts[3].empty()) {
			delete[] sparts;
			throw Exception("IP is not valid",
				TracePoint("sparam"));
		} else {
			int iparts[4];
			for (int i = 0; i < 4; i++)
				iparts[i] = strtol(sparts[i].c_str(), NULL,
					base);
			delete[] sparts;
			if (checkByteRange(iparts[0])
				&& checkByteRange(iparts[1])
				&& checkByteRange(iparts[2])
				&& checkByteRange(iparts[3]))
				setAddress(iparts[0], iparts[1], iparts[2],
					iparts[3]);
			else
				throw Exception("IP is not valid",
					TracePoint("sparam"));
		}
	} else { // its compact
		unsigned int caddr = strtoul(iIP.c_str(), NULL, base);
		setAddress(caddr);
	}
}

void IPv4Param::setNetmask(const unsigned int &iNetmask)
	throw (Exception)
{
	if (iNetmask >= 0 && iNetmask <= 32) {
		netmask = iNetmask;
		containNetmask = true;
	} else {
		throw Exception("Netmask is not valid",
			TracePoint("sparam"));
	}
}

void IPv4Param::setNetmask(const string &iNetmask) throw (Exception)
{
	//validate
	string allowedchars = "0123456789.";
	string hexAllowedchars = "0123456789.abcdefABCDEF";
	int base = 10;
	if (!validateString(iNetmask, allowedchars)) {
		if (!validateString(iNetmask, allowedchars))
			throw Exception("Netmask is not valid",
				TracePoint("sparam"));
		else
			base = 16;
	}
	//is extended?
	if (iNetmask.find('.', 0) != iNetmask.npos) {
		int pcount;
		string *sparts = split(iNetmask, '.', pcount);
		if (pcount != 4 || sparts[0].empty() || sparts[1].empty()
			|| sparts[2].empty() || sparts[3].empty()) {
			delete[] sparts;
			throw Exception("Netmask is not valid",
				TracePoint("sparam"));
		} else {
			int iparts[4];
			for (int i = 0; i < 4; i++)
				iparts[i] = strtol(sparts[i].c_str(), NULL,
					base);
			delete[] sparts;
			if (checkByteRange(iparts[0])
				&& checkByteRange(iparts[1])
				&& checkByteRange(iparts[2])
				&& checkByteRange(iparts[3]))
				setNetmask(iparts[0], iparts[1], iparts[2],
					iparts[3]);
			else
				throw Exception("Netmask is not valid",
					TracePoint("sparam"));
		}
	} else {
		unsigned int caddr = strtoul(iNetmask.c_str(), NULL, base);
		//compact?
		if (caddr <= 32) {
			setNetmask((int) caddr);
		} else { // simple
			int simpleform;
			bool result = convertCompactNetmaskToSimple(caddr,
				simpleform);
			if (result)
				setNetmask(simpleform);
			else
				throw Exception("Netmask is not valid",
					TracePoint("sparam"));
		}
	}
}

void IPv4Param::setNetmask(int part1, int part2, int part3, int part4)
	throw (Exception)
{
	if (checkByteRange(part1) && checkByteRange(part2)
		&& checkByteRange(part3) && checkByteRange(part4)) {
		unsigned int res = (unsigned int) part1 * 16777216
			+ part2 * 65536 + part3 * 256 + part4;
		int simpleform;
		bool result = convertCompactNetmaskToSimple(res, simpleform);
		if (result)
			setNetmask(simpleform);
		else
			throw Exception("Netmask is not valid",
				TracePoint("sparam"));
	} else {
		throw Exception("Netmask is not valid",
			TracePoint("sparam"));
	}
}

string IPv4Param::getAddress() const
{
	std::ostringstream buff;
	buff << address[0] << "." << address[1] << "." << address[2] << "."
		<< address[3];
	return buff.str();
}

unsigned int IPv4Param::getAddressCompact() const
{
	unsigned int adrs = address[0];
	adrs <<= 24;
	adrs += address[1] << 16;
	adrs += address[2] << 8;
	adrs += address[3];
	return adrs;
}

void IPv4Param::getAddressParts(int *parts)
{
	parts[0] = address[0];
	parts[1] = address[1];
	parts[2] = address[2];
	parts[3] = address[3];
}

int IPv4Param::getNetmask() const
{
	if (containNetmask)
		return netmask;
	else
		return 32;
}

unsigned int IPv4Param::getNetmaskCompact() const
{
	unsigned int cmpctMode;
	convertSimpleNetmaskToCompact(netmask, cmpctMode);
	return cmpctMode;
}

string IPv4Param::getNetmaskExtended()
{
	unsigned int cmpctMode = getNetmaskCompact();
	int parts[4];
	parts[0] = (cmpctMode & 0xFF000000) >> 24;
	parts[1] = (cmpctMode & 0x00FF0000) >> 16;
	parts[2] = (cmpctMode & 0x0000FF00) >> 8;
	parts[3] = (cmpctMode & 0x000000FF);
	std::ostringstream buff;
	buff << parts[0] << "." << parts[1] << "." << parts[2] << "."
		<< parts[3];
	return buff.str();
}

string IPv4Param::value() const
{
	std::ostringstream buff;
	buff << getAddress();
	if (containNetmask)
		buff << "/" << getNetmask();
	return buff.str();
}

bool IPv4Param::checkNetworkAvailability(string IPAddress) const
{
	IPv4Param IPtmp("tmp");
	IPtmp.set(IPAddress);
	if (!haveNetmask())
		return false;
	if ((IPtmp.getAddressCompact() & getNetmaskCompact())
		== (getAddressCompact() & getNetmaskCompact()))
		return true;
	return false;
}

bool IPv4Param::checkNetworkAvailability(IPv4Param IPAddress) const
{
	return checkNetworkAvailability(IPAddress.getAddress());
}

/* Implementation of "IPv6Param" Class 
 */

IPv6Param& IPv6Param::operator =(const IPv6Param& iIP)
{
	set(iIP);
	return *this;
}

IPv6Param& IPv6Param::operator =(const IPv4Param& iIP)
{
	set(iIP);
	return *this;
}

IPv6Param& IPv6Param::operator =(const string& iIP)
{
	set(iIP);
	return *this;
}

XParam& IPv6Param::operator =(const XParam& iIP)
{
	set(iIP);
	return *this;
}

void IPv6Param::set(const IPv6Param& iIP) throw (Exception)
{
	if (get_pname() != iIP.get_pname())
		throw Exception(
			"Assigned XParam or IPv6Param has different name",
			TracePoint("sparam"));

	address[0] = iIP.address[0];
	address[1] = iIP.address[1];
	address[2] = iIP.address[2];
	address[3] = iIP.address[3];
	address[4] = iIP.address[4];
	address[5] = iIP.address[5];
	address[6] = iIP.address[6];
	address[7] = iIP.address[7];
	netmask = iIP.netmask;
	containNetmask = iIP.containNetmask;
}

void IPv6Param::set(const IPv4Param& iIP) throw (Exception)
{
	if (get_pname() != iIP.get_pname())
		throw Exception(
			"Assigned XParam or IPv4Param has different name",
			TracePoint("sparam"));

	address[0] = 0;
	address[1] = 0;
	address[2] = 0;
	address[3] = 0;
	address[4] = 0xffff;
	address[5] = 0;
	address[6] = (iIP.getPart(0) << 8) + iIP.getPart(1);
	address[7] = (iIP.getPart(2) << 8) + iIP.getPart(3);
	netmask = 96 + iIP.get_netmask();
	containNetmask = true;
}

void IPv6Param::set(const string& iIP) throw (Exception)
{
	try {
		char type = 0;
		if (iIP.find('\\', 0) != iIP.npos)
			type = '\\';
		else if (iIP.find('/', 0) != iIP.npos)
			type = '/';
		else
			// if it contains only IP part
			setAddress(iIP);
		if (type != 0) { // so it containt IP and Netmask. SPLIT IT!
			int count;
			string *sparts = split(iIP, type, count);
			if (count != 2 || sparts[0].empty()
				|| sparts[1].empty()) {
				delete[] sparts;
				throw Exception("IP is not valid",
					TracePoint("sparam"));
			} else {
				setAddress(sparts[0]);
				setNetmask(sparts[1]);
				delete[] sparts;
			}
		}
	} catch (Exception &excp) {
		// try for ipv4
		try {
			IPv4Param ip4(this->get_pname());
			ip4 = iIP;
			set(ip4);
		} catch (Exception &e) {
			//give up!
			throw excp;
		}
	}
}

void IPv6Param::set(const XParam& iIP) throw (Exception)
{
	const IPv6Param* ip = dynamic_cast<const IPv6Param*>(&iIP);
	if (ip == NULL)
		throw Exception("Bad IP in assignment",
			TracePoint("sparam"));

	set(*ip);
}

void IPv6Param::set(int part1, int part2, int part3, int part4, int part5,
	int part6, int part7, int part8, int netmask)
{
	setAddress(part1, part2, part3, part4, part5, part6, part7, part8);
	setNetmask(netmask);
}

void IPv6Param::setAddress(const string& iIP) throw (Exception)
{
	//validate
	string allowedIPv6 = "1234567890ABCDEFabcdef:";
	if (validateString(iIP, allowedIPv6)) {
		int partCount = 0;
		string *sparts = split(iIP, ':', partCount);
		//check box count limits
		if (partCount < 3 || partCount > 8)
			throw Exception("IP is not valid",
				TracePoint("sparam"));
		//check first box
		if (sparts[0].empty())
			sparts[0] = "0";
		//check last box
		if (sparts[partCount - 1].empty())
			sparts[partCount - 1] = "0";
		//check for empty box (::)
		int emptyBox = -1;
		for (int i = 0; i < partCount; ++i)
			if (sparts[i].empty()) {
				if (emptyBox == -1)
					emptyBox = i;
				else
					throw Exception(
						"IP is not valid",
						TracePoint("sparam"));
			}
		if (emptyBox == -1) {
			if (partCount == 8) {
				//complete form
				unsigned long int ul;
				int parts[8];
				for (int i = 0; i < 8; ++i) {
					ul = strtol(sparts[i].c_str(), NULL,
						16);
					if (checkWordRange(ul))
						parts[i] = (int) ul;
					else
						throw Exception(
							"IP is not valid",
							TracePoint("sparam"));
				}
				for (int i = 0; i < 8; ++i)
					address[i] = parts[i];
			} else
				throw Exception("IP is not valid",
					TracePoint("sparam"));
		} else {
			//incomplate form
			unsigned long int ul;
			int parts[8];
			//before empty box
			for (int i = 0; i < emptyBox; i++) {
				ul = strtol(sparts[i].c_str(), NULL, 16);
				if (checkWordRange(ul))
					parts[i] = (int) ul;
				else
					throw Exception(
						"IP is not valid",
						TracePoint("sparam"));
			}
			//after empty box
			for (int i = emptyBox + 1; i < partCount; ++i) {
				ul = strtol(sparts[i].c_str(), NULL, 16);
				if (checkWordRange(ul))
					parts[i + (8 - partCount)] = (int) ul;
				else
					throw Exception(
						"IP is not valid",
						TracePoint("sparam"));
			}
			//fill remained empty boexes with 0
			for (int i = emptyBox;
				i < emptyBox + (8 - partCount) + 1; i++)
				parts[i] = 0;
			//copy
			for (int i = 0; i < 8; ++i)
				address[i] = parts[i];
		}
	} else
		throw Exception("IP is not valid", TracePoint("sparam"));
}

void IPv6Param::setAddress(int part1, int part2, int part3, int part4,
	int part5, int part6, int part7, int part8) throw (Exception)
{
	if (checkWordRange(part1) && checkWordRange(part2)
		&& checkWordRange(part3) && checkWordRange(part4)) {
		address[0] = part1;
		address[1] = part2;
		address[2] = part3;
		address[3] = part4;
		address[4] = part5;
		address[5] = part6;
		address[6] = part7;
		address[7] = part8;
	} else {
		throw Exception("IP is not valid", TracePoint("sparam"));
	}
}

string IPv6Param::getAddress() const
{
	int skipStart = -1, skipEnd = -1;
	for (int i = 0; i < 8; ++i)
		if (skipStart == -1 && address[i] == 0)
			skipStart = i;
		else if (skipStart != -1 && skipEnd == -1 && address[i] != 0)
			skipEnd = i - 1;
	if (skipStart != -1 && skipEnd == -1)
		skipEnd = 7;

	std::ostringstream buff;
	bool skipped = false;
	for (int i = 0; i < 8; i++) {
		if (i >= skipStart && i <= skipEnd) {
			if (!skipped) {
				buff << (skipStart == 0 ? "::" : ":");
				skipped = true;
			}
		} else
			buff << hex << address[i] << (i != 7 ? ":" : "");
	}
	return buff.str();
}

string IPv6Param::getAddressComplete() const
{
	std::ostringstream buff;
	buff << hex << address[0] << ":" << address[1] << ":" << address[2]
		<< ":" << address[3] << ":" << address[4] << ":" << address[5]
		<< ":" << address[6] << ":" << address[7];
	return buff.str();
}

void IPv6Param::getAddressParts(int* parts)
{
	parts[0] = address[0];
	parts[1] = address[1];
	parts[2] = address[2];
	parts[3] = address[3];
	parts[4] = address[4];
	parts[5] = address[5];
	parts[6] = address[6];
	parts[7] = address[7];
}

void IPv6Param::setNetmask(const unsigned int& iNetmask)
	throw (Exception)
{
	if (iNetmask >= 0 && iNetmask <= 128) {
		netmask = iNetmask;
		containNetmask = true;
	} else {
		throw Exception("Netmask is not valid",
			TracePoint("sparam"));
	}
}

void IPv6Param::setNetmask(const string& iIP) throw (Exception)
{
	//validate
	string allowedchars = "0123456789";
	if (!validateString(iIP, allowedchars)) {
		throw Exception("Netmask is not valid",
			TracePoint("sparam"));
		return;
	}
	//check
	int caddr = atoi(iIP.c_str());
	if (caddr >= 0 && caddr <= 128) {
		setNetmask(caddr);
	} else
		throw Exception("Netmask is not valid",
			TracePoint("sparam"));
}

int IPv6Param::getNetmask() const
{
	if (containNetmask)
		return netmask;
	else
		return 128;
}

string IPv6Param::value() const
{
	std::ostringstream buff;
	buff << getAddress();
	if (containNetmask)
		buff << "/" << getNetmask();

	return buff.str();
}
bool IPv6Param::checkNetworkAvailability(string IPAddress) const
{
	IPv6Param IPtmp("tmp");
	IPtmp.set(IPAddress);
	if (!haveNetmask())
		return false;

	int tmpNetmask[8] = { };
	int c = netmask;
	for (int i = 0; i < 8 && c > 0; i++)
		for (int j = 15; j >= 0 && c > 0; j--, c--)
			tmpNetmask[i] += pow(2, j);
	for (int i = 0; i < 8; ++i)
		if ((address[i] & tmpNetmask[i])
			!= (IPtmp.address[i] & tmpNetmask[i]))
			return false;
	return true;
}

bool IPv6Param::checkNetworkAvailability(IPv6Param IPAddress) const
{
	return checkNetworkAvailability(IPAddress.getAddress());
}

/* Implementation of "IPxParam" class 
 */

IPxParam &IPxParam::operator = (const IPxParam &ip)
{
	version = ip.version;
	*this = ip.value();

	return *this;
}

XParam &IPxParam::operator = (const string &ip)
{
	IPParam		*ipParam;

	if (ipv4) {
		delete ipv4;
		ipv4 = NULL;
	}
	if (ipv6) {
		delete ipv6;
		ipv6 = NULL;
	}
	try {
		ipParam = getIP(get_pname(), ip);
		ipv4 = dynamic_cast<IPv4Param*>(ipParam);
		if (ipv4) {
			version = IPType::IPv4;

			return *ipv4;
		}
		ipv6 = dynamic_cast<IPv6Param*>(ipParam);
		if (ipv6) {
			version = IPType::IPv6;

			return *ipv6;
		}
		throw Exception(Exception::FAILED,
				"Invalid IP address",
				TracePoint("sparam"));
	}
	catch (Exception &exception) {
		throw exception;
	}
}

XParam &IPxParam::operator = (const XParam &parameter)
{
	const IPxParam	*ip = dynamic_cast<const IPxParam*>(&parameter);

	if (ip == NULL)
		throw Exception("Bad IP in assignment",
			TracePoint("sparam"));
	*this = *ip;

	return *this;
}

string IPxParam::value() const
{
	if ((version == IPType::IPv4) && (ipv4))
		return ipv4->value();
	if ((version == IPType::IPv6) && (ipv6))
		return ipv6->value();

	return "";
}

#if 0
void IPxParam::reset()
{
	if ((version == IPType::IPv4) && (ipv4))
		ipv4->reset();
	if ((version == IPType::IPv6) && (ipv6))
		ipv6->reset();
}
#endif

string IPxParam::getAddress() const
{
	if (version == IPType::IPv4)
		return ipv4->getAddress();
	if (version == IPType::IPv6)
		return ipv6->getAddress();

	return "";
}

IPType::Version IPxParam::getIPVersion()
{
	return version;
}

string IPxParam::getBroadcast() const
{
	if (version == IPType::IPv4)
		return ipv4->getBroadcast();
	if (version == IPType::IPv6)
		return ipv6->getBroadcast();

	return "";
}

string IPxParam::getUnicast() const
{
	if (version == IPType::IPv4)
		return ipv4->getUnicast();
	if (version == IPType::IPv6)
		return ipv6->getUnicast();

	return "";
}

void IPxParam::set(const string &iIP)
{
	if (version == IPType::IPv4)
		return ipv4->set(iIP);
	if (version == IPType::IPv6)
		return ipv6->set(iIP);
}

void IPxParam::set(const XParam &iIP)
{
	if (version == IPType::IPv4)
		return ipv4->set(iIP);
	if (version == IPType::IPv6)
		return ipv6->set(iIP);
}

void IPxParam::setAddress(const string &iIP)
{
	IPParam	*ip = getIP(get_pname(), iIP);

	try {
		if (ip->getIPVersion() == IPType::IPv4) {
			if (ipv4)
				delete ipv4;
			ipv4 = dynamic_cast<IPv4Param*>(ip);
			version = IPType::IPv4;
			ipv4->setAddress(iIP);
		} else if (ip->getIPVersion() == IPType::IPv6) {
			if (ipv6)
				delete ipv6;
			ipv6 = dynamic_cast<IPv6Param*>(ip);
			version = IPType::IPv6;
			ipv6->setAddress(iIP);
		}
	} catch (std::bad_alloc &exception) {
	}
}

void IPxParam::setNetmask(const unsigned int &iNetmask)
{
	if (version == IPType::IPv4)
		return ipv4->setNetmask(iNetmask);
	if (version == IPType::IPv6)
		return ipv6->setNetmask(iNetmask);
}

void IPxParam::setNetmask(const string &iNetmask)
{
	if (version == IPType::IPv4)
		return ipv4->setNetmask(iNetmask);
	if (version == IPType::IPv6)
		return ipv6->setNetmask(iNetmask);
}

int IPxParam::getNetmask() const
{
	if (version == IPType::IPv4)
		return ipv4->getNetmask();
	if (version == IPType::IPv6)
		return ipv6->getNetmask();
	return 32;
}

string IPxParam::getNetmaskString() const
{
	if (version == IPType::IPv4)
		return ipv4->getNetmaskString();
	if (version == IPType::IPv6)
		return ipv6->getNetmaskString();
	return "";
}

bool IPxParam::checkNetworkAvailability(string IPAddress) const
{
	if (version == IPType::IPv4)
		return ipv4->checkNetworkAvailability(IPAddress);
	if (version == IPType::IPv6)
		return ipv6->checkNetworkAvailability(IPAddress);
	return false;
}

/* Implementation of "IPxRangeParam" class */

IPxRangeParam::IPxRangeParam(const string &pname) :
	XMixParam(pname),
	from("from"),
	to("to"),
	_not("not")
{
	addParam(&from);
	addParam(&to);
	addParam(&_not);

	_not.no();
}

IPxRangeParam::IPxRangeParam(IPxRangeParam &&_ipxr) :
	XMixParam(std::move(_ipxr)),
	from(std::move(_ipxr.from)),
	to(std::move(_ipxr.to)),
	_not(std::move(_ipxr._not))
{ 
	addParam(&from);
	addParam(&to);
	addParam(&_not);
}

bool IPxRangeParam::is_not()
{
	return _not.is_enable();
}

void IPxRangeParam::set_not()
{
	_not.yes();
}

void IPxRangeParam::unset_not()
{
	_not.no();
}

bool IPxRangeParam::has_from()
{
	return ! from.value().empty();
}

bool IPxRangeParam::has_to()
{
	return ! to.value().empty();
}

string IPxRangeParam::getFrom()
{
	return from.value();
}

string IPxRangeParam::getTo()
{
	return to.value();
}

IPxParam &IPxRangeParam::get_from()
{
	return from;
}

IPxParam &IPxRangeParam::get_to()
{
	return to;
}

bool IPxRangeParam::key(string &_key)
{
	_key = value();
	return true;
}

string IPxRangeParam::get_key()
{
	return value();
}

string IPxRangeParam::value()
{
	string val = (is_not()) ? "!" : "";
	val += from.getAddress();
	if (to.getAddress() != "0.0.0.0")
		val += '-' + to.getAddress();

	return val;
}

IPType::Version IPxRangeParam::getIPVersion()
{
	return from.getIPVersion();
}

string IPxRangeParam::getAddressFrom() const
{
	return from.getAddress();
}

string IPxRangeParam::getAddressTo() const
{
	return to.getAddress();
}

string IPxRangeParam::getBroadcastFrom() const
{
	return from.getBroadcast();
}

string IPxRangeParam::getBroadcastTo() const
{
	return to.getBroadcast();
}

string IPxRangeParam::getUnicastFrom() const
{
	return from.getUnicast();
}

string IPxRangeParam::getUnicastTo() const
{
	return to.getUnicast();
}

void IPxRangeParam::setFrom(const string &iIP)
{
	from = iIP;
	if (from.getIPVersion() != to.getIPVersion())
		to = "";
}

void IPxRangeParam::setTo(const string &iIP) throw (Exception)
{
	if (!has_from())
		throw Exception("No IP address sets for from",
				TracePoint("sparam")
				);

	to = iIP;
	if (from.getIPVersion() != to.getIPVersion()) {
		to = "";

		throw Exception("IP version mismatch", TracePoint("sparam"));
	}
}

void IPxRangeParam::setFrom(const XParam &iIP)
{
	from.set(iIP);
}

void IPxRangeParam::setTo(const XParam &iIP)
{
	to.set(iIP);
}

void IPxRangeParam::setAddressFrom(const string &iIP)
{
	from.setAddress(iIP);
	if (from.getIPVersion() != to.getIPVersion())
		to = "";
}

void IPxRangeParam::setAddressTo(const string &iIP)
{
	if (!has_from())
		throw Exception("No IP address sets for from",
				TracePoint("sparam")
				);

	to.setAddress(iIP);
	if (from.getIPVersion() != to.getIPVersion()) {
		to = "";

		throw Exception("IP version mismatch", TracePoint("sparam"));
	}
}

void IPxRangeParam::setNetmaskFrom(const unsigned int &iNetmask)
{
	from.setNetmask(iNetmask);
}

void IPxRangeParam::setNetmaskTo(const unsigned int &iNetmask)
{
	to.setNetmask(iNetmask);
}

void IPxRangeParam::setNetmaskFrom(const string &iNetmask)
{
	from.setNetmask(iNetmask);
}

void IPxRangeParam::setNetmaskTo(const string &iNetmask)
{
	to.setNetmask(iNetmask);
}

int IPxRangeParam::getNetmaskFrom() const
{
	return from.getNetmask();
}

int IPxRangeParam::getNetmaskTo() const
{
	return to.getNetmask();
}

string IPxRangeParam::getNetmaskStringFrom() const
{
	return from.getNetmaskString();
}

string IPxRangeParam::getNetmaskString() const
{
	return to.getNetmaskString();
}

/* Implementation of "PortParam" class 
 */

PortParam &PortParam::operator = (const PortParam &portParam)
					throw (Exception)
{
	notSign = portParam.notSign;
	 from = portParam.from;
	 to = portParam.to;
	 portString = portParam.portString;

	return *this;
}

XParam &PortParam::operator = (const unsigned int port) throw (Exception)
{
	std::ostringstream	portNo;

	if (port > MAX_PORT)
		throw Exception(Exception::FAILED,
				"Invalid port number !",
				TracePoint("sparam"));
	portRange = false;
	notSign = false;
	from = port;
	to = INVALID_PORT;
	portNo << port;
	portString = portNo.str();

	return *this;
}

XParam &PortParam::operator = (const string &port) throw (Exception)
{
	bool		_notSign = false;
	bool		doTextProcessing = true;
	bool		setZero = true;
	const char	*_port = port.c_str();
	unsigned char	index;
	unsigned char	state = 0;
	XInt		_from = MIN_PORT;
	XInt		_to = INVALID_PORT;

	for (index = 0; doTextProcessing; index++)
		switch (state) {
			/**
			 * 'start' state
			 * Valid characters are !, : and 0-9
			 */
			case 0:
				if (_port[index] == '!') {
					_notSign = true;
					state = 1;
				}
				else if (isdigit(_port[index])) {
					_from = _from * 10 +
							(_port[index] - 0x30);
					state = 2;
				}
				else if (_port[index] == ':')
					state = 3;
				else
					state = 4;
				break;
			/** Valid characters are : and 0-9 */
			case 1:
				if (_port[index] == ':')
					state = 3;
				else if (isdigit(_port[index])) {
					_from = _from * 10 +
						(_port[index] - 0x30);
					state = 2;
				}
				else
					state = 4;
				break;
			/** Valid characters are 0-9, : and \0 */
			case 2:
				if (isdigit(_port[index]))
					_from = _from * 10 +
						(_port[index] - 0x30);
				else if (_port[index] == ':') {
					_to = MAX_PORT;
					state = 3;
				}
				else if (_port[index] == '\0')
					doTextProcessing = false;
				else
					state = 4;
				break;
			/** Valid characters are 0-9 and \0 */
			case 3:
				if (isdigit(_port[index])) {
					if(setZero) {
						_to = MIN_PORT;
						setZero = false;
					}
					_to = _to * 10 + (_port[index] - 0x30);
				}
				else if (_port[index] == '\0')
					doTextProcessing = false;
				else
					state = 4;
				break;
			default:
				throw Exception(Exception::FAILED,
						"Invalid port format !",
						TracePoint("sparam"));
		}
	if ((_from < MIN_PORT) || (_from > MAX_PORT))
		throw Exception(Exception::FAILED,
				"Invalid port number !",
				TracePoint("sparam"));
	if (state > 2) {
		if ((_to < MIN_PORT) || (_to > MAX_PORT))
			throw Exception(Exception::FAILED,
					"Invalid port number !",
					TracePoint("sparam"));
		if (_to <= _from)
			throw Exception(Exception::FAILED,
					"Invalid port range !",
					TracePoint("sparam"));
	}
	notSign = _notSign;
	from = _from;
	to = _to;
	portString = port;
	portRange = to != INVALID_PORT ? true: false;

	return *this;
}

XParam &PortParam::operator = (const XParam &parameter) throw (Exception)
{
	const PortParam	*port = dynamic_cast<const PortParam*>(&parameter);

	if (port == NULL)
		throw Exception( "Bad PortParam in assignment",
					TracePoint("sparam"));
	notSign = port->notSign;
	from = port->from;
	to = port->to;
	portString = port->portString;

	return *this;
}

/* Implementation of "MACAddressParam" class.
 */
XParam &MACAddressParam::operator = (const string &mac) throw (Exception)
{
	if (macIsValid(mac))
		val = mac;
	else
		throw Exception("Bad MAC Address !",
				TracePoint("sparam"));

	return *this;
}

XParam &MACAddressParam::operator = (const char *mac) throw (Exception)
{
	if (macIsValid(mac))
		val.assign(mac);
	else
		throw Exception("Bad MAC Address !",
				TracePoint("sparam"));

	return *this;
}

bool MACAddressParam::macIsValid(const string &mac)
{
	int length = mac.length();
	if (length != 17) return false;
	for (int i = 0; i < length; ++i) {
		char c = mac[i];
		switch (i % 3) {
		case 0:
		case 1:
			if ((c >= '0' && c <= '9') ||
				(c >= 'a' && c <= 'f') ||
				(c >= 'A' && c <= 'F'))
				continue;
			else return false;
			break;
		case 2:
			if (c != ':') return false;
			break;
		}
	}
	return true;
}

/* Implementation of "DBEngineType" class.
 */

DBEngineParam *DBEngineType::newT() throw (Exception)
{
	DBEngineParam *ret;
	switch (type.get_value()) {
	case DBEngineTypes::SQLite : ret = new SQLiteDBEngineParam(get_pname()); break;
	default:
		throw Exception("Bad type !", TracePoint("sparam"));
		break;
	};
	if (ret == NULL)
		throw Exception("Can't allocate memory !", TracePoint("sparam"));
	return ret;
}

XParam &DBEngineType::operator = (const XmlNode *node) throw (Exception)
{
	set_pname(node->get_name());
	for (iterator iter = params.begin(); iter != params.end(); ++iter) {
		XParam *child = *iter;
		const XParam::XmlNode::NodeList nlist = node->get_children(
			child->get_pname());
		int lsize = nlist.size();
		if (lsize == 1)
			(*child) = *nlist.begin();
		else if (lsize == 0)
			continue;
		else if (lsize > 1)
			/* in mixture parameters, we should have only one
			 * instance for each parameter*/
			throw Exception(
				"There is mutiple " + child->get_pname()
					+ " node !", TracePoint("pparam"));
	}
	return (*this);
}

} // namespace pparam
