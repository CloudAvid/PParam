#!/usr/bin/sh

oslist="Ubuntu Debian CentOS"
for linux in $oslist; do
	os=`grep -o -a -m 1 -h -r "$linux" /etc/issue | head -1`
done
	
if [ "CentOS" == "$os" ]
then
	echo "Waiting ...."
	echo "Installing packages ...."
	yum install automake autoconf libtool libtool-ltdl-devel uuid-devel uuid libxml++-devel sqlite-devel -y
	echo "Successfully installed packages"

elif [ "$os" == "Ubuntu" ] || [ "$os" == "Debian" ]
then
	echo "Waiting ...."
	echo "Installing packages ...."
	apt-get install automake autoconf libtool libltdl-dev  sqlite3 libsqlite3-dev libxml++2.6-dev uuid-dev -y
	echo "Successfully installed packages"

else
	echo "$os is NOT match"
fi
exit 0
