#!/bin/bash

PID=`cat '/Library/Application Support/Lithium/Monitoring Data/Database/postmaster.pid' | grep ^[0-9]`
sudo -u lithium kill $PID
sudo -u lithium /Library/Lithium/LithiumCore.app/Contents/MacOS/ipcclean

echo "OK"
