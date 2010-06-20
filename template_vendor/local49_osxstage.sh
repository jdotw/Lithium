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

PREFIX=/Lithium

CONFIGURE_PATHS="--prefix=$PREFIX"

./configure $CONFIGURE_PATHS --with-db-includes=$PREFIX/include \
--with-db-libs=$PREFIX \
--with-induction-includes=$PREFIX/include \
--with-induction-libs=$PREFIX \
--with-lithium-includes=$PREFIX/include \
--with-crypt=crypto --with-xml-includes=/usr/include/libxml2 \
--with-xml-libs=/usr/lib --with-postgres-includes=$PREFIX/include \
--with-postgres-libs=$PREFIX


