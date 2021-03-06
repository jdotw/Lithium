echo -n "Running 'glibtoolize' ... "
glibtoolize --force
echo "Done"
echo -n "Running 'aclocal' ... "
aclocal
echo "Done"
echo -n "Running 'autoconf' ... "
autoconf
echo "Done"
echo -n "Running 'automake' ... "
automake -a
echo "Done"

PREFIX=/Library/Lithium/LithiumCore.app
FWPREFIX=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0
DBFWPREFIX=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumDatabase.framework/Versions/5.0

CONFIGURE_PATHS="--prefix=$PREFIX --bindir=$PREFIX/Contents/MacOS --sbindir=$PREFIX/Contents/MacOS --libexecdir=$PREFIX/Contents/MacOS --datadir=$PREFIX/Resources --sysconfdir=/Library/Preferences/Lithium --sharedstatedir=$PREFIX/Resources --libdir=$FWPREFIX/Libraries --includedir=$FWPREFIX/Headers --oldincludedir=$FWPREFIX/Headers --infodir=$PREFIX/Resources --mandir=$PREFIX/Resources"

CFLAGS="-O -gdwarf-2 -W -Wall -Wno-unused-parameter -Werror -mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.5.sdk -arch i386 -arch ppc" \
CC="/Developer/usr/bin/clang" \
LDFLAGS="-Wl,-syslibroot,/Developer/SDKs/MacOSX10.5.sdk" \
./configure $CONFIGURE_PATHS --localstatedir='/Library/Application Support/Lithium/Monitoring Data/History' \
--with-db-includes=$FWPREFIX/Headers --with-db-libs=$FWPREFIX/Libraries \
--with-induction-includes=$FWPREFIX/Headers --with-induction-libs=$FWPREFIX/Libraries \
--with-lithium-includes=$FWPREFIX/Headers \
--with-crypt=crypto --with-xml-includes=/usr/include/libxml2 \
--with-xml-libs=/usr/lib --with-postgres-includes=$DBFWPREFIX/Headers \
--with-postgres-libs=$DBFWPREFIX/Libraries

