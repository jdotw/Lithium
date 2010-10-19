#!/bin/bash

if [ -s $1 ]; then
  echo "usage: pollute_iface_metrics.sh <count> <database>"
  exit
fi

echo "DELETE FROM iface_metrics;"
for (( i=1 ; i < $1 ; i++ ))
do
  /Library/Lithium/LithiumCore.app/Contents/MacOS/psql -U lithium $2 -c "INSERT INTO iface_metrics (site, device, object, metric, mday, month, year, valstr, valnum, tstamp) VALUES ('default', 'local1', 'en1', 'mb_month_out', '0', '10', '2010', '', '1048576.00', '1287432808')"
done

