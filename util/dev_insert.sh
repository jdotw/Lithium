#!/bin/bash

if [ -s $1 ]; then
  echo "usage: dev_insert.sh <count>"
  exit
fi

echo "DELETE FROM devices;"
for (( i=1 ; i < $1 ; i++ ))
do
  echo "INSERT INTO devices (site, name, descr, ip, snmpversion, snmpcomm, vendor, refresh_interval, protocol) VALUES ('default', 'local$i', 'Local $i', '10.1.1.53', '1', 'public', 'netsnmp', '30', '1');"
done

sudo killall lithium
