#!/bin/sh

killall -9 lcrrdcached
rm /var/tmp/.lcrrdcached.pid
rm /var/tmp/.lcrrdcached.sock

/Library/Lithium/LithiumCore.app/Contents/MacOS/lcrrdcached -g -l /var/tmp/.lcrrdcached.sock -p /var/tmp/.lcrrdcached.pid

