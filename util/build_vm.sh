#!/bin/sh

#
# Get Versions
#

cd core_admin

BUILDNUM=`cat Info.plist | grep -A1 CFBundleShortVersionString | grep string | sed 's/\s*<string>\(.*\)<\/string>/\1/g'`
BUILDNUM_SHORT=`cat Info.plist | grep -A1 CFBundleVersion | grep string | sed 's/\s*<string>\(.*\)<\/string>/\1/g'`

cd $BASEDIR

# Remove old build
ssh root@10.1.1.19 rm -rf "/opt/vmware/www/build/1/exports/*"
ssh root@10.1.1.19 'bash /etc/environment; studiocli -d -i LithiumCent.1'

# Set Profile Version
ssh root@10.1.1.19 sed -i'.backup' -e "s/5\.0\.[0-9][0-9]*\.[0-9][0-9]*/$BUILDNUM.$BUILDNUM_SHORT/g" /opt/vmware/var/lib/build/profiles/LithiumCentOS.xml

# Build
ssh root@10.1.1.19 'bash /etc/environment; studiocli -c -p /opt/vmware/var/lib/build/profiles/LithiumCentOS.xml -i 1'
