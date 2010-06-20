#! /bin/sh

echo "=============================================================="
for i in vendor_*
do
  if test -d $i; then
    cd $i
    echo -n "$i version: "
    grep VERSION configure.in
    echo "=============================================================="
    cd ..
  fi
done

