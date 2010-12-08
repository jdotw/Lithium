#!/bin/sh

sudo -u lithium /Library/Lithium/LithiumCore.app/Contents/MacOS/createdb lithium
/Library/Lithium/LithiumCore.app/Contents/MacOS/psql -U lithium -f '/Library/Application Support/L49 Import/sql.dump'

if [ $? -eq 0 ]; then
  echo "OK"
else
  echo "ERROR"
fi
