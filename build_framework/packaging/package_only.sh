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
# Create Installer Package
#

/Xcode3/usr/bin/packagemaker \
  --doc "$PKGDIR/3. Distribution Package/Lithium Core 5.0.pmdoc" \
  --out "$INSTALLERDIR/Lithium-Core-$COREBUILDNUM.pkg" \
  --title "Lithium Core $COREBUILDNUM Installer"

cd "$INSTALLERDIR"
zip -r "$DMGDIR/Lithium-Core-$COREBUILDNUM.zip" "Lithium-Core-$COREBUILDNUM.pkg"

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
