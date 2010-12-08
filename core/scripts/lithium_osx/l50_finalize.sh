#!/bin/bash

# Copy Config
mkdir -p /Library/Preferences/Lithium/lithium
cp /Lithium/etc/lithium/node.conf /Library/Preferences/Lithium/lithium/node.conf

# Touch config and make it writeable by lithium
touch /Library/Preferences/Lithium/lithium/node.conf
chown lithium:admin /Library/Preferences/Lithium/lithium/node.conf
chmod ug+rw /Library/Preferences/Lithium/lithium/node.conf
chmod o-r /Library/Preferences/Lithium/lithium/node.conf

# Fix port number in node.conf
sed -e 's/port "5432"/port "51132"/g' -i '' /Library/Preferences/Lithium/lithium/node.conf
sed -e 's/host "localhost"/host "\/tmp"/g' -i '' /Library/Preferences/Lithium/lithium/node.conf
sed -e 's/imageroot "\/Lithium\/WebServer\/Documents"/imageroot "\/Library\/Application Support\/Lithium\/ClientService\/Resources\/htdocs"/g' -i '' /Library/Preferences/Lithium/lithium/node.conf


# Touch profile
touch '/Library/Application Support/Lithium/ClientService/Resources/htdocs/default/profile.php'

echo OK
