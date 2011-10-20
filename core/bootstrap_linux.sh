make distclean
rm configure
rm config.*
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

./configure --build i486-linux-gnu --prefix=/usr --localstatedir=/lithium/var --datadir=/lithium/share --sysconfdir=/lithium/etc --mandir=${prefix}/share/man --infodir=${prefix}/share/info
