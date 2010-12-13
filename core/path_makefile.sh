#!/bin/sh

for i in `find * | grep Makefile\.am | grep vendor`
do
  sed -i '' -e 's/moduledir/pkglibdir/g' $i
done


