#!/bin/bash

util/makedistclean_all.sh

cd induction
./osx_stagegen.sh && make install
if [ $? -ne 0 ]; then
  echo "*!*!*!*!*! ERROR: Induction failed to build"
  exit;
fi
cd ..

cd actuator
./osx_stagegen.sh && make install
if [ $? -ne 0 ]; then
  echo "*!*!*!*!*! ERROR: Actuator failed to build"
  exit;
fi
cd ..

cd construct
./osx_stagegen.sh && make install
if [ $? -ne 0 ]; then
  echo "*!*!*!*!*! ERROR: Construct failed to build"
  exit;
fi
cd ..

cd mars
./osx_stagegen.sh && make install
if [ $? -ne 0 ]; then
  echo "*!*!*!*!*! ERROR: MARS failed to mars"
  exit;
fi
cd ..

cd lithium
./osx_stagegen.sh && make install
if [ $? -ne 0 ]; then
  echo "*!*!*!*!*! ERROR: Lithium failed to build"
  exit;
fi
cd ..

for i in vendor_*
do
  if [ -d $i ]; then
    cd $i
    ./osx_stagegen.sh && make install
    if [ $? -ne 0 ]; then
      echo "*!*!*!*!*!* ERROR: $i failed to build"
      exit
    fi
    cd ..
  fi
done

