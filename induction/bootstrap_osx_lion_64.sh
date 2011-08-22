echo -n "Running 'glibtoolize' ... "
/Developer/usr/bin/glibtoolize --force
echo "Done"
echo -n "Running 'aclocal' ... "
/Developer/usr/bin/aclocal
echo "Done"
echo -n "Running 'autoconf' ... "
/Developer/usr/bin/autoconf
echo "Done"
echo -n "Running 'automake' ... "
/Developer/usr/bin/automake -a
echo "Done"

PREFIX=/Library/Lithium/LithiumCore.app
FWPREFIX=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0
DBFWPREFIX=/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumDatabase64.framework/Versions/5.0

CONFIGURE_PATHS="--prefix=$PREFIX --bindir=$PREFIX/Contents/MacOS --sbindir=$PREFIX/Contents/MacOS --libexecdir=$PREFIX/Contents/MacOS --datadir=$PREFIX/Contents/Resources --sysconfdir=/Library/Preferences/Lithium --sharedstatedir=$PREFIX/Contents/Resources --libdir=$FWPREFIX/Libraries --includedir=$FWPREFIX/Headers --oldincludedir=$FWPREFIX/Headers --infodir=$PREFIX/Contents/Resources --mandir=$PREFIX/Contents/Resources"

ARGS="$CONFIGURE_PATHS"
echo "Running './configure $ARGS'"
PATH="/Developer/usr/bin:/Developer/usr/sbin:/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumDatabase64.framework/Versions/5.0/Contents/MacOS:/Library/Lithium/LithiumCore.app/Contents/MacOS:$PATH" \
CFLAGS="-I/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0/Headers" \
LDFLAGS="-L/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Versions/5.0/Libraries" \
./configure $ARGS --localstatedir='/Library/Application Support/Lithium/Monitoring Data/History'
