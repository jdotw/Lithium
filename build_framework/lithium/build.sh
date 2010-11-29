#!/bin/bash

BASEDIR=$PWD

PREFIX=/Library/Lithium/LithiumCore.app
FWPREFIX=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0
DBFWPREFIX=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumDatabase.framework/Versions/5.0

CONFIGURE_PATHS="--prefix=$PREFIX --bindir=$PREFIX/Contents/MacOS --sbindir=$PREFIX/Contents/MacOS --libexecdir=$PREFIX/Contents/MacOS --datadir=$PREFIX/Contents/Resources --sysconfdir=/Library/Preferences/Lithium --sharedstatedir=$PREFIX/Contents/Resources --libdir=$FWPREFIX/Libraries --includedir=$FWPREFIX/Headers --oldincludedir=$FWPREFIX/Headers --infodir=$PREFIX/Contents/Resources --mandir=$PREFIX/Contents/Resources"

echo "Paths is $CONFIGURE_PATHS"

CC="/Developer/usr/bin/gcc"
SDK="/Developer/SDKs/MacOSX10.5.sdk"
ARCHTARGET="-arch i386 -arch ppc"
OSTARGET="10.5"

COREADMINSRCDIR="$PWD/../../core_admin"
cd "$COREADMINSRCDIR"
BUILDNUM=`agvtool mvers | grep '^Found CFBundleShortVersionString of'  | awk '{ print $4 }' | sed 's/\"//g'`
BUILDNUM_SHORT=`agvtool vers | grep '^ '  | sed 's/^    //g'`
echo "Build number is $BUILDNUM ($BUILDNUM_SHORT)"
cd $BASEDIR

# 
# Setup environment
#

if [ $UID -ne 0 ]; then
  echo "ERROR: Build script must be run as root"
  exit 1
fi

NFRFLAG=""
MSPFLAG=""

if [ "$1" == "NFR" ]; then
  echo "NOTE: Building NFR Version of Lithium"
  NFRFLAG="-DNFR_DEMO"
  MSPFLAG=""
fi

if [ "$1" == "MSP" ]; then
  echo "NOTE: Building MSP Version"
  NFRFLAG=""
  MSPFLAG="--with-coremsp"
fi

#
# Create dSYM paths
# 

DSYM_PATH="$HOME/Source/Lithium/build_framework/packaging/dSYM Archives/Core/$BUILDNUM-$BUILDNUM_SHORT"
mkdir -p "$DSYM_PATH"

#
# Build Lithium and Modules
#

cd "$PWD/../.."

util/makedistclean_all.sh

cd induction
glibtoolize -f -c
aclocal
autoconf
automake -a -c -f
CC="$CC" \
CFLAGS="$NFRFLAG -O -gdwarf-2 -W -Wall -Wno-unused-parameter -Werror -mmacosx-version-min=$OSTARGET -isysroot $SDK $ARCHTARGET" \
LDFLAGS="-Wl,-syslibroot,$SDK" \
./configure $CONFIGURE_PATHS --localstatedir='/Library/Application Support/Lithium/Monitoring Data/History' --with-db-includes=$FWPREFIX/Headers --with-db-libs=$FWPREFIX/Libraries --with-xml-includes=/usr/include/libxml2 --with-xml-libs=/usr/lib --with-postgres-includes=$DBFWPREFIX/Headers --with-postgres-libs=$DBFWPREFIX/Libraries --disable-dependency-tracking
if [ $? -ne 0 ]; then
  echo "ERROR: Induction failed to configure"
  exit 1
fi
make -j`sysctl -n hw.ncpu`
if [ $? -ne 0 ]; then
  echo "ERROR: Induction failed to build"
  exit 1
fi
make install
if [ $? -ne 0 ]; then
  echo "ERROR: Induction failed to install"
  exit 1
fi
cd ..
echo "DSYM_PATH is $DSYM_PATH"
dsymutil -o "$DSYM_PATH/induction.dSYM" "$FWPREFIX/Libraries/libinduction-$BUILDNUM.0.dylib"

cd actuator
glibtoolize -f -c
aclocal
autoconf
automake -a -c -f
CC="$CC" \
CFLAGS="$NFRFLAG -O -gdwarf-2 -W -Wall -Wno-unused-parameter -Werror -mmacosx-version-min=$OSTARGET -isysroot $SDK $ARCHTARGET" \
LDFLAGS="-Wl,-syslibroot,$SDK" \
./configure $CONFIGURE_PATHS --localstatedir='/Library/Application Support/Lithium/Monitoring Data/History' --with-induction-includes=$FWPREFIX/Headers --with-induction-libs=$FWPREFIX/Libraries --with-db-includes=$FWPREFIX/Headers --with-db-libs=$FWPREFIX/Libraries --with-xml-includes=/usr/include/libxml2 --with-xml-libs=/usr/lib --with-postgres-includes=$DBFWPREFIX/Headers --with-postgres-libs=$DBFWPREFIX/Libraries --disable-dependency-tracking
if [ $? -ne 0 ]; then
  echo "ERROR: Actuator failed to configure"
  exit 1
fi
make -j`sysctl -n hw.ncpu`
if [ $? -ne 0 ]; then
  echo "ERROR: Actuator failed to build"
  exit 1
fi
make install
if [ $? -ne 0 ]; then
  echo "ERROR: Actuator failed to install"
  exit 1
fi
cd ..
dsymutil -o "$DSYM_PATH/actuator.dSYM" "$FWPREFIX/Libraries/lithium/actuator.so"

cd construct
glibtoolize -f -c
aclocal
autoconf
automake -a -c -f
CC="$CC" \
CFLAGS="$NFRFLAG -O -gdwarf-2 -W -Wall -Wno-unused-parameter -Werror -mmacosx-version-min=$OSTARGET -isysroot $SDK $ARCHTARGET" \
LDFLAGS="-Wl,-syslibroot,$SDK" \
./configure $CONFIGURE_PATHS --localstatedir='/Library/Application Support/Lithium/Monitoring Data/History' --with-db-includes=$FWPREFIX/Headers --with-db-libs=$FWPREFIX/Libraries --with-induction-includes=$FWPREFIX/Headers --with-induction-libs=$FWPREFIX/Libraries --with-postgres-includes=$DBFWPREFIX/Headers --with-postgres-libs=$DBFWPREFIX/Libraries --with-crypt=crypto --with-xml-includes=/usr/include/libxml2 --with-xml-libs=/usr/lib --disable-dependency-tracking
if [ $? -ne 0 ]; then
  echo "ERROR: Construct failed to configure"
  exit 1
fi
make -j`sysctl -n hw.ncpu`
if [ $? -ne 0 ]; then
  echo "ERROR: Construct failed to build"
  exit 1
fi
make install
if [ $? -ne 0 ]; then
  echo "ERROR: Construct failed to install"
  exit 1
fi
cd ..
dsymutil -o "$DSYM_PATH/lithium.dSYM" "$PREFIX/Contents/MacOS/lithium"

cd mars
glibtoolize -f -c
aclocal
autoconf
automake -a -c -f
CC="$CC" \
CFLAGS="$NFRFLAG -O -gdwarf-2 -W -Wall -Wno-unused-parameter -Werror -mmacosx-version-min=$OSTARGET -isysroot $SDK $ARCHTARGET" \
LDFLAGS="-Wl,-syslibroot,$SDK" \
./configure $CONFIGURE_PATHS --localstatedir='/Library/Application Support/Lithium/Monitoring Data/History' --with-db-includes=$FWPREFIX/Headers --with-db-libs=$FWPREFIX/Libraries --with-induction-includes=$FWPREFIX/Headers --with-induction-libs=$FWPREFIX/Libraries --with-postgres-includes=$DBFWPREFIX/Headers --with-postgres-libs=$DBFWPREFIX/Libraries --disable-dependency-tracking
if [ $? -ne 0 ]; then
  echo "ERROR: MARS failed to configure"
  exit 1
fi
make -j`sysctl -n hw.ncpu`
if [ $? -ne 0 ]; then
  echo "ERROR: MARS failed to build"
  exit 1
fi
make install
if [ $? -ne 0 ]; then
  echo "ERROR: MARS failed to install"
  exit 1
fi
cd ..
dsymutil -o "$DSYM_PATH/mars.dSYM" "$FWPREFIX/Libraries/lithium/mars.so"

cd lithium
glibtoolize -f -c
aclocal
autoconf
automake -a -c -f
CC="$CC" \
CFLAGS="$NFRFLAG -O -gdwarf-2 -W -Wall -Wno-unused-parameter -Werror -mmacosx-version-min=$OSTARGET -isysroot $SDK $ARCHTARGET" \
LDFLAGS="-Wl,-syslibroot,$SDK" \
./configure $CONFIGURE_PATHS --localstatedir='/Library/Application Support/Lithium/Monitoring Data/History' --with-db-includes=$FWPREFIX/Headers --with-db-libs=$FWPREFIX/Libraries --with-induction-includes=$FWPREFIX/Headers --with-induction-libs=$FWPREFIX/Libraries --with-crypt=crypto --with-xml-includes=/usr/include/libxml2 --with-xml-libs=/usr/lib --with-postgres-includes=$DBFWPREFIX/Headers --with-postgres-libs=$DBFWPREFIX/Libraries --disable-dependency-tracking $MSPFLAG
if [ $? -ne 0 ]; then
  echo "ERROR: Lithium failed to configure"
  exit 1
fi
make -j`sysctl -n hw.ncpu`
if [ $? -ne 0 ]; then
  echo "ERROR: Lithium failed to build"
  exit 1
fi
make install
if [ $? -ne 0 ]; then
  echo "ERROR: Lithium failed to install"
  exit 1
fi
cd ..
dsymutil -o "$DSYM_PATH/admin.dSYM" "$FWPREFIX/Libraries/lithium/admin.so"
dsymutil -o "$DSYM_PATH/client_handler.dSYM" "$FWPREFIX/Libraries/lithium/client_handler.so"
dsymutil -o "$DSYM_PATH/core.dSYM" "$FWPREFIX/Libraries/lithium/core.so"
dsymutil -o "$DSYM_PATH/customer.dSYM" "$FWPREFIX/Libraries/lithium/customer.so"
dsymutil -o "$DSYM_PATH/device.dSYM" "$FWPREFIX/Libraries/lithium/device.so"

# cd vendor_xserve
# glibtoolize -f -c
# aclocal
# autoconf
# automake -a -c -f
# CC="/Developer/usr/bin/clang" \
# CFLAGS="$NFRFLAG -O -gdwarf-2 -W -Wall -Wno-unused-parameter -Werror -mmacosx-version-min=$OSTARGET -isysroot $SDK $ARCHTARGET" \
# LDFLAGS="-Wl,-syslibroot,$SDK" \
# ./configure $CONFIGURE_PATHS --localstatedir='/Library/Application Support/Lithium/Monitoring Data/History' --with-induction-includes=$FWPREFIX/Headers --with-lithium-includes=$FWPREFIX/Headers --with-postgres-includes=$DBFWPREFIX/Headers --with-postgres-libs=$DBFWPREFIX/Libraries --with-xml-includes=/usr/include/libxml2 --with-xml-libs=/usr/lib --with-curl-includes=$FWPREFIX/Headers --with-curl-libs=$FWPREFIX/Libraries --disable-dependency-tracking
# if [ $? -ne 0 ]; then
#   echo "ERROR: Failed to configure vendor_xserve"
#   exit 1
# fi
# make -j`sysctl -n hw.ncpu`
# if [ $? -ne 0 ]; then
#   echo "ERROR: Failed to build vendor_xserve"
#   exit 1
# fi
# make install
# if [ $? -ne 0 ]; then
#   echo "ERROR: Failed to install vendor_xserve"
#   exit 1
# fi
# cd ..
# dsymutil -o "$DSYM_PATH/osx_client.dSYM" "$FWPREFIX/Libraries/lithium/vendor_modules/osx_client.so"
# dsymutil -o "$DSYM_PATH/osx_server.dSYM" "$FWPREFIX/Libraries/lithium/vendor_modules/osx_server.so"
# dsymutil -o "$DSYM_PATH/xserve_g5.dSYM" "$FWPREFIX/Libraries/lithium/vendor_modules/xserve_g5.so"
# dsymutil -o "$DSYM_PATH/xserve_intel.dSYM" "$FWPREFIX/Libraries/lithium/vendor_modules/xserve_intel.so"

# cd vendor_xraid
# glibtoolize -f -c
# aclocal
# autoconf
# automake -a -c -f
# CC="/Developer/usr/bin/clang" \
# CFLAGS="$NFRFLAG -O -gdwarf-2 -W -Wall -Wno-unused-parameter -Werror -mmacosx-version-min=$OSTARGET -isysroot $SDK $ARCHTARGET" \
# LDFLAGS="-Wl,-syslibroot,$SDK" \
# ./configure $CONFIGURE_PATHS --localstatedir='/Library/Application Support/Lithium/Monitoring Data/History' --with-induction-includes=$FWPREFIX/Headers --with-lithium-includes=$FWPREFIX/Headers --with-postgres-includes=$DBFWPREFIX/Headers --with-postgres-libs=$DBFWPREFIX/Libraries --with-xml-includes=/usr/include/libxml2 --with-xml-libs=/usr/lib --disable-dependency-tracking
# if [ $? -ne 0 ]; then
#   echo "ERROR: Failed to configure vendor_xraid"
#   exit 1
# fi
# make -j`sysctl -n hw.ncpu`
# if [ $? -ne 0 ]; then
#   echo "ERROR: Failed to build vendor_xraid"
#   exit 1
# fi
# make install
# if [ $? -ne 0 ]; then
#   echo "ERROR: Failed to install vendor_xraid"
#   exit 1
# fi
# cd ..
# dsymutil -o "$DSYM_PATH/xraid.dSYM" "$FWPREFIX/Libraries/lithium/vendor_modules/xraid.so"

# cd ..

#cd modules

#../lithium/util/makedistclean_all.sh

for i in vendor_*
do
  if [ -d $i ]; then
    cd $i
    glibtoolize -f -c
    aclocal
    autoconf
    automake -a -c -f
    CC="$CC" \
    CFLAGS="$NFRFLAG -O -gdwarf-2 -W -Wall -Wno-unused-parameter -Werror -mmacosx-version-min=$OSTARGET -isysroot $SDK $ARCHTARGET" \
    LDFLAGS="-Wl,-syslibroot,$SDK" \
    ./configure $CONFIGURE_PATHS --localstatedir='/Library/Application Support/Lithium/Monitoring Data/History' --with-induction-includes=$FWPREFIX/Headers --with-lithium-includes=$FWPREFIX/Headers --with-postgres-includes=$DBFWPREFIX/Headers --with-postgres-libs=$DBFWPREFIX/Libraries --with-xml-includes=/usr/include/libxml2 --with-xml-libs=/usr/lib --with-curl-includes=$FWPREFIX/Headers --with-curl-libs=$FWPREFIX/Libraries --disable-dependency-tracking
    if [ $? -ne 0 ]; then
      echo "ERROR: $i failed to configure"
      exit
    fi
    make -j`sysctl -n hw.ncpu`
    if [ $? -ne 0 ]; then
      echo "ERROR: $i failed to build"
      exit
    fi
    make install
    if [ $? -ne 0 ]; then
      echo "ERROR: $i failed to install"
      exit
    fi
    cd ..
    MOD_NAME=`echo $i | sed s/vendor_//g`
    dsymutil -o "$DSYM_PATH/$MOD_NAME.dSYM" "$FWPREFIX/Libraries/lithium/vendor_modules/$MOD_NAME.so"
  fi
done

#
# Clean up
#

#
# Copy web files to /Lithium/htdocs
#

rm -rf '/Library/Lithium/LithiumCore.app/Contents/Resources/ClientService/htdocs'
mkdir -p '/Library/Lithium/LithiumCore.app/Contents/Resources/ClientService/htdocs'
cp -r $PWD/htdocs/* '/Library/Lithium/LithiumCore.app/Contents/Resources/ClientService/htdocs'

#
# Copy scripts to /Lithium/share and /Lithium/var/lithium
#

rm -rf '/Library/Lithium/LithiumCore.app/Contents/Resources/ActionScripts'
mkdir -p '/Library/Lithium/LithiumCore.app/Contents/Resources/ActionScripts'
cp -r $PWD/scripts/action_scripts '/Library/Lithium/LithiumCore.app/Contents/Resources/ActionScripts'

rm -rf '/Library/Lithium/LithiumCore.app/Contents/Resources/ServiceScripts'
mkdir -p '/Library/Lithium/LithiumCore.app/Contents/Resources/ServiceScripts'
cp -r $PWD/scripts/service_scripts '/Library/Lithium/LithiumCore.app/Contents/Resources/ServiceScripts'

mkdir -p '/Library/Lithium/LithiumCore.app/Contents/Resources/CoreScripts'
cp -r $PWD/scripts/lithium/*pl /Library/Lithium/LithiumCore.app/Contents/Resources/CoreScripts
cp -r $PWD/scripts/lithium/*sh /Library/Lithium/LithiumCore.app/Contents/Resources/CoreScripts
cp $PWD/scripts/lithium/* /Library/Lithium/LithiumCore.app/Contents/MacOS

