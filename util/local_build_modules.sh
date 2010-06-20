#! /bin/bash

echo "=============================================================="
for i in *
do
  echo $i
  if test -d $i; then
    cd $i
    echo "OSX Local Build Staging $i"
    ./local_osxstage.sh
    echo "Return is $?"
    if [ $? -ne "0" ]; then
      echo "********** Error Configuring $i ************"
      exit
    fi
    make -j`sysctl -n hw.ncpu` install
    if [ $? -ne "0" ]; then
      echo "********** Error Building $i ************"
      exit
    fi
    echo "=============================================================="
    cd ..
  fi
done

