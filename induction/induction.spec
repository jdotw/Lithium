# This is a sample spec file for wget

%define _topdir	 	BASEDIR
%define name		induction
%define release		BUILDNUM_SHORT
%define version 	BUILDNUM
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

