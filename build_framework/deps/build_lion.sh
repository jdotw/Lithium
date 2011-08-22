#!/bin/bash

#
# Create framework/bundle
#

BASEDIR=$PWD

#
# Build
#

cd ipmitool
bash -l ./build_lion.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build IPMItool"
  exit 1
fi
cd ..

cd nmap
bash -l ./build_lion.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build nmap"
  exit 1
fi
cd ..

cd postgres
bash -l ./build_lion.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build PostgreSQL"
  exit 1
fi
bash -l ./build_lion_64.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build PostgreSQL"
  exit 1
fi
cd ..

cd rrdtool
bash -l ./build_lion.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build RRDtool"
  exit 1
fi
cd ..

cd httpd
bash -l ./build_lion.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build apache"
  exit 1
fi
cd ..

cd php
bash -l ./build_lion.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build php"
  exit 1
fi
cd ..

cd launchdaemons
bash -l ./build_lion.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build launchdaemon depenencies"
  exit 1
fi
cd ..

cd perl
bash -l ./build_lion.sh
if [ $? -ne 0 ]; then 
  echo "ERROR: Failed to build perl depenencies"
  exit 1
fi
cd ..

echo "[OK] All dependencies build and installed"
