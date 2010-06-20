#! /bin/sh

echo "=============================================================="
for i in *
do
  if test -d $i; then
    cd $i
    echo "Changing version of $i from $1 to $2"
    sed -e "s/$1/$2/g" -i '' configure.in
    echo "=============================================================="
    cd ..
  fi
done

