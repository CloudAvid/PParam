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

class UserRole
{
public:
	enum {
		ADMINISTRATOR,
		LOCAL,
		REMOTE,
		SUPPORT,
		LOG,
		MAX
	};

	static const string	typeString[MAX];
};

const string	UserRole::typeString[MAX] = {
					"administrator",
					"local",
					"remote",
					"support",
					"log",
					};

class User : public XMixParam
{
public:
	User() :
		XMixParam("user"),
		role("role", UserRole::LOCAL),
		fullName("full_name"),
		username("username"),
		password("password"),
		enabled("enabled"),
		loginDate("login_date")
	{
		addParam(&role);
		addParam(&fullName);
		addParam(&username);
		addParam(&password);
		addParam(&enabled);
		addParam(&loginDate);

		password.set_runtime();
		enabled.yes();
		loginDate.now();
	}
	User(User &&user) : XMixParam(std::move(user)),
			    role(std::move(user.role)),
			    fullName(std::move(user.fullName)),
			    username(std::move(user.username)),
			    password(std::move(user.password)),
			    enabled(std::move(user.enabled)),
			    loginDate(std::move(user.loginDate))
	{
		addParam(&role);
		addParam(&fullName);
		addParam(&username);
		addParam(&password);
		addParam(&enabled);
		addParam(&loginDate);
	}
	void set_fullName(const string _fullName)
	{
		fullName = _fullName;
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
	XEnumParam<UserRole>	role;
	XTextParam		fullName;
	XTextParam		username;
	CryptoParam		password;
	BoolParam		enabled;
	DateTime		loginDate;
};

int main()
{
	User	user;

	user.set_fullName("Example User");
	user.set_username("exam");
	user.set_password("example");
	cout << user.xml(true, 8, true);

	User uuser(std::move(user));
	cout << user.xml(true, 8, true);
	cout << uuser.xml(true, 8, true);

	return 0;
}
