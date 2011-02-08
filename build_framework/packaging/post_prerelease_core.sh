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

COREADMINSRCDIR="$PWD/../../core_admin"
cd "$COREADMINSRCDIR"
BUILDNUM=`agvtool mvers | grep '^Found CFBundleShortVersionString of'  | awk '{ print $4 }' | sed 's/\"//g'`
BUILDNUM_SHORT=`agvtool vers | grep '^ '  | sed 's/^    //g'`
echo "Build number is $BUILDNUM ($BUILDNUM_SHORT)"
cd $BASEDIR

#
# Scp 
#

cd "$DMGDIR"
scp LithiumCore-$BUILDNUM.dmg www.lithiumcorp.com:/www/download.lithiumcorp.com/lithium5/core/osx/prerelease/LithiumCore-$BUILDNUM.$BUILDNUM_SHORT.dmg

# 
# Clean up
# 

cd $BASEDIR

#
# Print URL
#

echo "Uploaded to http://download.lithiumcorp.com/lithium5/core/osx/prerelease/LithiumCore-$BUILDNUM.$BUILDNUM_SHORT.dmg"
