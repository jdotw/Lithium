#!/bin/sh

FILES=`find * | grep Makefile\.am`
#FILES=$1

for i in `find * | grep Makefile\.am`
do
  sed -i '' -e 's/module_PROGRAMS/pkglib_LTLIBRARIES/g' $i
  sed -i '' -e 's/\.so/\.la/g' $i
  sed -i '' -e 's/_so_/_la_/g' $i
  sed -i '' -e 's/_la_LDFLAGS = @MODULE_LDFLAGS@/_la_LDFLAGS = -module @MODULE_LDFLAGS@/g' $i
done


