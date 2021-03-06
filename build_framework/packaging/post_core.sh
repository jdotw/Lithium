#!/bin/bash

PKGDIR="$PWD"
SRCDIR="$PWD/../../core_admin"
FSDIR="$PWD/4. FileStorm Projects"
DSYMDIR="$PWD/dSYM Archives"
DMGDIR="$PWD/Disk Images"
BASEDIR=$PWD


#
# Get Build Number
#

cd "$SRCDIR"
BUILDNUM=`agvtool mvers | grep '^Found CFBundleShortVersionString of'  | awk '{ print $4 }' | sed 's/\"//g'`
BUILDNUM_SHORT=`agvtool vers | grep '^ '  | sed 's/^    //g'`
echo "Build number is $BUILDNUM ($BUILDNUM_SHORT)"

#
# Scp 
#

#cd "$DMGDIR"
#scp *Core*$BUILDNUM*dmg www.lithiumcorp.com:/www/download.lithiumcorp.com/lithium5/core/osx
#scp *Core*$BUILDNUM*zip www.lithiumcorp.com:/www/download.lithiumcorp.com/lithium5/core/osx/updater

#
# Amazon AWS
#

cd "$DMGDIR"
mkdir -p "AWS/core/osx"
mkdir -p "AWS/core/osx/updater"
cp LithiumCore-$BUILDNUM.dmg "AWS/core/osx/LithiumCore-$BUILDNUM.$BUILDNUM_SHORT.dmg"
cp Lithium-Core-$BUILDNUM.zip "AWS/core/osx/updater/Lithium-Core-$BUILDNUM.zip"
s3put -d 2 -c 100 -b l5release -g public-read -p "$PWD/AWS/" "$PWD/AWS/core/osx/"
rm -rf "AWS"

#
# Link 
#

#ssh $USER@www.lithiumcorp.com "ln -sf /www/download.lithiumcorp.com/lithium5/core/osx/LithiumCore-$BUILDNUM.dmg /www/download.lithiumcorp.com/lithium5/core/osx/LithiumCore-CURRENT.dmg"
#ssh $USER@www.lithiumcorp.com "ln -sf /www/download.lithiumcorp.com/lithium5/core/osx/updater/Lithium-Core-$BUILDNUM.zip /www/download.lithiumcorp.com/lithium5/core/osx/updater/Lithium-Core-CURRENT.zip"

# 
# Clean up
# 

cd $BASEDIR
