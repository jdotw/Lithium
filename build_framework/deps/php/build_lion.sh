#!/bin/bash

BASEDIR=$PWD

SDK=/Developer/SDKs/MacOSX10.6.sdk
OSTARGET=10.6
ARCHTARGET=""

UB_CFLAGS="$ARCHTARGET -mmacosx-version-min=$OSTARGET -isysroot $SDK -Wl,-search_paths_first -O"
UB_LDFLAGS="$ARCHTARGET"

MAKEFLAGS=-j`sysctl -n hw.ncpu`

PHP=php-5.3.7


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
# PHP
#

cd $BASEDIR
rm -rf $PHP
tar jxvf $PHP.tar.bz2
cd $PHP
CFLAGS="$UB_CFLAGS" \
LDFLAGS="$UB_LDFLAGS" \
MACOSX_DEPLOYMENT_TARGET=$OSTARGET \
PATH="/Library/Lithium/LithiumCore.app/Contents/MacOS:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin" \
./configure $CONFIGURE_PATHS --disable-xmlreader --without-pear --disable-xml --disable-dependency-tracking --with-apxs2=$PREFIX/Contents/MacOS/apxs --with-pgsql=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumDatabase64.framework/Versions/5.0/Contents/MacOS
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to configure PHP"
  exit 1
fi
make $MAKEFLAGS
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to build PHP"
  exit 1
fi
make install
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to install PHP"
  exit 1
fi
cd ..
rm -rf $PHP

cp $BASEDIR/configs/php/* /Library/Lithium/LithiumCore.app/Contents/Resources/

