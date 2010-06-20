#! /bin/sh

echo "=============================================================="
for i in *
do
  if test -d $i; then
    cd $i
    echo "Making distclean for $i"
    make distclean
    echo "=============================================================="
    cd ..
  fi
done

