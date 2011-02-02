#!/bin/bash

if [ -s $1 ]; then
  echo "usage: create_devices.sh <count> <database> <address> <community>"
  exit
fi

/Library/Lithium/LithiumCore.app/Contents/MacOS/psql -U lithium $2 -c "DELETE FROM devices;"
for (( i=1 ; i < $1 ; i++ ))
do
  /Library/Lithium/LithiumCore.app/Contents/MacOS/psql -U lithium $2 -c "INSERT INTO devices (uuid, site, name, descr, ip, snmpversion, snmpcomm, vendor, refresh_interval, protocol) VALUES ('`uuidgen`', 'default', 'local$i', 'Local $i', '$3', '1', '$4', 'netsnmp', '30', '1');"
done

sudo killall lithium
