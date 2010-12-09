#!/bin/sh

mkdir -p '/Library/Application Support/L49 Import'
/Lithium/bin/pg_dumpall -U lithium -c > '/Library/Application Support/L49 Import/sql.dump'

if [ $? -eq 0 ]; then
  echo "OK"
else
  echo "ERROR"
fi
