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

cd "$DMGDIR"
scp *Console*$BUILDNUM*dmg www.lithiumcorp.com:/www/lithium5.com/download/console

#
# Link 
#

ssh $USER@www.lithiumcorp.com "ln -sf /www/lithium5.com/download/console/LithiumConsole-$BUILDNUM.dmg /www/lithium5.com/download/console/LithiumConsole-CURRENT.dmg"

# 
# Clean up
# 

cd $BASEDIR
