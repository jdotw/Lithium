#!/bin/bash

killall -9 lcrrdcached
rm /var/tmp/.lcrrdcached.pid
rm /var/tmp/.lcrrdcached.sock

if [ -f "/Library/Lithium/LithiumCore.app/Contents/MacOS/lcrrdcached" ] ; then
  CACHEDPATH="/Library/Lithium/LithiumCore.app/Contents/MacOS/lcrrdcached"
else
  CACHEDPATH="/usr/bin/lcrrdcached"
fi

$CACHEDPATH -g -l /var/tmp/.lcrrdcached.sock -p /var/tmp/.lcrrdcached.pid -l 127.0.0.1:42217 -w 3600 -z 3600 -f 86400

