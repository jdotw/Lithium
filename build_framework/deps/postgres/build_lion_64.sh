#!/bin/sh

BASEDIR=$PWD

POSTGRES=postgresql-8.3.15
SDK=/Developer/SDKs/MacOSX10.6.sdk
OSTARGET=10.6
ARCHTARGET=""

MAKEFLAGS=-j`sysctl -n hw.ncpu`

PREFIX=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumDatabase64.framework/Versions/5.0
FWPREFIX=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumDatabase64.framework/Versions/5.0

CONFIGURE_PATHS="--prefix=$PREFIX --bindir=$PREFIX/Contents/MacOS --sbindir=$PREFIX/Contents/MacOS --libexecdir=$PREFIX/Contents/MacOS --datadir=$PREFIX/Contents/Resources --sysconfdir=$PREFIX/Contents/Resources --sharedstatedir=$PREFIX/Contents/Resources --localstatedir=$PREFIX/Contents/Resources --libdir=$FWPREFIX/Libraries --includedir=$FWPREFIX/Headers --oldincludedir=$FWPREFIX/Headers --infodir=$PREFIX/Contents/Resources --mandir=$PREFIX/Contents/Resources"

#
# Setup environment
#

if [ $UID -ne 0 ]; then
  echo "ERROR: Build script must be run as root"
  exit 1
fi

cd $BASEDIR
rm -rf build
mkdir build
cd build

# 
# Unpack and Configure
#


tar jxvf $BASEDIR/$POSTGRES.tar.bz2
cd $POSTGRES

./configure $CONFIGURE_PATHS --with-pgport=51132 --disable-dependency-tracking \
CC="/usr/bin/gcc-4.2" \
CFLAGS="$ARCHTARGET -mmacosx-version-min=$OSTARGET -isysroot $SDK -Wl,-search_paths_first -O" \
LDFLAGS="$ARCHTARGET -lz -liconv -ltcl" \
#LD="gcc -mmacosx-version-min=$OSTARGET -isysroot $SDK -nostartfiles $ARCHTARGET" \
MACOSX_DEPLOYMENT_TARGET=$OSTARGET
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to configure PostgreSQL"
  exit 1
fi

# 
# Compile and Install
#

make $MAKEFLAGS
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to compile PostgreSQL"
  exit 1
fi

make install
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to install PostgreSQL"
  exit 1
fi

#
# Fix executable names
#

cd $PREFIX/Contents/MacOS
sudo mv postgres lcpostgres
sudo rm postmaster
sudo ln -sf lcpostgres postmaster
sudo ln -sf lcpostgres postgres
sudo ln -sf lcpostgres lcpostmaster

# 
# Copy configs
#

cd $BASEDIR
cd config
mkdir -p $PREFIX/Contents/Resources/DefaultPreferences/Database
cp * $PREFIX/Contents/Resources/DefaultPreferences/Database


#
# Cleanup phase
#

cd $BASEDIR
rm -rf build

echo "[OK] Universal Binary build of PostgreSQL completed successfully"
