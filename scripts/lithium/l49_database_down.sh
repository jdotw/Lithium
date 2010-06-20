#!/bin/sh

sudo -u lithium kill `cat /Lithium/var/postgres/postmaster.pid | grep ^[0-9]`
sudo -u lithium /Lithium/bin/ipcclean
echo "OK"
