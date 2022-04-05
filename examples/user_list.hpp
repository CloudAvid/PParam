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

class User;

class UserRole : public XMixParam
{
public:
	enum Role {
		ADMINISTRATOR,
		REMOTE,
		MAX
	};

	UserRole() :
		XMixParam("user"),
		role("role", ADMINISTRATOR)
	{
		addParam(&role);
	}
	void set_role(const int _role)
	{
		role = _role;
	}
	User *newT() throw (Exception);

public:
	static const string	typeString[MAX];

private:
	XEnumParam<UserRole>	role;
};

class User : public XMixParam
{
public:
	typedef	UserRole	Type;

	User() :
		XMixParam("user"),
		role("role"),
		username("username"),
		password("password")
	{
		addParam(&role);
		addParam(&username);
		addParam(&password);

		password.set_runtime();
	}
	virtual void type(Type &_type) const
	{
	}
	void set_role(const string _role)
	{
		role = _role;
	}
	string get_username()
	{
		return username.value();
	}
	void set_username(const string _username)
	{
		username = _username;
	}
	void set_password(const string _password)
	{
		password = _password;
		password.encrypt_md5();
	}

private:
	XTextParam	role;
	XTextParam	username;
	CryptoParam	password;
};

class Administrator : public User
{
public:
	Administrator() :
		legalMAC("legal_mac")
	{
		addParam(&legalMAC);

		set_role("administrator");
	}
	virtual void type(Type &_type) const
	{
		_type.set_role(UserRole::ADMINISTRATOR);
	}
	void set_legalMAC(const string mac)
	{
		legalMAC = mac;
	}

private:
	MACAddressParam	legalMAC;
};

class RemoteUser : public User
{
public:
	RemoteUser() :
		domain("domain"),
		legalIPList("legal_ip_list")
	{
		addParam(&domain);
		addParam(&legalIPList);

		set_role("remote");
	}
	virtual void type(Type &_type) const
	{
		_type.set_role(UserRole::REMOTE);
	}
	void set_domain(const string _domain)
	{
		domain = _domain;
	}
	void addlegalIP(const string ip)
	{
		IPxParam	ipx("ip");

		ipx = ip;
		legalIPList.addT(ipx);
	}

private:
	XTextParam	domain;
	IPxList		legalIPList;
};

class UserList : public XISetParam<User>
{
public:
	UserList() :
		XISetParam<User>("user_list")
	{
	}
};
