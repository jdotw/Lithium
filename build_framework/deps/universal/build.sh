#!/bin/bash

BASEDIR=$PWD

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
CURL=curl-7.19.6

OLDPATH=$PATH
export PATH=/Library/Lithium/LithiumCore.app/Contents/MacOS:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin 

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
./configure $CONFIGURE_PATHS --disable-xmlreader --without-pear --disable-xml --disable-dependency-tracking --with-apxs2=$PREFIX/Contents/MacOS/apxs --with-mysqli=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0/Support/mysql_config --with-pgsql=/Library/Lithium/LithiumCore.app/Contents/MacOS
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

#
# Clean-up Phase
#

cd $BASEDIR


#
# Reset PREFIX
#

PREFIX=/Library/Lithium/LithiumCore.app
FWPREFIX=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0
CONFIGURE_PATHS="--prefix=$PREFIX --bindir=$PREFIX/Contents/MacOS --sbindir=$PREFIX/Contents/MacOS --libexecdir=$PREFIX/Contents/MacOS --datadir=$PREFIX/Contents/Resources --sysconfdir=$PREFIX/Contents/Resources --sharedstatedir=$PREFIX/Contents/Resources --localstatedir=$PREFIX/Contents/Resources --libdir=$FWPREFIX/Libraries --includedir=$FWPREFIX/Headers --oldincludedir=$FWPREFIX/Headers --infodir=$PREFIX/Contents/Resources --mandir=$PREFIX/Contents/Resources"

#
# DB4
#

cd $BASEDIR
rm -rf $DB4
tar zxvf $DB4.tar.gz
cd $DB4
export CFLAGS="$UB_CFLAGS"
export LDFLAGS="$UB_LDFLAGS" 
export MACOSX_DEPLOYMENT_TARGET=$OSTARGET
cd build_unix
../dist/configure $CONFIGURE_PATHS
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to configure DB4"
  exit 1
fi
make $MAKEFLAGS
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to build DB4"
  exit 1
fi
make install
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to install DB4"
  exit 1
fi
unset CFLAGS
unset LDFLAGS
unset MACOSX_DEPLOYMENT_TARGET
cd ../..
rm -rf $DB4
rm $PREFIX/Support/db_*


#
# WGET
#

cd $BASEDIR
rm -rf $WGET
tar jxvf $WGET.tar.bz2
cd $WGET
./configure $CONFIGURE_PATHS --disable-dependency-tracking \
CFLAGS="$UB_CFLAGS" \
LDFLAGS="$UB_LDFLAGS" \
MACOSX_DEPLOYMENT_TARGET=$OSTARGET
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to configure WGET"
  exit 1
fi
make $MAKEFLAGS
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to build WGET"
  exit 1
fi
make install
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to install WGET"
  exit 1
fi
cd ..
rm -rf $WGET

#
# Curl
#

cd $BASEDIR
rm -rf $CURL
tar jxvf $CURL.tar.bz2
cd $CURL
./configure $CONFIGURE_PATHS --disable-dependency-tracking --without-nss --disable-ldap --disable-ipv6  --disable-manual \
CFLAGS="$UB_CFLAGS" \
LDFLAGS="$UB_LDFLAGS" \
MACOSX_DEPLOYMENT_TARGET=$OSTARGET
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to configure cURL"
  exit 1
fi
make $MAKEFLAGS
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to build cURL"
  exit 1
fi
make install
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to install cURL"
  exit 1
fi
cd ..
rm -rf $CURL

export PATH=$OLDPATH
