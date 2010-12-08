moduledir = $(libdir)/lithium/vendor_modules
module_PROGRAMS = netsnmp.so

netsnmp_so_SOURCES = main.c

netsnmp_so_DEPENDENCIES = ../induction/libinduction.la
netsnmp_so_LDADD = ../induction/libinduction.la
netsnmp_so_LDFLAGS = @MODULE_LDFLAGS@

