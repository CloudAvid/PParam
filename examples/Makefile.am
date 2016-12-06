AM_CPPFLAGS= $(LIBXMLXX_CFLAGS) -I../include

noinst_PROGRAMS= nic user servers user_list user_xlist xlist_test
nic_SOURCES= nic.cpp
user_SOURCES= user.cpp
servers_SOURCES= servers.cpp
user_list_SOURCES= user_list.cpp
user_xlist_SOURCES= user_xlist.cpp
xlist_test_SOURCES= xlist_test.cpp

examples_ldadd= $(LIBXMLXX_LIBS) -L$(top_srcdir)/src/.libs -lpparam
xlist_test_ldadd= $(LIBXMLXX_LIBS) -L$(top_srcdir)/src/.libs -lpparam -lpthread
examples_ldflags= -Wl,--rpath -Wl,$(top_srcdir)/src/.libs

nic_LDADD= $(examples_ldadd)
nic_LDFLAGS= $(examples_ldflags)
user_LDADD= $(examples_ldadd)
user_LDFLAGS= $(examples_ldflags)
servers_LDADD= $(examples_ldadd)
servers_LDFLAGS= $(examples_ldflags)
user_list_LDADD= $(examples_ldadd)
user_list_LDFLAGS= $(examples_ldflags)
user_xlist_LDADD= $(examples_ldadd)
user_xlist_LDFLAGS= $(examples_ldflags)
xlist_test_LDADD= $(xlist_test_ldadd)
xlist_test_LDFLAGS= $(examples_ldflags)
