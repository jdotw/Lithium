#!/bin/bash

PKGDIR="$PWD/packaging"
SRCDIR="$PWD/../../console"
FSDIR="$PWD/4. FileStorm Projects"
DSYMDIR="$PWD/dSYM Archives"
DMGDIR="$PWD/Disk Images"
BASEDIR=$PWD


#
# Get Build Number
#

cd "$SRCDIR"
BUILDNUM=`agvtool mvers | grep '^Found CFBundleShortVersionString of'  | awk '{ print $4 }' | sed 's/\"//g'`

echo "Build number is $BUILDNUM"

#
# Scp 
#

#cd "$DMGDIR"
#scp *Console*$BUILDNUM*dmg www.lithiumcorp.com:/www/download.lithiumcorp.com/lithium5/console

#
# Amazon AWS
#

cd "$DMGDIR"
mkdir -p "AWS/console"
cp LithiumConsole-$BUILDNUM.dmg "AWS/console/LithiumConsole-$BUILDNUM.dmg"
s3put -d 2 -c 100 -b l5release -g public-read -p "AWS" AWS/console/LithiumConsole-$BUILDNUM.dmg
rm -rf "AWS"

#
# Link 
#

#ssh $USER@www.lithiumcorp.com "ln -sf /www/download.lithiumcorp.com/lithium5/console/LithiumConsole-$BUILDNUM.dmg /www/download.lithiumcorp.com/lithium5/console/LithiumConsole-CURRENT.dmg"

# 
# Clean up
# 

cd $BASEDIR
