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
  PREFIX="--prefix=/opt/local --with-db-includes=/opt/local/include --with-db-libs=/opt/local/lib --with-induction-includes=/opt/local/include --with-induction-libs=/opt/local/lib --with-crypt=crypto --with-xml-includes=/opt/local/include/libxml2 --with-xml-libs=/opt/local/lib"
else
  echo "Setting localstatedir=/var and sysconfdir=/etc"
  PREFIX="--localstatedir=/var --sysconfdir=/etc --with-induction-includes=/usr/local/include --with-induction-libs=/usr/local/lib"
fi

ARGS="$PREFIX"
echo "Running './configure $ARGS'"
./configure $ARGS
