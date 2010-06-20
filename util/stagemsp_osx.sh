#!/bin/sh

util/makedistclean_all.sh

cd induction
./osx_stagegen.sh && make install
cd ..

cd actuator
./osx_stagegen.sh && make install
cd ..

cd construct
./osx_stagegen.sh && make install
cd ..

cd mars
./osx_stagegen.sh && make install
cd ..

cd lithium
./osx_stagegen_msp.sh && make install
cd ..


