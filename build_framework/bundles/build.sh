#!/bin/bash

BASEDIR=$PWD

if [ $UID -ne 0 ]; then
  echo "ERROR: Must be root to run build script"
  exit 1
fi

COREADMINSRCDIR="$HOME/Source/Core Admin"
cd "$COREADMINSRCDIR"
BUILDNUM=`agvtool vers | grep '^ '  | sed 's/^    //g'`
echo "Build number is $BUILDNUM"
cd $BASEDIR

#
# Clean Unneccessary Stuff
#

## FIX ##

#
# Lithium Core
#

cat Info.plist | sed "s/BUILD_NUMBER/$BUILDNUM/g" > /Library/Lithium/LithiumCore.app/Contents/Info.plist
cp Core.icns /Library/Lithium/LithiumCore.app/Contents/Resources

cd /Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework
cd Versions
ln -sf 5.0 Current
cd ..
ln -sf Versions/Current/Headers Headers
ln -sf Versions/Current/Resources Resources
ln -sf Versions/Current/Support Support
ln -sf Versions/Current/Libraries Libraries
cd $BASEDIR

#
# Lithium Client Service Framework
#

cd /Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumClientService.framework
cd Versions
ln -sf 5.0 Current
cd ..
ln -sf Versions/Current/Headers Headers
ln -sf Versions/Current/Resources Resources
ln -sf Versions/Current/Support Support
ln -sf Versions/Current/Libraries Libraries
cd $BASEDIR

#
# Lithium Database Framework
#

cd /Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumDatabase.framework
cd Versions
ln -sf 5.0 Current
cd ..
ln -sf Versions/Current/Headers Headers
ln -sf Versions/Current/Resources Resources
ln -sf Versions/Current/Support Support
ln -sf Versions/Current/Libraries Libraries
cd $BASEDIR

#
# 

cd $BASEDIR

