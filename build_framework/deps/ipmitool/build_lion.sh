#!/bin/bash

SRC=ipmitool-1.8.11_lc_5.0.tar.gz
SRCDIR=ipmitool-1.8.11
EXECNAME=lcipmitool

LITHIUMPATH=/Library/Lithium/LithiumCore.app/Contents/MacOS

CONFIGARGS=""

BASEDIR=$PWD

if [ -e "cache/lcipmitool_xs3_lion" ]; then
  mkdir -p $LITHIUMPATH
  cp cache/lcipmitool_xs3_lion $LITHIUMPATH/lcipmitool_xs3
  cp cache/lcipmitool_xs1_lion $LITHIUMPATH/lcipmitool_xs1
  echo "[OK] Used cached IPMItool Build for Lion"
  exit 0
fi

BUILDPATH=$PWD/build
STAGEPATH=$PWD/stage
RESULTPATH=$PWD/result

MAKEFLAGS=-j`sysctl -n hw.ncpu`

ARCH=`arch`

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
RBUILDPATH=$RBASEDIR/build
RSTAGEPATH=$RBASEDIR/stage

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
rm -rf build
rm -rf stage
rm -rf result

# Setup Dirs
mkdir build
mkdir stage
mkdir result

# Unpack Local
cd $BUILDPATH
tar zxvf ../$SRC
cd $SRCDIR

# Compile Local
./configure --prefix=$STAGEPATH $CONFIGARGS
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to configure LOCAL ipmitool build"
  exit 1
fi
make $MAKEFLAGS
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to compile LOCAL ipmitool build"
  exit 1
fi

# Get executable 
cp $BUILDPATH/$SRCDIR/src/$EXECNAME $RESULTPATH/$EXECNAME-$LOCALARCH

# Return to base
cd $BASEDIR

# Clean
rm -rf build
rm -rf stage

#
# REMOTE BUILD
#

# Clean
ssh $REMOTE "rm -rf $RBASEDIR"

# Setup dirs 
ssh $REMOTE "mkdir $RBASEDIR"
scp $SRC $REMOTE:$RBASEDIR
ssh $REMOTE "mkdir $RBUILDPATH"
ssh $REMOTE "mkdir $RSTAGEPATH"

# Unpack remote
ssh $REMOTE "cd $RBUILDPATH; tar zxvf $RBASEDIR/$SRC"

# Compile 
ssh $REMOTE "cd $RBUILDPATH/$SRCDIR; ./configure --prefix=$RSTAGEPATH $CONFIGARGS; make"
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to compile REMOTE ipmitool build"
  exit 1
fi

# Get Executable
scp $REMOTE:/$RBUILDPATH/$SRCDIR/src/$EXECNAME $RESULTPATH/$EXECNAME-$REMOTEARCH
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to retrieve REMOTE ipmitool build"
  exit 1
fi

# Remote Clean
ssh $REMOTE "rm -rf $RBASEDIR"

#
# LIPO Time
#

mkdir -p $LITHIUMPATH
sudo lipo -create -arch $LOCALARCH $RESULTPATH/$EXECNAME-$LOCALARCH -arch $REMOTEARCH $RESULTPATH/$EXECNAME-$REMOTEARCH -output $LITHIUMPATH/$EXECNAME
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to lipo the LOCAL and REMOTE builds of ipmitool"
  exit 1
fi

mkdir $BASEDIR/cache
cp $LITHIUMPATH/lcipmitool $BASEDIR/cache/lcipmitool

# Final Clean
rm -rf $BASEDIR/result

echo "[OK] Universal Binary build of IPMItool completed successfully"
