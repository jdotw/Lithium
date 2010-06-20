#!/bin/sh
for i in `grep -R '$(LD) $(LDREL) $(LDOUT) SUBSYS.o $(OBJS)' * | sed s/:.*//g | grep -v '.orig'`
do
  echo "Fixing $i"
  sed -e 's/$(LD) $(LDREL) $(LDOUT) SUBSYS.o $(OBJS)/$(LD) $(LDREL) -arch i386 $(LDOUT) SUBSYS-i386.o $(OBJS); $(LD) $(LDREL) -arch ppc $(LDOUT) SUBSYS-ppc.o $(OBJS); lipo -arch i386 SUBSYS-i386.o -arch ppc SUBSYS-ppc.o -create -output SUBSYS.o/g' -i '.orig' $i
done

for i in `grep -R '$(LD) $(LDREL) $(LDOUT) $@ $^' * | sed s/:.*//g | grep -v '.orig'`
do
  echo "Fixing $i"
  sed -e 's/$(LD) $(LDREL) $(LDOUT) $@ $^/$(LD) $(LDREL) -arch i386 $(LDOUT) SUBSYS-i386.o $^; $(LD) $(LDREL) -arch ppc $(LDOUT) SUBSYS-ppc.o $^; lipo -arch i386 SUBSYS-i386.o -arch ppc SUBSYS-ppc.o -create -output $@/g' -i '.orig' $i
done

for i in `grep -R '$(LD) $(LDREL) $(LDOUT) SUBSYS.o $^' * | sed s/:.*//g | grep -v '.orig'`
do
  echo "Fixing $i"
  sed -e 's/$(LD) $(LDREL) $(LDOUT) SUBSYS.o $^/$(LD) $(LDREL) -arch i386 $(LDOUT) SUBSYS-i386.o $^; $(LD) $(LDREL) -arch ppc $(LDOUT) SUBSYS-ppc.o $^; lipo -arch i386 SUBSYS-i386.o -arch ppc SUBSYS-ppc.o -create -output SUBSYS.o/g' -i '.orig' $i
done

