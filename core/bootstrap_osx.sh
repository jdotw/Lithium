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

ARGS="--disable-dependency-tracking"
echo "Running './configure $ARGS'"
PATH="$PATH:/Library/Lithium/LithiumCore.app/Contents/MacOS" \
CFLAGS="-mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.5.sdk -arch i386 -arch ppc -I/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Headers" \
LDFLAGS="-Wl,-syslibroot,/Developer/SDKs/MacOSX10.5.sdk -L/Library/Lithium/LithiumCore.app/Contents/Frameworks/LithiumCore.framework/Libraries" \
./configure $ARGS
