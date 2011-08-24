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
CONFIGURE_PATHS="--prefix=$PREFIX --bindir=$PREFIX/Contents/MacOS --sbindir=$PREFIX/Contents/MacOS --libexecdir=$PREFIX/Contents/MacOS --datadir=$FWPREFIX/Contents/Resources/ClientService --sysconfdir=$PREFIX/Contents/Resources/ClientService --sharedstatedir=$PREFIX/Contents/Resources/ClientService --localstatedir=$PREFIX/Contents/Resources/ClientService --libdir=$FWPREFIX/Libraries --includedir=$FWPREFIX/Headers --oldincludedir=$FWPREFIX/Headers --infodir=$PREFIX/Contents/Resources/ClientService --mandir=$PREFIX/Contents/Resources/ClientService"

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

