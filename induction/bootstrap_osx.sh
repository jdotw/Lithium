if test -x /usr/bin/libtoolize; then
  echo -n "Running 'libtoolize' ... "
  libtoolize --force
  echo "Done"
else
  if test -x /usr/bin/glibtoolize; then
    echo -n "Running 'glibtoolize' ... "
    glibtoolize --force
    echo "Done"
  fi
fi
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

CONFIGURE_PATHS="--prefix=$PREFIX --bindir=$PREFIX/Contents/MacOS --sbindir=$PREFIX/Contents/MacOS --libexecdir=$PREFIX/Contents/MacOS --datadir=$PREFIX/Contents/Resources --sysconfdir=/Library/Preferences/Lithium --sharedstatedir=$PREFIX/Contents/Resources --libdir=$FWPREFIX/Libraries --includedir=$FWPREFIX/Headers --oldincludedir=$FWPREFIX/Headers --infodir=$PREFIX/Contents/Resources --mandir=$PREFIX/Contents/Resources"

ARGS="--disable-dependency-tracking $CONFIGURE_PATHS"
echo "Running './configure $ARGS'"
PATH="$PATH:/Library/Lithium/LithiumCore.app/Contents/MacOS" \
CFLAGS="-mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.5.sdk -arch i386 -arch ppc -I/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0/Headers" \
LDFLAGS="-Wl,-syslibroot,/Developer/SDKs/MacOSX10.5.sdk -L/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0/Libraries" \
./configure $ARGS --localstatedir='/Library/Application Support/Lithium/Monitoring Data/History'
