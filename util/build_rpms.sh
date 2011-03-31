#!/bin/sh

BASEDIR=$PWD

#
# Get Versions
#

cd core_admin

BUILDNUM=`cat Info.plist | grep -A1 CFBundleShortVersionString | grep string | sed 's/\s*<string>\(.*\)<\/string>/\1/g'`
BUILDNUM_SHORT=`cat Info.plist | grep -A1 CFBundleVersion | grep string | sed 's/\s*<string>\(.*\)<\/string>/\1/g'`

cd $BASEDIR

# Create build paths

LIBBUILDDIR="$BASEDIR/rpm-build-induction"
COREBUILDDIR="$BASEDIR/rpm-build-lithiumcore"
sudo rm -rf "$LIBBUILDDIR"
sudo rm -rf "$COREBUILDDIR"
mkdir "$LIBBUILDDIR"
mkdir "$COREBUILDDIR"
cd "$LIBBUILDDIR"
mkdir BUILD RPMS SOURCES SPECS SRPMS
cd "$COREBUILDDIR"
mkdir BUILD RPMS SOURCES SPECS SRPMS

#
#
# INDUCTION BUILD
#
#

# Create source tarball

cd "$BASEDIR/induction"
sudo make distclean
sudo rm configure
sudo rm config.*
sudo ./bootstrap_linux.sh
cd ..

cd "$LIBBUILDDIR"
rm -rf "SOURCES-STAGE"
mkdir "SOURCES-STAGE"
cd "SOURCES-STAGE"
cp -r "$BASEDIR/induction" .
mv induction "induction-$BUILDNUM"
tar zcvf "$LIBBUILDDIR/SOURCES/induction-$BUILDNUM.tar.gz" "induction-$BUILDNUM"
cd "$LIBBUILDDIR"
rm -rf "SOURCES-STAGE"

# Create spec file

SPECFILE="$LIBBUILDDIR/SPECS/induction.spec"
REGEXP='s/\//\\\//g'
ESCBASEDIR=`echo "$LIBBUILDDIR" | sed $REGEXP`
echo "BASE: $ESCBASEDIR"
cp "$BASEDIR/induction/induction.spec" $SPECFILE
sed -i '' -e "s/BASEDIR/$ESCBASEDIR/g" $SPECFILE
sed -i '' -e "s/BUILDNUM_SHORT/$BUILDNUM_SHORT/g" $SPECFILE
sed -i '' -e "s/BUILDNUM/$BUILDNUM/g" $SPECFILE

# Build RPM
cd $LIBBUILDDIR
rpmbuild -v -bb --clean SPECS/induction.spec

# Clean up
rm -rf "$LIBBUILDDIR/induction-$BUILDNUM-root"

#
#
# LITHIUM CORE BUILD
#
#

# Install Induction
cd "$BASEDIR/rpm-build-induction/RPMS/i386"
sudo rpm -U *rpm

# Create source tarball

cd "$BASEDIR/core"
sudo make distclean
sudo rm configure
sudo rm config.*
sudo ./bootstrap_linux.sh
cd ..

cd "$COREBUILDDIR"
rm -rf "SOURCES-STAGE"
mkdir "SOURCES-STAGE"
cd "SOURCES-STAGE"
echo `pwd`
cp -r "$BASEDIR/core" .
mv core "lithiumcore-$BUILDNUM"
tar zcvf "$COREBUILDDIR/SOURCES/lithiumcore-$BUILDNUM.tar.gz" "lithiumcore-$BUILDNUM"
cd "$COREBUILDDIR"
rm -rf "SOURCES-STAGE"

# Create spec file

SPECFILE="$COREBUILDDIR/SPECS/lithiumcore.spec"
REGEXP='s/\//\\\//g'
ESCBASEDIR=`echo "$COREBUILDDIR" | sed $REGEXP`
echo "BASE: $ESCBASEDIR"
cp "$BASEDIR/core/lithiumcore.spec" $SPECFILE
sed -i '' -e "s/BASEDIR/$ESCBASEDIR/g" $SPECFILE
sed -i '' -e "s/BUILDNUM_SHORT/$BUILDNUM_SHORT/g" $SPECFILE
sed -i '' -e "s/BUILDNUM/$BUILDNUM/g" $SPECFILE

# Build RPM
cd $COREBUILDDIR
rpmbuild -v -bb --clean SPECS/lithiumcore.spec

# Clean up
rm -rf "$COREBUILDDIR/lithiumcore-$BUILDNUM-root"

