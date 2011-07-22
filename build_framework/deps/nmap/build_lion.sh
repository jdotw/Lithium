#!/bin/bash

SRC=nmap-4.75.tar.bz2
SRCDIR=nmap-4.75
EXECNAME=nmap

LITHIUMPATH=/Library/Lithium/LithiumCore.app/Contents/MacOS

CONFIGARGS="--datarootdir=/Library/Lithium/LithiumCore.app/Contents/Resources"

BASEDIR=$PWD

if [ -e "cache/nmap_lion" ]; then
  mkdir -p $LITHIUMPATH
  cp $BASEDIR/cache/nmap_lion $LITHIUMPATH/nmap
  mkdir -p /Library/Lithium/LithiumCore.app/Contents/Resources/nmap
  cp -r $BASEDIR/cache/nmap-resources/* /Library/Lithium/LithiumCore.app/Contents/Resources/nmap
  echo "[OK] Used cached nmap Build for Lion"
  exit 0
fi

BUILDPATH=$PWD/build
STAGEPATH=$PWD/stage
RESULTPATH=$PWD/result

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
tar jxvf ../$SRC
cd $SRCDIR

# Compile Local
./configure --prefix=$STAGEPATH $CONFIGARGS
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to configure LOCAL nmap build"
  exit 1
fi
make $MAKEFLAGS
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to compile LOCAL nmap build"
  exit 1
fi
make install
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to install LOCAL nmap build"
  exit 1
fi

# Get executable 
cp $STAGEPATH/bin/$EXECNAME $RESULTPATH/$EXECNAME-$LOCALARCH

# Return to base
cd $BASEDIR

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
ssh $REMOTE "cd $RBUILDPATH; tar jxvf $RBASEDIR/$SRC"

# Compile 
ssh $REMOTE "cd $RBUILDPATH/$SRCDIR; ./configure --prefix=$RSTAGEPATH $CONFIGARGS; make; make install"
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to compile REMOTE nmap build"
  exit 1
fi

# Get Executable
scp $REMOTE:/$RSTAGEPATH/bin/$EXECNAME $RESULTPATH/$EXECNAME-$REMOTEARCH
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to retrieve REMOTE nmap executable"
  exit 1
fi

# Remote Clean
ssh $REMOTE "rm -rf $RBASEDIR"

#
# LIPO Time
#

sudo lipo -create -arch $LOCALARCH $RESULTPATH/$EXECNAME-$LOCALARCH -arch $REMOTEARCH $RESULTPATH/$EXECNAME-$REMOTEARCH -output $LITHIUMPATH/$EXECNAME
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to lipo the LOCAL+REMOTE nmap executables"
  exit 1
fi

mkdir $BASEDIR/cache
cp $LITHIUMPATH/nmap $BASEDIR/cache/nmap
rm -rf $BASEDIR/cache/nmap-resources
mkdir -p $BASEDIR/cache/nmap-resources
cp -r /Library/Lithium/LithiumCore.app/Contents/Resources/nmap/* $BASEDIR/cache/nmap-resources

#
# Clean-up Phase
#

rm -rf $BASEDIR/build
rm -rf $BASEDIR/stage
rm -rf $BASEDIR/result

echo "[OK] Universal Binary build of nmap completed successfully."
