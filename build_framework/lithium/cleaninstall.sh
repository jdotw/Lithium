#!/bin/sh

if [ $UID -ne 0 ]; then
  echo "ERROR: Build script must be run as root"
  exit 1
fi

launchctl unload /Library/LaunchDaemons/com.lithiumcorp*
rm /Library/LaunchDaemons/com.lithiumcorp*
rm -rf /Library/Lithium /Library/Application\ Support/Lithium /Library/Preferences/Lithium /Library/Logs/Lithium


