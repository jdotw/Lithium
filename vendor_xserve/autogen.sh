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

if test -x /opt/local; then
  echo "Setting prefix to /opt/local"
  PREFIX="--prefix=/opt/local --with-induction-includes=/opt/local/include --with-lithium-includes=/opt/local/include --with-xml-includes=/opt/local/include/libxml2 --with-xml-libs=/opt/local/lib"
else
  echo "Using default prefix"
  PREFIX="--with-induction-includes=/usr/local/include --with-lithium-includes=/usr/local/include"
fi

ARGS="$PREFIX"
echo "Running './configure $ARGS'"
./configure $ARGS
