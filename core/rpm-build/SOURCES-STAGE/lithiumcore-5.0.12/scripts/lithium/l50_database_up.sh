#!/bin/bash

launchctl unload -w /Library/LaunchDaemons/com.lithiumcorp.database.plist
PID=`cat '/Library/Application Support/Lithium/Monitoring Data/Database/postmaster.pid' | grep ^[0-9]`
if [ $? -eq 0 ]; then
  sudo -u lithium kill $PID
fi
sudo -u lithium /Library/Lithium/LithiumCore.app/Contents/MacOS/ipcclean
sudo -u lithium /Library/Lithium/LithiumCore.app/Contents/MacOS/lcpostmaster -D '/Library/Application Support/Lithium/Monitoring Data/Database' -i 2>&1
