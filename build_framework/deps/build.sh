#!/bin/bash

#
# Create framework/bundle
#

BASEDIR=$PWD

#
# Build
#

cd mysql
bash -l ./build.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build MySQL Client"
  exit 1
fi
cd ..

cd ipmitool
bash -l ./build.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build IPMItool"
  exit 1
fi
cd ..

cd nmap
bash -l ./build.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build nmap"
  exit 1
fi
cd ..

cd postgres
bash -l ./build.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build PostgreSQL"
  exit 1
fi
cd ..

cd rrdtool
bash -l ./build.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build RRDtool"
  exit 1
fi
cd ..

cd universal
bash -l ./build.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build universal depenencies"
  exit 1
fi
cd ..

cd launchdaemons
bash -l ./build.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build launchdaemon depenencies"
  exit 1
fi
cd ..

cd perl
bash -l ./build.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build perl depenencies"
  exit 1
fi
cd ..

echo "[OK] All dependencies build and installed"
