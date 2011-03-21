#!/bin/sh

scp rpm-build-induction/RPMS/i386/* root@10.1.1.19:/opt/vmware/www/ISV/induction
scp rpm-build-lithiumcore/RPMS/i386/* root@10.1.1.19:/opt/vmware/www/ISV/lithiumcore

