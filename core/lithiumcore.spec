# This is a sample spec file for wget

%define _topdir	 	BASEDIR
%define name		lithiumcore
%define release		BUILDNUM_SHORT
%define version 	BUILDNUM
%define buildroot %{_topdir}/%{name}-%{version}-root

BuildRoot:	%{buildroot}
Summary: 		Lithium Core
License: 		Commercial
Name: 			%{name}
Version: 		%{version}
Release: 		%{release}
Source: 		%{name}-%{version}.tar.gz
Prefix: 		/usr
Group: 			Network/Utilities
AutoReqProv: no

%description
Lithium Core network, server and storage monitoring daemon

%prep
%setup -q

%build
./configure --prefix=/usr
make

%install
mkdir -p $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/usr/share/conf
cp ../../../linux/lcrrd* $RPM_BUILD_ROOT/usr/bin
cp -r ../../../linux/ruby/* $RPM_BUILD_ROOT
cp -r ../../../linux/perl/* $RPM_BUILD_ROOT
cp ../../../linux/centos/lithiumcore.init $RPM_BUILD_ROOT/usr/bin/lithiumcore.init
cp ../../../linux/centos/start_lithium.sh $RPM_BUILD_ROOT/usr/bin/start_lithium.sh
cp ../../../linux/centos/httpd.conf $RPM_BUILD_ROOT/usr/share/conf/httpd.conf
cp ../../../linux/centos/ssl.conf $RPM_BUILD_ROOT/usr/share/conf/ssl.conf
cp ../../../linux/centos/node.conf $RPM_BUILD_ROOT/usr/share/conf/node.conf.linux
cp ../../../linux/centos/banner $RPM_BUILD_ROOT/usr/share/conf/banner
cp ../../../linux/centos/snmpd.options $RPM_BUILD_ROOT/usr/share/conf/snmpd.options
cp ../../../linux/centos/php.ini $RPM_BUILD_ROOT/usr/share/conf/php.ini
chmod ug+x $RPM_BUILD_ROOT/usr/bin/start_lithium.sh
make install prefix=$RPM_BUILD_ROOT/usr

%files
%defattr(-,root,root)
/usr/bin
/usr/lib
/usr/share
/usr/include
/var
/usr/local

%post
if [ -e /etc/init.d/lithiumcore ]; then
  /etc/init.d/lithiumcore restart
fi
