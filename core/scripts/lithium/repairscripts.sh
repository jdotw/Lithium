#!/bin/bash

# Repairs action and service scripts

if [ -e '/Library/Lithium/LithiumCore.app' ] ; then
  LITHIUMDATA="/Library/Application Support/Lithium/Monitoring Data/History/lithium"
  SCRIPTPATH="/Library/Lithium/LithiumCore.app/Contents/Resources"
else
  LITHIUMDATA="/usr/var/lithium"
  SCRIPTPATH="/usr/share"
fi

if [ -e "$LITHIUMDATA" ] ; then

  cd "$LITHIUMDATA"

  for i in customer-*
  do

    if [ ! -d "$i" ] ; then 
      continue 
    fi
  
    echo "Processing $i"

    cd $i
    mkdir -p action_scripts
    cp $SCRIPTPATH/action_scripts/* action_scripts
    mkdir -p service_scripts
    cp $SCRIPTPATH/service_scripts/* service_scripts
    cd ..

  done

fi

