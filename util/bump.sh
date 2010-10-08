#!/bin/sh

cd core_admin
agvtool bump -all
cd ..
cd console
agvtool bump -all
cd ..
