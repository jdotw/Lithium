#!/bin/bash

if [ -e '/Library/Lithium/LithiumCore.app' ] ; then
  COREPATH="/Library/Lithium/LithiumCore.app/Contents/MacOS/lithium"
  LOGPATH="/Library/Logs/Lithium/hang-`date +%Y%m%d-%H%M%S`.log"
else
  COREPATH="/usr/bin/lithium"
  LOGPATH="/usr/var/log/lithium"
fi

echo "bt" | gdb "$COREPATH" $1 > "$LOGPATH/hang-`date +%Y%m%d-%H%M%S`.log"

kill -15 $1
