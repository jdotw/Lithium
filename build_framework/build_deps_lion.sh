#!/bin/bash

BASEDIR=$PWD

PREFIX=/Library/Lithium

if [ $UID -ne 0 ]; then
  echo "ERROR: Must be root to run build script"
  exit 1
fi

#
# Clean Existing Prefix 
#

launchctl unload /Library/LaunchDaemons/com.lithiumcorp*
rm -rf $PREFIX
rm -rf '/Library/Application Support/Lithium'
rm -rf '/Library/Logs/Lithium'
rm -rf '/Library/Preferences/Lithium'
rm -rf /Library/LaunchDaemons/com.lithium*
mkdir -p $PREFIX

#
# Check for cache
#

if [ -e "cache/deps-cache-lion.tar.gz" ]; then
  cd $PREFIX
  tar zxvf $BASEDIR/cache/deps-cache-lion.tar.gz
  cd $BASEDIR
  echo "[OK] Used cached Dependencies Build"
  exit 0
fi

#
# Build Deps
#

cd deps
bash -l ./build_lion.sh
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to build Lithium dependencies"
  exit 1
fi
cd ..

#
# Clean Prefix 
# 

rm -rf $PREFIX/LithiumCore.app/doc
rm -rf $PREFIX/LithiumCore.app/docs
rm -rf $PREFIX/LithiumCore.app/Contents/Resources/man*
rm -rf $PREFIX/LithiumCore.app/Contents/Frameworks/LithiumClientService.framework/Versions/5.0/Contents/Resources/ClientService/htdocs/*
rm -rf $PREFIX/LithiumCore.app/Contents/Frameworks/LithiumClientService.framework/Versions/5.0/Contents/Resources/ClientService/icons/*
rm -rf $PREFIX/LithiumCore.app/Contents/Frameworks/LithiumClientService.framework/Versions/5.0/Contents/Resources/ClientService/man*

