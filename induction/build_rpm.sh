#!/bin/sh

BASEDIR=$PWD

#
# Get Versions
#

cd ../core_admin

BUILDNUM=`cat Info.plist | grep -A1 CFBundleShortVersionString | grep string | sed 's/\s*<string>\(.*\)<\/string>/\1/g'`
BUILDNUM_SHORT=`cat Info.plist | grep -A1 CFBundleVersion | grep string | sed 's/\s*<string>\(.*\)<\/string>/\1/g'`

cd $BASEDIR

# Create build paths

BUILDDIR="$BASEDIR/rpm-build"
rm -rf "$BUILDDIR"
mkdir "$BUILDDIR"
cd "$BUILDDIR"

mkdir BUILD RPMS SOURCES SPECS SRPMS

# Create source tarball

cd "$BASEDIR"
sudo make distclean
cd ..

cd "$BUILDDIR"
rm -rf "SOURCES-STAGE"
mkdir "SOURCES-STAGE"
cd "SOURCES-STAGE"
cp -r "$BASEDIR/../induction" .
mv induction "induction-$BUILDNUM"
tar zcvf "$BUILDDIR/SOURCES/induction-$BUILDNUM.tar.gz" "induction-$BUILDNUM"
cd "$BASEDIR"
rm -rf "SOURCES-STAGE"

# Create spec file

SPECFILE="$BUILDDIR/SPECS/induction.spec"
REGEXP='s/\//\\\//g'
ESCBASEDIR=`echo "$BUILDDIR" | sed $REGEXP`
echo "BASE: $ESCBASEDIR"
cp "$BASEDIR/induction.spec" $SPECFILE
sed -i '' -e "s/BASEDIR/$ESCBASEDIR/g" $SPECFILE
sed -i '' -e "s/BUILDNUM_SHORT/$BUILDNUM_SHORT/g" $SPECFILE
sed -i '' -e "s/BUILDNUM/$BUILDNUM/g" $SPECFILE

# Build RPM
cd $BUILDDIR
rpmbuild -v -bb --clean SPECS/induction.spec

# Clean up
rm -rf "$BUILDDIR/induction-$BUILDNUM-root"
