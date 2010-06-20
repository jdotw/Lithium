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

echo "Build number is $BUILDNUM"

#
# Scp 
#

cd "$DMGDIR"
scp *Core*$BUILDNUM*dmg www.lithiumcorp.com:/www/lithium5.com/download/core/osx
scp *Core*$BUILDNUM*zip www.lithiumcorp.com:/www/lithium5.com/download/core/osx/updater

#
# Link 
#

ssh $USER@www.lithiumcorp.com "ln -sf /www/lithium5.com/download/core/osx/LithiumCore-$BUILDNUM.dmg /www/lithium5.com/download/core/osx/LithiumCore-CURRENT.dmg"
ssh $USER@www.lithiumcorp.com "ln -sf /www/lithium5.com/download/core/osx/updater/Lithium-Core-$BUILDNUM.zip /www/lithium5.com/download/core/osx/updater/Lithium-Core-CURRENT.zip"

# 
# Clean up
# 

cd $BASEDIR
