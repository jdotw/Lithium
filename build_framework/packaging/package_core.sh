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
COREADMINSRCDIR="$PWD/../../core_admin"
CONSOLESRCDIR="$PWD/../../console"
MAINSRCDIR="$PWD/.."
FSDIR="$PWD/4. FileStorm Projects"
DMGDIR="$PWD/Disk Images"
INSTALLERDIR="$PWD/1. Applications/Core Installer"
DSYMDIR="$PWD/dSYM Archives"
BASEDIR=$PWD

#
# Clean 
# 

cd "$DMGDIR"
rm -f *Core*.dmg
rm -f *Core*.zip
rm -f *Console*.dmg
cd "$INSTALLERDIR"
rm -f *.pkg
rm -f *.zip

#
# Get Build Numbers
#

cd "$COREADMINSRCDIR"
COREBUILDNUM=`agvtool mvers | grep '^Found CFBundleShortVersionString of'  | awk '{ print $4 }' | sed 's/\"//g'`
echo "Core Build number is $COREBUILDNUM"

cd "$CONSOLESRCDIR"
CONSOLEBUILDNUM=`agvtool mvers | grep '^Found CFBundleShortVersionString of'  | awk '{ print $4 }' | sed 's/\"//g'`
echo "Console Build number is $CONSOLEBUILDNUM"

#
# Build-Specific Clean
# 

sudo rm -rf "$DSYMDIR/Core/$COREBUILDNUM"
sudo rm -rf "$DSYMDIR/Core Admin/$COREBUILDNUM"
sudo rm -rf "$DSYMDIR/Console/$CONSOLEBUILDNUM"

#
# Build Lithium Core
# 

cd "$MAINSRCDIR"
sudo ./build.sh "$COREBUILDNUM"
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to build Lithium Core"
  exit 1
fi

#
# Build Console
#

cd "$PKGDIR"
./package_console.sh
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to package Lithium Console"
  exit 1
fi

#
# Build Core Admin
#

cd "$COREADMINSRCDIR"

/Developer/usr/bin/xcodebuild -project "LCAdminTools.xcodeproj" \
  -alltargets \
  -configuration "Release" \
  -sdk "macosx10.5" \
  SYMROOT="$PKGDIR/1. Applications/Core Admin" \
  DSTROOT="$PKGDIR/1. Applications/Core Admin" \
  OBJROOT="$PKGDIR/1. Applications/Core Admin" \
  clean

/Developer/usr/bin/xcodebuild -project "LCAdminTools.xcodeproj" \
  -alltargets \
  -configuration "Release" \
  -sdk "macosx10.5" \
  SYMROOT="$PKGDIR/1. Applications/Core Admin" \
  DSTROOT="$PKGDIR/1. Applications/Core Admin" \
  OBJROOT="$PKGDIR/1. Applications/Core Admin" \
  build

if [ $? -ne 0 ]; then
  echo "ERROR: Failed to build Lithium Core Admin"
  exit 1
fi

#
# Archive dSYM File
#

mkdir -p "$DSYMDIR/Core Admin/$COREBUILDNUM"
cp -r "$PKGDIR/1. Applications/Core Admin/Release/Lithium Core Admin.app.dSYM" "$DSYMDIR/Core Admin/$COREBUILDNUM"

#
# Create Installer Package
#

/Developer/usr/bin/packagemaker \
  --doc "$PKGDIR/3. Distribution Package/Lithium Core 5.0.pmdoc" \
  --out "$INSTALLERDIR/Lithium-Core-$COREBUILDNUM.pkg" \
  --title "Lithium Core $COREBUILDNUM Installer"

cd "$INSTALLERDIR"
zip -r "$DMGDIR/Lithium-Core-$COREBUILDNUM.zip" "Lithium-Core-$COREBUILDNUM.pkg"

#
# Clean Core Build Directory
# 

sudo rm -rf "/Library/LithiumBuild-$COREBUILDNUM"
sudo mv "/Library/Lithium" "/Library/LithiumBuild-$COREBUILDNUM"

#
# Package Disk Image
# 

cd "$FSDIR"
osascript "Finalize Console.applescript" "$CONSOLEBUILDNUM"
osascript "Finalize Core.applescript" "$COREBUILDNUM"

# 
# Clean up
# 

cd $BASEDIR
