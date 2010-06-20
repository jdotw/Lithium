#!/bin/bash

# Repairs web links

if [ -e '/Library/Application Support/Lithium/ClientService/Resources/htdocs' ] ; then

  cd '/Library/Application Support/Lithium/ClientService/Resources/htdocs/'

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
    ln -sf '/Library/Application Support/Lithium/ClientService/Resources/htdocs/default/admin' .
    ln -sf '/Library/Application Support/Lithium/ClientService/Resources/htdocs/default/console.php' .
    ln -sf '/Library/Application Support/Lithium/ClientService/Resources/htdocs/default/index.php' .
    ln -sf '/Library/Application Support/Lithium/ClientService/Resources/htdocs/default/script.php' .
    ln -sf '/Library/Application Support/Lithium/ClientService/Resources/htdocs/default/xml.php' .
    ln -sf '/Library/Application Support/Lithium/ClientService/Resources/htdocs/default/diag_res.php' .
    ln -sf '/Library/Application Support/Lithium/ClientService/Resources/htdocs/default/diag_auth.php' .
    mkdir cache
    mkdir image_cache
    chmod 777 cache
    chmod 777 image_cache
    cd ..

  done

fi

