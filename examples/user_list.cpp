#include "user_list.hpp"

const string	UserRole::typeString[MAX] = {
					"administrator",
					"remote"
					};

User *UserRole::newT() throw (Exception)
{
	if (role.get_value() == ADMINISTRATOR)
		return new Administrator;
	if (role.get_value() == REMOTE)
		return new RemoteUser;

	throw Exception("Undefined user", TracePoint("user"));
}

int main()
{
	Administrator	administrator;
	RemoteUser	remoteUser;
	UserList	userList;

	try {
		administrator.set_username("admin");
		administrator.set_password("admin");
		administrator.set_legalMAC("3f:4e:bc:fd:6a:8f");
		userList.addT(administrator);
		remoteUser.set_username("remote");
		remoteUser.set_password("remote");
		remoteUser.set_domain("example");
		remoteUser.addlegalIP("192.168.0.1/24");
		remoteUser.addlegalIP("3fee::1");
		userList.addT(remoteUser);
	} catch (Exception &exception) {
		cout << exception.what() << endl;
	}
	cout << userList.xml(true, 8, true);

	return 0;
}
