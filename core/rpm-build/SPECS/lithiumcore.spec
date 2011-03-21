# This is a sample spec file for wget

%define _topdir	 	/usr/local/src/lithium/core/rpm-build
%define name		lithiumcore
%define release		1211
%define version 	5.0.12
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

%description
Lithium Core network, server and storage monitoring daemon

%prep
%setup -q

%build
./configure
make

%install
mkdir -p $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin
cp ../../../../linux/lcrrd* $RPM_BUILD_ROOT/usr/bin
cp -r ../../../../linux/ruby/* $RPM_BUILD_ROOT
cp -r ../../../../linux/perl/* $RPM_BUILD_ROOT
make install prefix=$RPM_BUILD_ROOT/usr

%files
%defattr(-,root,root)
/usr/bin
/usr/lib
/usr/share
/usr/include
/var
/usr/local
