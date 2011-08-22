dnl Process this file with autoconf to produce a configure script.
AC_INIT(src/loop.c)

VERSION=5.0.18
PACKAGE=libinduction

AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)

AM_MAINTAINER_MODE

AM_CONFIG_HEADER(src/build_config.h)
AM_CONFIG_HEADER(src/version.h)

AC_DEFINE_UNQUOTED(LITHIUM_VERSION, "${VERSION}")
AC_DEFINE_UNQUOTED(INDUCTION_VERSION, "${VERSION}")

dnl Checks for programs.
AC_PROG_CC_C99
AC_ISC_POSIX
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S

dnl Checks for header files.
AC_HEADER_STDC

# Standard Library Checks
AC_SEARCH_LIBS(dlopen, dl, , AC_MSG_ERROR([Cannot build without dlopen]))
AC_SEARCH_LIBS(crypt, crypto)

# Platform Check
echo "TARGET: ${target}"
case "${target}" in
  *darwin*) echo "*  Darwin host detected (${target}).";
            MODULE_LDFLAGS='-bundle -flat_namespace -undefined suppress -avoid-version'
            AC_DEFINE(OS_DARWIN) ;;
  *) MODULE_LDFLAGS='-no-undefined -avoid-version' ;;
esac
AC_SUBST(MODULE_LDFLAGS)

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
LDFLAGS="$LDFLAGS -lcurl -lssl -lcrypto -lz"
#LDFLAGS="$LDFLAGS -L/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0/Libraries -lcurl -arch i386 -lssl -lcrypto -lz"
AC_SEARCH_LIBS(curl_multi_init, curl, , AC_MSG_ERROR([Cannot build without libcurl]))

# Check for perl
AC_SEARCH_LIBS(perl_alloc, perl)

# Check for DNS-SD
AC_CHECK_HEADER(dns_sd.h, AC_DEFINE(HAVE_DNS_SD), )

# Set custom CFLAGS/LDFLAGS
CFLAGS="$CFLAGS  -W -Wall -Wno-unused -Wno-declaration-after-statement -Wno-unused-parameter -Werror"
LDFLAGS="$LDFLAGS"

dnl ###########################################
dnl Create makefiles

echo "--------------------------------------------------------"
AC_OUTPUT(Makefile \
          src/Makefile \
          )
