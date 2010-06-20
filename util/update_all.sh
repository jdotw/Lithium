#! /bin/sh

export CVS_RSH=ssh

echo "=============================================================="
for i in *
do
  if test -d $i; then
    cd $i
    echo "Updating $i"
    svn update
    cd ..
    echo "=============================================================="
  fi
done

