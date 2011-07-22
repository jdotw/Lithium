#!/bin/bash

BASEDIR=$PWD

PREFIX=/Library/Lithium

if [ $UID -ne 0 ]; then
  echo "ERROR: Must be root to run build script"
  exit 1
fi

./build_deps.sh
./build_lithium.sh
