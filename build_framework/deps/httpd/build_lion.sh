#!/bin/bash

BASEDIR=$PWD

SDK=/Developer/SDKs/MacOSX10.6.sdk
OSTARGET=10.6
ARCHTARGET=""

UB_CFLAGS="$ARCHTARGET -mmacosx-version-min=$OSTARGET -isysroot $SDK -Wl,-search_paths_first -O"
UB_LDFLAGS="$ARCHTARGET"

MAKEFLAGS=-j`sysctl -n hw.ncpu`

HTTPD=httpd-2.2.19

#
# Sanity Check
#

if [ $UID -ne 0 ]; then
  echo "ERROR: Must be root to run build script"
  exit 1
fi

PREFIX=/Library/Lithium/LithiumCore.app
FWPREFIX=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumClientService.framework/Versions/5.0
CONFIGURE_PATHS="--prefix=$PREFIX --bindir=$PREFIX/Contents/MacOS --sbindir=$PREFIX/Contents/MacOS --libexecdir=$PREFIX/Contents/MacOS --datadir=$FWPREFIX/Contents/Resources/ClientService --sysconfdir=$PREFIX/Contents/Resources/ClientService --sharedstatedir=$PREFIX/Contents/Resources/ClientService --localstatedir=$PREFIX/Contents/Resources/ClientService --libdir=$FWPREFIX/Libraries --includedir=$FWPREFIX/Headers --oldincludedir=$FWPREFIX/Headers --infodir=$PREFIX/Contents/Resources/ClientService --mandir=$PREFIX/Contents/Resources/ClientService"

#
# Apache
#

cd $BASEDIR
rm -rf $HTTPD
tar jxvf $HTTPD.tar.bz2
cd $HTTPD
CFLAGS="$UB_CFLAGS" \
PATH="/Library/Lithium/LithiumCore.app/Contents/MacOS:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin" \
./configure $CONFIGURE_PATHS --with-included-apr --with-port=51180 --with-program-name=clientd --enable-modules=most --enable-ssl --enable-so
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to configure httpd"
  exit 1
fi
make $MAKEFLAGS
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to build httpd"
  exit 1
fi
make install
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to install httpd"
  exit 1
fi
cd ..
rm -rf $HTTPD

cp $BASEDIR/configs/httpd/clientd.conf /Library/Lithium/LithiumCore.app/Contents/Resources/ClientService/clientd.conf
cp $BASEDIR/configs/httpd/httpd-ssl.conf /Library/Lithium/LithiumCore.app/Contents/Resources/ClientService/extra/httpd-ssl.conf


