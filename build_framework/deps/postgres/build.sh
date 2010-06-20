#!/bin/sh

BASEDIR=$PWD

POSTGRES=postgresql-8.3.3
SDK=/Developer/SDKs/MacOSX10.5.sdk
OSTARGET=10.5
ARCHTARGET="-arch i386 -arch ppc"

MAKEFLAGS=-j`sysctl -n hw.ncpu`

PREFIX=/Library/Lithium/LithiumCore.app
FWPREFIX=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumDatabase.framework/Versions/5.0

CONFIGURE_PATHS="--prefix=$PREFIX --bindir=$PREFIX/Contents/MacOS --sbindir=$PREFIX/Contents/MaOS --libexecdir=$PREFIX/Contents/MacOS --datadir=$PREFIX/Contents/Resources --sysconfdir=$PREFIX/Contents/Resources --sharedstatedir=$PREFIX/Contents/Resources --localstatedir=$PREFIX/Contents/Resources --libdir=$FWPREFIX/Libraries --includedir=$FWPREFIX/Headers --oldincludedir=$FWPREFIX/Headers --infodir=$PREFIX/Contents/Resources --mandir=$PREFIX/Contents/Resources"

#
# Setup environment
#

if [ $UID -ne 0 ]; then
  echo "ERROR: Build script must be run as root"
  exit 1
fi

cd $BASEDIR
rm -rf build
mkdir build
cd build

# 
# Unpack and Configure
#


tar jxvf $BASEDIR/$POSTGRES.tar.bz2
cd $POSTGRES

./configure $CONFIGURE_PATHS --with-pgport=51132 --disable-dependency-tracking \
CFLAGS="$ARCHTARGET -mmacosx-version-min=$OSTARGET -isysroot $SDK -Wl,-search_paths_first -O" \
LDFLAGS="$ARCHTARGET -lz -liconv -ltcl" \
LD="gcc -mmacosx-version-min=$OSTARGET -isysroot $SDK -nostartfiles $ARCHTARGET" \
MACOSX_DEPLOYMENT_TARGET=$OSTARGET
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to configure PostgreSQL"
  exit 1
fi

#
# Fix PostgreSQL Make files to allow Universal Binary build
# 

#for i in `grep -R '$(LD) $(LDREL) $(LDOUT) SUBSYS.o $(OBJS)' * | sed s/:.*//g | grep -v '.orig'`
#do
#  echo "Fixing $i"
#  sed -e 's/$(LD) $(LDREL) $(LDOUT) SUBSYS.o $(OBJS)/$(LD) $(LDREL) -arch i386 $(LDOUT) SUBSYS-i386.o $(OBJS); $(LD) $(LDREL) -arch ppc $(LDOUT) SUBSYS-ppc.o $(OBJS); lipo -arch i386 SUBSYS-i386.o -arch ppc SUBSYS-ppc.o -create -output SUBSYS.o/g' -i '.orig' $i
#done

#for i in `grep -R '$(LD) $(LDREL) $(LDOUT) $@ $^' * | sed s/:.*//g | grep -v '.orig'`
#do
#  echo "Fixing $i"
#  sed -e 's/$(LD) $(LDREL) $(LDOUT) $@ $^/$(LD) $(LDREL) -arch i386 $(LDOUT) SUBSYS-i386.o $^; $(LD) $(LDREL) -arch ppc $(LDOUT) SUBSYS-ppc.o $^; lipo -arch i386 SUBSYS-i386.o -arch ppc SUBSYS-ppc.o -create -output $@/g' -i '.orig' $i
#done

#for i in `grep -R '$(LD) $(LDREL) $(LDOUT) SUBSYS.o $^' * | sed s/:.*//g | grep -v '.orig'`
#do
#  echo "Fixing $i"
#  sed -e 's/$(LD) $(LDREL) $(LDOUT) SUBSYS.o $^/$(LD) $(LDREL) -arch i386 $(LDOUT) SUBSYS-i386.o $^; $(LD) $(LDREL) -arch ppc $(LDOUT) SUBSYS-ppc.o $^; lipo -arch i386 SUBSYS-i386.o -arch ppc SUBSYS-ppc.o -create -output SUBSYS.o/g' -i '.orig' $i
#done

# 
# Compile and Install
#

make $MAKEFLAGS
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to compile PostgreSQL"
  exit 1
fi

make install
if [ $? -ne 0 ]; then
  echo "ERROR: Failed to install PostgreSQL"
  exit 1
fi

#
# Fix executable names
#

cd /Library/Lithium/LithiumCore.app/Contents/MacOS
sudo mv postgres lcpostgres
sudo rm postmaster
sudo ln -sf lcpostgres postmaster
sudo ln -sf lcpostgres postgres
sudo ln -sf lcpostgres lcpostmaster

# 
# Copy configs
#

cd $BASEDIR
cd config
mkdir -p $PREFIX/Contents/Resources/DefaultPreferences/Database
cp * $PREFIX/Contents/Resources/DefaultPreferences/Database


#
# Cleanup phase
#

cd $BASEDIR
rm -rf build

echo "[OK] Universal Binary build of PostgreSQL completed successfully"
