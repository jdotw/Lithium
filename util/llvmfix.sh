#! /bin/sh

echo "=============================================================="
for i in vendor_*
do
  if test -d $i; then
    cd $i
    sed -e "s/llvm-gcc/clang/g" -i '' local_osxstage.sh
    echo "=============================================================="
    cd ..
  fi
done

