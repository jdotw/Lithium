#! /bin/bash

cd induction
./local_osxstage.sh && pump make -j10 CC="distcc clang" && sudo make install
if $? != "0"; then
  echo "********** Error Building induction ************"
  exit
fi
cd ..

cd actuator
./local_osxstage.sh && pump make -j10 CC="distcc clang" && sudo make install
if $? != "0"; then
  echo "********** Error Building actuator ************"
  exit
fi
cd ..

cd construct
./local_osxstage.sh && pump make -j10 CC="distcc clang" && sudo make install
if $? != "0"; then
  echo "********** Error Building construct ************"
  exit
fi
cd ..

cd lithium
./local_osxstage.sh && pump make -j10 CC="distcc clang" && sudo make install
if $? != "0"; then
  echo "********** Error Building lithium ************"
  exit
fi
cd ..

cd mars
./local_osxstage.sh && pump make -j10 CC="distcc clang" && sudo make install
if $? != "0"; then
  echo "********** Error Building mars ************"
  exit
fi
cd ..

echo "=============================================================="
for i in vendor_*
do
  echo $i
  if test -d $i; then
    cd $i
    echo "OSX Local Build Staging $i"
    ./local_osxstage.sh && pump make -j10 CC="distcc clang" && sudo make install
    if $? != "0"; then
      echo "********** Error Building $i ************"
      exit
    fi
    echo "=============================================================="
    cd ..
  fi
done

