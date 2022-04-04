AM_CPPFLAGS= $(SQLITE3_CFLAGS) \
	-I../include \
	$(LIBXML2_CFLAGS) \
	-I /usr/include/

pparamincludedir = $(includedir)/pvm/pparam
pparaminclude_HEADERS = ../include/logs.hpp \
		../include/exception.hpp \
		../include/xdbengine.hpp \
		../include/sparam.hpp \
		../include/xparam.hpp \
		../include/xparam.tcc \
		../include/xlist.hpp \
		../include/xobject.hpp \
		../include/xml.hpp

lib_LTLIBRARIES= libpparam.la
libpparam_la_SOURCES= logs.cpp \
		exception.cpp \
		xparam.cpp \
		sparam.cpp \
		xdbengine.cpp \
		xobject.cpp \
		xml.cpp

libpparam_la_LDFLAGS= -version-info $(LIBPPARAM_SO_VERSION)
libpparam_la_LIBADD= $(SQLITE3_LIBS) \
		-lssl \
		-lgcrypt \
		$(LIBXML2_LIBS)

