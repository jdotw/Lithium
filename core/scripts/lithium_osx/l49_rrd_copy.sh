#!/bin/bash

mkdir -p '/Library/Application Support/Lithium/Monitoring Data/History/lithium/'

FILE_COUNT=`rsync -av --dry-run /Lithium/var/lithium/customer-* '/Library/Application Support/Lithium/Monitoring Data/History/lithium/' | grep '^customer' | wc -l`
echo "COUNT: $FILE_COUNT"

rsync -av /Lithium/var/lithium/customer-* '/Library/Application Support/Lithium/Monitoring Data/History/lithium/'

if [ $? -eq 0 ]; then
  echo "OK"
else
  echo "ERROR"
fi
