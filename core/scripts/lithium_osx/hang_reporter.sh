#!/bin/sh

echo "bt" | gdb /Library/Lithium/LithiumCore.app/Contents/MacOS/lithium $1 > /Library/Logs/Lithium/hang-`date +%Y%m%d-%H%M%S`.log

kill -15 $1
