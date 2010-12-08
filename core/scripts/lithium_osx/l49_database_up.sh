#!/bin/sh

sudo -u lithium kill `cat /Lithium/var/postgres/postmaster.pid | grep ^[0-9]`
sudo -u lithium /Lithium/bin/ipcclean
sudo -u lithium /Lithium/bin/postmaster -D /Lithium/var/postgres -i 2>&1
