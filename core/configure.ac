dnl Process this file with autoconf to produce a configure script.
AC_INIT(construct/main.c)

VERSION=5.0.19
PACKAGE=lithium

AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)

AM_MAINTAINER_MODE

AM_CONFIG_HEADER(include/config.h)

AC_DEFINE_UNQUOTED(LITHIUM_VERSION, "${VERSION}")

dnl Configuration headers
AM_CONFIG_HEADER(include/lithium.h)
AM_CONFIG_HEADER(device/snmp.h)

# Platform Check
echo "TARGET: ${target}"
case "${target}" in
  *darwin*) echo "*  Darwin host detected (${target}).";
            MODULE_LDFLAGS='-bundle -flat_namespace -undefined suppress -avoid-version'
            AC_DEFINE(OS_DARWIN) ;;
  *) MODULE_LDFLAGS='-no-undefined -avoid-version' ;;
esac
AC_SUBST(MODULE_LDFLAGS)

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

# Check for induction
AC_CHECK_HEADER(induction.h)
AC_SEARCH_LIBS(i_loop, induction, , AC_MSG_ERROR([Cannot build without Lithium Induction]))

# Check for PostgreSQL
CFLAGS="$CFLAGS -I`pg_config --includedir`"
LDFLAGS="$LDFLAGS -L`pg_config --libdir`"
AC_CHECK_HEADER(libpq-fe.h)
AC_SEARCH_LIBS(PQexec, pq, , AC_MSG_ERROR([Cannot build without PostgreSQL]))

# Check for SQLite 
AC_SEARCH_LIBS(sqlite3_open,  sqlite3, , AC_MSG_ERROR([Cannot build without sqlite3]))

# Check for SNMP Libs
CFLAGS="$CFLAGS `net-snmp-config --cflags | sed 's/-DMACOSX_DEPLOYMENT_TARGET=10.7 //g'`"
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
#LDFLAGS="-L/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0/Libraries -lcurl -arch i386 -lssl -lcrypto -lz"
AC_SEARCH_LIBS(curl_multi_init, curl, , AC_MSG_ERROR([Cannot build without libcurl]))

# Check for perl
AC_SEARCH_LIBS(perl_alloc, perl)

# Check for DNS-SD
AC_CHECK_HEADER(dns_sd.h, AC_DEFINE(HAVE_DNS_SD), )
AC_SEARCH_LIBS(DNSServiceRegister, dns_sd)

# Set custom CFLAGS/LDFLAGS
CFLAGS="$CFLAGS -W -Wall -Wno-unused-parameter -Wno-declaration-after-statement -I.."

dnl ###########################################
dnl Create makefiles

echo "--------------------------------------------------------"
AC_OUTPUT(	Makefile 		\
		include/Makefile \
		construct/Makefile		\
		actuator/Makefile		\
		mars/Makefile		\
		core_module/Makefile		\
		admin/Makefile		\
        client_handler/Makefile \
        customer/Makefile \
        device/Makefile \
          vendor_3com/Makefile \
          vendor_activestorage/Makefile \
          vendor_airport/Makefile \
          vendor_alliedtelesyn/Makefile \
          vendor_apcups/Makefile \
          vendor_cisco/Makefile \
          vendor_fcswitch/Makefile \
          vendor_filer/Makefile \
          vendor_foundry/Makefile \
          vendor_hp/Makefile \
          vendor_icmp/Makefile \
          vendor_infortrend/Makefile \
          vendor_liebert/Makefile \
          vendor_mgeups/Makefile \
          vendor_netbotz/Makefile \
          vendor_netsnmp/Makefile \
          vendor_printer/Makefile \
          vendor_snmpadv/Makefile \
          vendor_snmpbasic/Makefile \
          vendor_vtrak/Makefile \
          vendor_windows/Makefile \
          vendor_xserve/Makefile \
          vendor_xraid/Makefile \
          vendor_brocadefcsw/Makefile \
          scripts/Makefile \
          scripts/action_scripts/Makefile \
          scripts/service_scripts/Makefile \
          scripts/lithium/Makefile \
          htdocs/default/cache/Makefile \
          htdocs/default/image_cache/Makefile \
          htdocs/default/Makefile \
          htdocs/default/admin/Makefile \
          htdocs/diag/Makefile \
          htdocs/images/Makefile \
          htdocs/include/Makefile \
          htdocs/Makefile \
          conf/Makefile \
		)
