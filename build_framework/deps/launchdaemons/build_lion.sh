#!/bin/sh

if [ $UID -ne 0 ]; then
  echo "ERROR: Must be root to run build script" 
  exit 1
fi

mkdir -p /Library/Lithium/LithiumCore.app/Contents/Resources/LaunchDaemons
cp *plist /Library/Lithium/LithiumCore.app/Contents/Resources/LaunchDaemons

exit 0
