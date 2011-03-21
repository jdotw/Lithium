#!/bin/bash

if [ -e '/Lithium/WebServer/Documents' ] ; then

  cd '/Lithium/WebServer/Documents/'

  for i in *
  do

    if [ ! -d "$i" ] ; then
      continue
    fi
    if test "$i" = "default" ; then
      continue
    fi
    if test "$i" = "diag" ; then
      continue
    fi
    if test "$i" = "images" ; then
      continue
    fi
    if test "$i" = "include" ; then
      continue
    fi

    echo "Processing $i"
    mkdir -p /Library/Application\ Support/Lithium/ClientService/Resources/htdocs/$i
    cp $i/profile.php /Library/Application\ Support/Lithium/ClientService/Resources/htdocs/$i/profile.php
  done
fi

/Library/Lithium/LithiumCore.app/Contents/MacOS/repairweb.sh

echo "OK"
