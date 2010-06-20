#!/bin/sh

if [ $UID -ne 0 ]; then
  echo "ERROR: Must be root to run build script" 
  exit 1
fi

mkdir -p /Library/Lithium/LithiumCore.app/Contents/Resources/Perl

cp -r modules/* /Library/Lithium/LithiumCore.app/Contents/Resources/Perl

exit 0
