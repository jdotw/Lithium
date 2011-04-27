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
# Amazon AWS
#

cd "$DMGDIR"
mkdir -p "AWS/core/vm"
cp $1 "AWS/core/vm/LithiumCoreVM-$BUILDNUM.$BUILDNUM_SHORT.zip"
s3put -d 2 -c 100 -b l5release -g public-read -p "$PWD/AWS/" "$PWD/AWS/core/vm/"
rm -rf "AWS"

# 
# Clean up
# 

cd $BASEDIR
