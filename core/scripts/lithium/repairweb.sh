#!/bin/bash

# Repairs web links

if [ -e '/Library/Lithium/LithiumCore.app' ] ; then
  HTDOCSPATH="/Library/Application Support/Lithium/ClientService/Resources/htdocs"
else
  HTDOCSPATH="/lithium/share/htdocs"
fi


if [ -e "$HTDOCSPATH" ] ; then

  cd "$HTDOCSPATH"

  for i in *
  do

    if [ ! -d "$i" ] ; then 
      continue 
    fi
    if test "$i" = "default" ; then 
      continue 
    fi
    if test "$i" = "diag" ; then 
      continue 
    fi
    if test "$i" = "images" ; then 
      continue 
    fi
    if test "$i" = "include" ; then 
      continue 
    fi
  
    echo "Processing $i"

    cd $i
    chmod 775 .
    chown lithium:lithium .
    ln -sf "$HTDOCSPATH/default/admin" .
    ln -sf "$HTDOCSPATH/default/console.php" .
    ln -sf "$HTDOCSPATH/default/index.php" .
    ln -sf "$HTDOCSPATH/default/script.php" .
    ln -sf "$HTDOCSPATH/default/xml.php" .
    ln -sf "$HTDOCSPATH/default/diag_res.php" .
    ln -sf "$HTDOCSPATH/default/diag_auth.php" .
    mkdir cache
    mkdir image_cache
    chmod 777 cache
    chmod 777 image_cache
    cd ..

  done

fi

