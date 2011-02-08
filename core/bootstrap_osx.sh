echo -n "Running 'glibtoolize' ... "
/Xcode3/usr/bin/glibtoolize --force
echo "Done"
echo -n "Running 'aclocal' ... "
/Xcode3/usr/bin/aclocal
echo "Done"
echo -n "Running 'autoconf' ... "
/Xcode3/usr/bin/autoconf
echo "Done"
echo -n "Running 'automake' ... "
/Xcode3/usr/bin/automake -a
echo "Done"

PREFIX=/Library/Lithium/LithiumCore.app
FWPREFIX=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0
DBFWPREFIX=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumDatabase.framework/Versions/5.0

CONFIGURE_PATHS="--prefix=$PREFIX --bindir=$PREFIX/Contents/MacOS --sbindir=$PREFIX/Contents/MacOS --libexecdir=$PREFIX/Contents/MacOS --datadir=$PREFIX/Contents/Resources --sysconfdir=/Library/Preferences/Lithium --sharedstatedir=$PREFIX/Contents/Resources --libdir=$FWPREFIX/Libraries --includedir=$FWPREFIX/Headers --oldincludedir=$FWPREFIX/Headers --infodir=$PREFIX/Contents/Resources --mandir=$PREFIX/Contents/Resources"

ARGS="$CONFIGURE_PATHS"
echo "Running './configure $ARGS'"
PATH="/Xcode3/usr/bin:/Xcode3/usr/sbin:$PATH:/Library/Lithium/LithiumCore.app/Contents/MacOS" \
CC="/Xcode3/usr/bin/clang" \
CFLAGS="-mmacosx-version-min=10.5 -isysroot /Xcode3/SDKs/MacOSX10.5.sdk -arch i386 -I/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0/Headers" \
LDFLAGS="-Wl,-syslibroot,/Xcode3/SDKs/MacOSX10.5.sdk -L/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0/Libraries" \
./configure $ARGS --localstatedir='/Library/Application Support/Lithium/Monitoring Data/History'
