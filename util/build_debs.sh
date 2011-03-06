rm *.deb

cd induction
debuild clean
debuild binary
cd ..

sudo dpkg -i libinduction5_*deb
sudo dpkg -i libinduction5-dev*deb

cd core
debuild clean
debuild binary
cd ..

VMSTUDIO=10.15.1.17

ssh root@$VMSTUDIO rm /opt/vmware/www/ISV/libinduction5/*
ssh root@$VMSTUDIO rm /opt/vmware/www/ISV/libinduction5-dev/*
ssh root@$VMSTUDIO rm /opt/vmware/www/ISV/lithiumcore/*

scp libinduction5_*deb root@$VMSTUDIO:/opt/vmware/www/ISV/libinduction5
scp libinduction5-dev_*deb root@$VMSTUDIO:/opt/vmware/www/ISV/libinduction5-dev
scp lithiumcore_*deb root@$VMSTUDIO:/opt/vmware/www/ISV/lithiumcore

rm *deb
