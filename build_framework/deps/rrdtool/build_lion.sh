#!/bin/bash

SRC=rrdtool-bundle.tar.gz
SRCDIR=rrdtool-bundle
EXECNAME=lcrrdtool

LITHIUMPATH=/Library/Lithium/LithiumCore.app/Contents/MacOS

BASEDIR=$PWD

if [ -e "cache/lcrrdtool_lion" ]; then
  mkdir -p $LITHIUMPATH
  cp cache/lcrrdtool_lion $LITHIUMPATH/lcrrdtool
  cp cache/lcrrdupdate_lion $LITHIUMPATH/lcrrdupdate
  cp cache/lcrrdcached_lion $LITHIUMPATH/lcrrdcached
  echo "[OK] Used cached LCRRDtool Build for Lion"
  exit 0
fi

ARCH=`arch`

MAKEFLAGS=-j`sysctl -n hw.ncpu`

if [ "$ARCH" == "i386" ]; then
  # Local machine is i386
  LOCALARCH=i386
  REMOTEARCH=ppc
  REMOTE=10.1.1.50
else
  if [ "$ARCH" == "ppc" ]; then
    # Local machine is PPC
    LOCALARCH=ppc
    REMOTEARCH=i386
    REMOTE=10.1.1.53
  else
    echo "ERROR: Unknown local architecture ($ARCH)"
    exit 1
  fi
fi

RBASEDIR=/tmp/lbuild-$EXECNAME-`uname -n`.`date +%s`.$$

RESULTPATH=$BASEDIR/result

#
# Sanity Check
#

if [ $UID -ne 0 ]; then
  echo "ERROR: Must be root to run build script"
  exit 1
fi

#
# LOCAL BUILD
# 

# Clean
rm -rf $RESULTPATH
mkdir $RESULTPATH

# Unpack Local
rm -rf $SRCDIR
tar zxvf $SRC
cd $SRCDIR

# Compile Local
./build.sh
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to build LOCAL rrdtool executables"
  exit 1
fi

# Return to base
cd $BASEDIR

# Get executable 
cp $SRCDIR/result/rrdtool $RESULTPATH/rrdtool-$LOCALARCH
cp $SRCDIR/result/rrdupdate $RESULTPATH/rrdupdate-$LOCALARCH

# Clean
rm -rf $SRCDIR

#
# REMOTE BUILD
#

# Clean
ssh $REMOTE "rm -rf $RBASEDIR"

# Setup dirs 
ssh $REMOTE "mkdir $RBASEDIR"
scp $SRC $REMOTE:$RBASEDIR

# Unpack remote
ssh $REMOTE "cd $RBASEDIR; tar zxvf $SRC"

# Compile 
ssh $REMOTE "cd $RBASEDIR/$SRCDIR; ./build.sh"
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to build REMOTE rrdtool executables"
  exit 1
fi

# Get Executable
scp $REMOTE:$RBASEDIR/$SRCDIR/result/rrdtool $RESULTPATH/rrdtool-$REMOTEARCH
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to retrieve REMOTE rrdtool executable"
  exit 1
fi
scp $REMOTE:$RBASEDIR/$SRCDIR/result/rrdupdate $RESULTPATH/rrdupdate-$REMOTEARCH
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to retrieve REMOTE rrdupdate executable"
  exit 1
fi

# Remote Clean
ssh $REMOTE "rm -rf $RBASEDIR"

#
# LIPO Time
#

sudo lipo -create -arch $LOCALARCH $RESULTPATH/rrdtool-$LOCALARCH -arch $REMOTEARCH $RESULTPATH/rrdtool-$REMOTEARCH -output $LITHIUMPATH/lcrrdtool
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to lipo the LOCAL and REMOTE builds of lcrrdtool"
  exit 1
fi

sudo lipo -create -arch $LOCALARCH $RESULTPATH/rrdupdate-$LOCALARCH -arch $REMOTEARCH $RESULTPATH/rrdupdate-$REMOTEARCH -output $LITHIUMPATH/lcrrdupdate
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to lipo the LOCAL and REMOTE builds of lcrrdupdate"
  exit 1
fi

mkdir $BASEDIR/cache
cp $LITHIUMPATH/lcrrdtool $BASEDIR/cache/lcrrdtool
cp $LITHIUMPATH/lcrrdupdate $BASEDIR/cache/lcrrdupdate

# Final Clean
rm -rf $BASEDIR/result

echo "[OK] Universal Binary build of RRDtool completed successfully"

