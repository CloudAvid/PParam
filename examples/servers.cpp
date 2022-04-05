#include <iostream>
using std::cout;
using std::endl;

#ifdef	HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	EXAMPLE_CODE
#include <sparam.hpp>
#include <xparam.hpp>
#else
#include "pparam/sparam.hpp"
#include "pparam/xparam.hpp"
#endif
using namespace pparam;

class Server : public XMixParam
{
public:
	Server() :
		XMixParam("server"),
		ip("ip"),
		uptime("uptime", 0, -1),
		cpuUsage("cpu_usage", 0, 100),
		ramUsage("ram_usage", 0, 100)
	{
		addParam(&ip);
		addParam(&uptime);
		addParam(&cpuUsage);
		addParam(&ramUsage);
	}
	bool key(string &_key)
	{
		_key = ip.value();

		return true;
	}

	IPxParam		ip;
	XIntParam<XULong>	uptime;
	XIntParam<XFloat>	cpuUsage;
	XIntParam<XFloat>	ramUsage;
};

class Servers : public XSetParam<Server,string>
{
public:
	Servers() :
		XSetParam<Server,string>("servers")
	{
		enable_smap();
	}
	Servers(Servers &&servers) : XSetParam<Server,string>(std::move(servers))
	{ }
};

int main()
{
	Server	server;
	Servers	servers;

	try {
		server.ip = "3fee::1";
		server.uptime = 1759412;
		server.cpuUsage = 27.5f;
		server.ramUsage = 44.0f;
		servers.addT(server);
		server.ip = "192.168.0.1/24";
		server.uptime = 687958;
		server.cpuUsage = 56.2f;
		server.ramUsage = 70.8f;
		servers.addT(server);
	} catch (Exception &exception) {
		cout << exception.what() << endl;

		return -1;
	}

	cout << servers.xml(false, 8, true);

	Servers servers2(std::move(servers));
	cout << servers.xml(false, 8, true);
	cout << servers.size() << endl;
	cout << servers2.xml(false, 8, true);
	cout << servers2.size() << endl;

	return 0;
}
