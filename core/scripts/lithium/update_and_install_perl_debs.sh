#!/bin/sh

echo "Updating APT Cache and installing required Perl modules"
apt-get update
apt-get install libxml-simple-perl libtime-modules-perl libdata-types-perl libnet-dns-perl libnet-imap-simple-ssl-perl

