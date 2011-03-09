#!/bin/bash

BASEDIR=$PWD

#
# Get Build Number
#

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

#
# Create dSYM paths
# 

DSYM_PATH="$HOME/Source/Lithium/build_framework/packaging/dSYM Archives/Core/$BUILDNUM-$BUILDNUM_SHORT"
mkdir -p "$DSYM_PATH"

#
# Build Induction
#

cd "$BASEDIR/../../induction"

/Xcode3/usr/bin/make distclean
./bootstrap_osx_universal.sh
if [ $? -ne 0 ]; then
  echo "ERROR: Induction failed to bootstrap"
  exit 1
fi
PATH=/Xcode3/usr/bin:/Xcode3/usr/sbin:$PATH \
/Xcode3/usr/bin/make -j`sysctl -n hw.ncpu`
if [ $? -ne 0 ]; then
  echo "ERROR: Induction failed to build"
  exit 1
fi
PATH=/Xcode3/usr/bin:/Xcode3/usr/sbin:$PATH \
/Xcode3/usr/bin/make install
if [ $? -ne 0 ]; then
  echo "ERROR: Induction failed to install"
  exit 1
fi

#
# Build Lithium Core and Modules
#

cd "$BASEDIR/../../core"

/Xcode3/usr/bin/make distclean
./bootstrap_osx_universal.sh
if [ $? -ne 0 ]; then
  echo "ERROR: Lithium Core failed to bootstrap"
  exit 1
fi
PATH=/Xcode3/usr/bin:/Xcode3/usr/sbin:$PATH \
/Xcode3/usr/bin/make -j`sysctl -n hw.ncpu`
if [ $? -ne 0 ]; then
  echo "ERROR: Lithium Core failed to build"
  exit 1
fi
PATH=/Xcode3/usr/bin:/Xcode3/usr/sbin:$PATH \
/Xcode3/usr/bin/make install
if [ $? -ne 0 ]; then
  echo "ERROR: Lithium Core failed to install"
  exit 1
fi

#
# Capture dSYM
# 

PREFIX=/Library/Lithium/LithiumCore.app
FWPREFIX=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0

/Xcode3/usr/bin/dsymutil -o "$DSYM_PATH/induction.dSYM" "$FWPREFIX/Libraries/libinduction-$BUILDNUM.0.dylib"
/Xcode3/usr/bin/dsymutil -o "$DSYM_PATH/lithium.dSYM" "$PREFIX/Contents/MacOS/lithium"
/Xcode3/usr/bin/dsymutil -o "$DSYM_PATH/mars.dSYM" "$FWPREFIX/Libraries/lithium/mars.so"
/Xcode3/usr/bin/dsymutil -o "$DSYM_PATH/admin.dSYM" "$FWPREFIX/Libraries/lithium/admin.so"
/Xcode3/usr/bin/dsymutil -o "$DSYM_PATH/client_handler.dSYM" "$FWPREFIX/Libraries/lithium/client_handler.so"
/Xcode3/usr/bin/dsymutil -o "$DSYM_PATH/core.dSYM" "$FWPREFIX/Libraries/lithium/core.so"
/Xcode3/usr/bin/dsymutil -o "$DSYM_PATH/customer.dSYM" "$FWPREFIX/Libraries/lithium/customer.so"
/Xcode3/usr/bin/dsymutil -o "$DSYM_PATH/device.dSYM" "$FWPREFIX/Libraries/lithium/device.so"
for i in vendor_*
do
  if [ -d $i ]; then
    cd $i
    MOD_NAME=`echo $i | sed s/vendor_//g`
    /Xcode3/usr/bin/dsymutil -o "$DSYM_PATH/$MOD_NAME.dSYM" "$FWPREFIX/Libraries/lithium/vendor_modules/$MOD_NAME.so"
    cd ..
  fi
done

