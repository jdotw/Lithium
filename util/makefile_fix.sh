#! /bin/sh

export CVS_RSH=ssh

echo "=============================================================="
for i in *
do
  if test -d $i; then
    cd $i
    echo "Updating $i"
    cp ~/Source/LithiumArchive/LithiumOld/lithium_xsnmp/lithium/$i/Makefile.am .
    cd ..
    echo "=============================================================="
  fi
done

