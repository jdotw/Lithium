# This is a sample spec file for wget

%define _topdir	 	/usr/local/src/lithium/rpm-build-induction
%define name		induction
%define release		1211
%define version 	5.0.12
%define buildroot %{_topdir}/%{name}-%{version}-root

BuildRoot:	%{buildroot}
Summary: 		Lithium Induction Library
License: 		Commercial
Name: 			%{name}
Version: 		%{version}
Release: 		%{release}
Source: 		%{name}-%{version}.tar.gz
Prefix: 		/usr
Group: 			Network/Utilities

%description
The induction library for Lithium

%prep
%setup -q

%build
./configure
make

%install
make install prefix=$RPM_BUILD_ROOT/usr

%files
%defattr(-,root,root)
/usr/lib
/usr/include/induction.h
/usr/include/induction

