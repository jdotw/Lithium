cd induction
debuild clean
debuild binary
cd ..

cd core
debuild clean
debuild binary
cd ..

scp libinduction5_*deb root@10.15.1.17:/opt/vmware/www/ISV/libinduction5
scp libinduction5-dev_*deb root@10.15.1.17:/opt/vmware/www/ISV/libinduction5-dev
scp lithiumcore_*deb root@10.15.1.17:/opt/vmware/www/ISV/lithiumcore

rm *deb
