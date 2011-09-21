#!/bin/bash

BASEDIR=$PWD

SDK=/Developer/SDKs/MacOSX10.6.sdk
OSTARGET=10.6
ARCHTARGET=""

UB_CFLAGS="$ARCHTARGET -mmacosx-version-min=$OSTARGET -isysroot $SDK -Wl,-search_paths_first -O"
UB_LDFLAGS="$ARCHTARGET"

MAKEFLAGS=-j`sysctl -n hw.ncpu`

NETSNMP=net-snmp-5.7

#
# Sanity Check
#

if [ $UID -ne 0 ]; then
  echo "ERROR: Must be root to run build script"
  exit 1
fi

PREFIX=/Library/Lithium/LithiumCore.app
FWPREFIX=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0
CONFIGURE_PATHS="--prefix=$PREFIX --bindir=$PREFIX/Contents/MacOS --sbindir=$PREFIX/Contents/MacOS --libexecdir=$PREFIX/Contents/MacOS --datadir=$FWPREFIX/Contents/Resources/SNMP --sysconfdir=$PREFIX/Contents/Resources/SNMP --sharedstatedir=$PREFIX/Contents/Resources/SNMP --localstatedir=$PREFIX/Contents/Resources/SNMP --libdir=$FWPREFIX/Libraries --includedir=$FWPREFIX/Headers --oldincludedir=$FWPREFIX/Headers --infodir=$PREFIX/Contents/Resources/SNMP --mandir=$PREFIX/Contents/Resources/SNMP"

#
# Apache
#

cd $BASEDIR
rm -rf $NETSNMP
tar zxvf $NETSNMP.tar.gz
cd $NETSNMP
CFLAGS="$UB_CFLAGS" \
PATH="/Library/Lithium/LithiumCore.app/Contents/MacOS:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin" \
./configure $CONFIGURE_PATHS --enable-ipv6
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to configure netsnmp"
  exit 1
fi
make $MAKEFLAGS
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to build netsnmp"
  exit 1
fi
make install
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to install netsnmp"
  exit 1
fi
cd ..
rm -rf $NETSNMP

#
# Clean target
#

rm -rf /Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0/Contents/Resources/SNMP/snmp/mib2c-data

rm -rf /Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0/Contents/Resources/SNMP/snmp/mib2c/*

rm -rf /Library/Lithium/LithiumCore.app/Contents/Resources/SNMP/man*



