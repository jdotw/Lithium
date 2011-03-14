#!/bin/bash

#
# Sanity Check
#

if [ $UID -eq 0 ]; then
  echo "ERROR: Do NOT run this script as root or with sudo!!!"
  exit 1
fi

#
# Variables
# 

PKGDIR="$PWD"
SRCDIR="$PWD/../../console"
FSDIR="$PWD/4. FileStorm Projects"
DSYMDIR="$PWD/dSYM Archives"
BASEDIR=$PWD


#
# Get Build Number
#

cd "$SRCDIR"
#agvtool bump -all
BUILDNUM=`agvtool mvers | grep '^Found CFBundleShortVersionString of'  | awk '{ print $4 }' | sed 's/\"//g'`

echo "Build number is $BUILDNUM"

#
# Build-Specific Clean
# 

sudo rm -rf "$DSYMDIR/Console/$BUILDNUM"

#
# Build Console
#

cd "$SRCDIR"

/Xcode3/usr/bin/xcodebuild -project "Lithium Console.xcodeproj" \
  -alltargets \
  -configuration "Release 10.6" \
  -sdk "macosx10.5" \
  SYMROOT="$PKGDIR/1. Applications/Console" \
  DSTROOT="$PKGDIR/1. Applications/Console" \
  OBJROOT="$PKGDIR/1. Applications/Console" \
  clean

/Xcode3/usr/bin/xcodebuild -project "Lithium Console.xcodeproj" \
  -alltargets \
  -configuration "Release 10.6" \
  -sdk "macosx10.5" \
  SYMROOT="$PKGDIR/1. Applications/Console" \
  DSTROOT="$PKGDIR/1. Applications/Console" \
  OBJROOT="$PKGDIR/1. Applications/Console" \
  build

#
# Archive dSYM File
#

mkdir -p "$DSYMDIR/Console/$BUILDNUM"
cp -r "$PKGDIR/1. Applications/Console/Release 10.6/Lithium Console.app.dSYM" "$DSYMDIR/Console/$BUILDNUM"

#
# Package Disk Image
# 

cd "$FSDIR"
osascript "Finalize Console.applescript" "$BUILDNUM"

# 
# Clean up
# 

cd $BASEDIR
