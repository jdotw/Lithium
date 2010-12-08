#!/bin/sh

env MIBS="+./lithium.mib" mib2c -i -c mib2c.lithium.conf lcCustomerTable
env MIBS="+./lithium.mib" mib2c -i -c mib2c.lithium.conf lcSiteTable
env MIBS="+./lithium.mib" mib2c -i -c mib2c.lithium.conf lcDeviceTable
env MIBS="+./lithium.mib" mib2c -i -c mib2c.lithium.conf lcContainerTable
env MIBS="+./lithium.mib" mib2c -i -c mib2c.lithium.conf lcObjectTable
env MIBS="+./lithium.mib" mib2c -i -c mib2c.lithium.conf lcMetricTable
env MIBS="+./lithium.mib" mib2c -i -c mib2c.lithium.conf lcTriggerTable
