dnl Process this file with autoconf to produce a configure script.
AC_INIT(src/loop.c)

VERSION=5.0.9
PACKAGE=libinduction

AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)

AM_MAINTAINER_MODE

AM_CONFIG_HEADER(src/build_config.h)

AC_DEFINE_UNQUOTED(LITHIUM_VERSION, "${VERSION}")

dnl Checks for programs.
AC_PROG_CC
AC_ISC_POSIX
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S

dnl Checks for header files.
AC_HEADER_STDC

# Standard Library Checks
AC_SEARCH_LIBS(dlopen, dl, , AC_MSG_ERROR([Cannot build without dlopen]))
AC_SEARCH_LIBS(crypt, crypto)

# Check for DB4
AC_SEARCH_LIBS(db_version, db-4, , AC_MSG_ERROR([Cannot build without DB4]))

# Check for PostgreSQL
CFLAGS="$CFLAGS -I`pg_config --includedir`"
LDFLAGS="$LDFLAGS -L`pg_config --libdir`"
AC_CHECK_HEADER(libpq-fe.h)
AC_SEARCH_LIBS(PQexec, pq, , AC_MSG_ERROR([Cannot build without PostgreSQL]))

# Check for SQLite 
AC_SEARCH_LIBS(sqlite3_open,  sqlite3, , AC_MSG_ERROR([Cannot build without sqlite3]))

# Check for SNMP Libs
CFLAGS="$CFLAGS `net-snmp-config --cflags`"
LDFLAGS="$LDFLAGS `net-snmp-config --agent-libs`"
AC_SEARCH_LIBS(init_snmp,  netsnmp, , AC_MSG_ERROR([Cannot build without Net-SNMP]))

# Check for libxml2
CFLAGS="$CFLAGS `xml2-config --cflags`"
LDFLAGS="$LDFLAGS `xml2-config --libs`"
AC_SEARCH_LIBS(xmlReadMemory, xml2, , AC_MSG_ERROR([Cannot build without libxml2]))

# Check for UUID
AC_SEARCH_LIBS(uuid_parse, uuid, , AC_MSG_ERROR([Cannot build without libuuid]))

# Check for libcurl
CFLAGS="$CFLAGS `curl-config --cflags`"
LDFLAGS="$LDFLAGS `curl-config --libs`"
AC_SEARCH_LIBS(curl_multi_init, curl, , AC_MSG_ERROR([Cannot build without libcurl]))

# Check for perl
AC_SEARCH_LIBS(perl_alloc, perl)

# Check for DNS-SD
AC_CHECK_HEADER(dns_sd.h, AC_DEFINE(HAVE_DNS_SD), )

# Set custom CFLAGS/LDFLAGS
CFLAGS="$CFLAGS -W -Wall -Wno-unused-parameter -Werror"

dnl ###########################################
dnl Create makefiles

echo "--------------------------------------------------------"
AC_OUTPUT(Makefile \
          src/Makefile \
          )