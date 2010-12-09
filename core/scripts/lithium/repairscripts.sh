#!/bin/bash

# Repairs web links

if [ -e '/Library/Application Support/Lithium/Monitoring Data/History/lithium' ] ; then

  cd '/Library/Application Support/Lithium/Monitoring Data/History/lithium'

  for i in customer-*
  do

    if [ ! -d "$i" ] ; then 
      continue 
    fi
  
    echo "Processing $i"

    cd $i
    mkdir -p action_scripts
    cp /Library/Lithium/LithiumCore.app/Contents/Resources/action_scripts/* action_scripts
    mkdir -p service_scripts
    cp /Library/Lithium/LithiumCore.app/Contents/Resources/service_scripts/* service_scripts
    cd ..

  done

fi

