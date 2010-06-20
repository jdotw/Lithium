#!/bin/bash

BASEDIR=$PWD

PREFIX=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0

CONFIGURE_PATHS="--prefix=$PREFIX --bindir=$PREFIX/Support --sbindir=$PREFIX/Support --libexecdir=$PREFIX/Support --datadir=$PREFIX/Resources --sysconfdir=$PREFIX/Resources --sharedstatedir=$PREFIX/Resources --localstatedir=$PREFIX/Resources --libdir=$PREFIX/Libraries --includedir=$PREFIX/Headers --oldincludedir=$PREFIX/Headers --infodir=$PREFIX/Resources --mandir=$PREFIX/Resources"

SDK=/Developer/SDKs/MacOSX10.5.sdk
OSTARGET=10.5
ARCHTARGET="-arch i386 -arch ppc"

UB_CFLAGS="$ARCHTARGET -mmacosx-version-min=$OSTARGET -isysroot $SDK -Wl,-search_paths_first -O"
UB_LDFLAGS="$ARCHTARGET"

MAKEFLAGS=-j`sysctl -n hw.ncpu`

HTTPD=httpd-2.2.9
MYSQL=mysql-5.0.67
PHP=php-5.2.6
DB4=db-4.7.25.NC
WGET=wget-1.11.4

#
# Sanity Check
#

if [ $UID -ne 0 ]; then
  echo "ERROR: Must be root to run build script"
  exit 1
fi

#
# MySQL Client
# 

cd $BASEDIR
rm -rf $MYSQL
tar zxvf $MYSQL.tar.gz
cd $MYSQL
./configure $CONFIGURE_PATHS --without-extra-tools --without-bench --without-server --without-docs --without-man --disable-dependency-tracking \
CFLAGS="$UB_CFLAGS" \
CXXFLAGS="$UB_CFLAGS" \
LDFLAGS="$UB_LDFLAGS" \
MACOSX_DEPLOYMENT_TARGET=$OSTARGET
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to configure mysql client"
  exit 1
fi
make $MAKEFLAGS
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to build mysql client"
  exit 1
fi
make install
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to install mysql client"
  exit 1
fi
rm -rf $PREFIX/mysql-test
cd ..
rm -rf $MYSQL


