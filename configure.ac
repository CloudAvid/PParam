#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT(pparam, 1.0.0, pparam@pdnsoft.com)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR(src/, examples/)
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

AC_PROG_CXX
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
LT_INIT([disable-static], [enable-silent-rules])
AC_PROG_LIBTOOL
CXXFLAGS="-O0 -g -std=c++0x -Wall"
CFLAGS="-O0 -g"
AC_LANG([C++])

PKG_CHECK_MODULES([UUID], [uuid])
PKG_CHECK_MODULES([LIBXMLXX], [libxml++-2.6])
PKG_CHECK_MODULES([OPENSSL], [openssl])
PKG_CHECK_MODULES([GLIBMM], [glibmm-2.4])

AC_CHECK_LIB([uuid], [uuid_parse])

AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/file.h sys/socket.h syslog.h unistd.h])
AC_DEFINE([EXAMPLE_CODE], [1], [example code preprocessor defined])

AC_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

AC_FUNC_FORK
AC_CHECK_FUNCS([dup2 memset pow socket strerror strtol strtoul sysinfo])

AC_SUBST([LIBPPARAM_SO_VERSION], [1:0:0])

libdir=/usr/lib64
includedir=/usr/local/include

AC_OUTPUT(Makefile src/Makefile examples/Makefile libpparam-1.0.pc)
