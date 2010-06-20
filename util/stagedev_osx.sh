#! /bin/sh

echo "=============================================================="
for i in vendor_*
do
  if test -d $i; then
    cd $i
    echo "OSX Build Staging $i"
    ./osx_stagegen.sh
    if $? != 0; then
      echo "Error Building"
      exit
    fi
    make install
    echo "=============================================================="
    cd ..
  fi
done

