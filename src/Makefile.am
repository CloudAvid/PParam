AM_CPPFLAGS= $(LIBXMLXX_CFLAGS) \
	$(SQLITE3_CFLAGS) \
	-I../include

pparamincludedir = $(includedir)/pvm/pparam
pparaminclude_HEADERS = ../include/logs.hpp \
		../include/exception.hpp \
		../include/xdbengine.hpp \
		../include/sparam.hpp \
		../include/xparam.hpp \
		../include/xparam.tcc \
		../include/xlist.hpp \
		../include/xobject.hpp

lib_LTLIBRARIES= libpparam.la
libpparam_la_SOURCES= logs.cpp \
		exception.cpp \
		xparam.cpp \
		sparam.cpp \
		xdbengine.cpp \
		xobject.cpp
libpparam_la_LDFLAGS= -version-info $(LIBPPARAM_SO_VERSION)
libpparam_la_LIBADD= $(LIBXMLXX_LIBS) \
		$(SQLITE3_LIBS) \
		-lssl \
		-lcrypto
