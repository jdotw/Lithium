#!/bin/bash

BASEDIR=$PWD

PREFIX=/Library/Lithium

if [ $UID -ne 0 ]; then
  echo "ERROR: Must be root to run build script"
  exit 1
fi

#
# Build Lithium
#

cd lithium
bash -l ./build.sh
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to build Lithium"
  exit 1
fi
cd ..

#
# Build Bundles
#

cd bundles
bash -l ./build.sh
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to build Bundles"
  exit 1
fi
cd ..


