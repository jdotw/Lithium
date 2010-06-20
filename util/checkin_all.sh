#! /bin/sh

echo "=============================================================="
for i in *
do
  if test -d $i; then
    cd $i
    echo "Checking in $i (Message: $1)"
    svn commit -m "$1"
    echo "=============================================================="
    cd ..
  fi
done

