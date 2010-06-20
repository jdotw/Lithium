#! /bin/sh

svn copy svn+ssh://svn.lithiumcorp.com/sql/svn/actuator/trunk svn+ssh://svn.lithiumcorp.com/sql/svn/actuator/tags/release-$1 -m "Tagging release $1"
svn copy svn+ssh://svn.lithiumcorp.com/sql/svn/construct/trunk svn+ssh://svn.lithiumcorp.com/sql/svn/construct/tags/release-$1 -m "Tagging release $1"
svn copy svn+ssh://svn.lithiumcorp.com/sql/svn/induction/trunk svn+ssh://svn.lithiumcorp.com/sql/svn/induction/tags/release-$1 -m "Tagging release $1"
svn copy svn+ssh://svn.lithiumcorp.com/sql/svn/lithium/trunk svn+ssh://svn.lithiumcorp.com/sql/svn/lithium/tags/release-$1 -m "Tagging release $1"
svn copy svn+ssh://svn.lithiumcorp.com/sql/svn/mars/trunk svn+ssh://svn.lithiumcorp.com/sql/svn/mars/tags/release-$1 -m "Tagging release $1"
svn copy svn+ssh://svn.lithiumcorp.com/sql/svn/webmsp/trunk svn+ssh://svn.lithiumcorp.com/sql/svn/webmsp/tags/release-$1 -m "Tagging release $1"
svn copy svn+ssh://svn.lithiumcorp.com/sql/svn/vendor_xraid/trunk svn+ssh://svn.lithiumcorp.com/sql/svn/vendor_xraid/tags/release-$1 -m "Tagging release $1"
svn copy svn+ssh://svn.lithiumcorp.com/sql/svn/vendor_xserve/trunk svn+ssh://svn.lithiumcorp.com/sql/svn/vendor_xserve/tags/release-$1 -m "Tagging release $1"

for i in vendor_*
do
  if test -d $i; then
    svn copy svn+ssh://oss.lithiumcorp.com/www/svn/$i/trunk svn+ssh://oss.lithiumcorp.com/www/svn/$i/tags/release-$1  -m "Tagging release $1"
  fi
done

