#include "user_xlist.hpp"

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
	Administrator		administrator;
	RemoteUser		*remoteUser;
	UserList		userList;
	UserList::ListIterator	listIterator;

	try {
		userList.loadXmlDoc("user_list.xml");
		administrator.set_username("root");
		administrator.set_password("root");
		userList.addT(administrator);
	} catch (Exception &exception) {
		cout << exception.what() << endl;
	}
	for (listIterator = userList.begin(); listIterator != userList.end();
			++listIterator) {
		remoteUser = dynamic_cast<RemoteUser*>(*listIterator);
		if (remoteUser)
			cout <<"user: "
				<< remoteUser->get_username() << endl;
	}

	return 0;
}
