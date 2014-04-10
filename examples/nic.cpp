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
#include <pparam/sparam.hpp>
#include <pparam/xparam.hpp>
#endif
using namespace  pparam;

int main()
{
	XTextParam		device("device");
	IPv4Param		ipv4("ipv4");
	IPv6Param		ipv6("ipv6");
	XIntParam<XUInt>	recievedPackets("rx_packets", 0, -1);
	XIntParam<XUInt>	transferredPackets("tx_packets", 0, -1);

	try {
		device = "eth1";
		ipv4 = "192.168.0.1";
		ipv6 = "3fee::1";
		recievedPackets = 57347;
		transferredPackets = 48936;
	} catch (Exception &exception) {
		cout << "ERROR: " << exception.what() << endl;
	}
	cout << device.xml() << endl;
	cout << ipv4.xml() << endl;
	cout << ipv6.xml() << endl;
	cout << recievedPackets.xml() << endl;
	cout << transferredPackets.xml() << endl;

	return 0;
}
