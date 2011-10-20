rm *.deb

cd induction
debuild clean
rm -rf debian/tmp
debuild binary
cd ..

sudo dpkg -i libinduction5_*deb
sudo dpkg -i libinduction5-dev*deb

cd core
debuild clean
rm -rf debian/tmp
debuild binary
cd ..

VMSTUDIO=10.1.1.21

ssh root@$VMSTUDIO mkdir -p /opt/vmware/www/ISV/libinduction5
ssh root@$VMSTUDIO mkdir -p /opt/vmware/www/ISV/libinduction5-dev
ssh root@$VMSTUDIO mkdir -p /opt/vmware/www/ISV/lithiumcore

ssh root@$VMSTUDIO rm /opt/vmware/www/ISV/libinduction5/*
ssh root@$VMSTUDIO rm /opt/vmware/www/ISV/libinduction5-dev/*
ssh root@$VMSTUDIO rm /opt/vmware/www/ISV/lithiumcore/*

scp linux/debian/debs/*.deb root@$VMSTUDIO:/opt/vmware/www/ISV/appliancePackages
scp libinduction5_*deb root@$VMSTUDIO:/opt/vmware/www/ISV/libinduction5
scp libinduction5-dev_*deb root@$VMSTUDIO:/opt/vmware/www/ISV/libinduction5-dev
scp lithiumcore_*deb root@$VMSTUDIO:/opt/vmware/www/ISV/lithiumcore
