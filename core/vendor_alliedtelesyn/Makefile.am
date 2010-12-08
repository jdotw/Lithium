moduledir = $(libdir)/lithium/vendor_modules
module_PROGRAMS = alliedtelesyn.so

alliedtelesyn_so_SOURCES = main.c \
						   atcpu.c \
						   atcpu_cntform.c \
						   atcpu_item.c \
						   atcpu_objform.c \
						   fanps.c \
						   fanps_cntform.c \
						   fanps_item.c \
						   fanps_objform.c \
						   snmp_iface.c \
						   snmp_iface_cntform.c \
						   snmp_iface_objform.c \
						   snmp_iface_objfact.c

alliedtelesyn_so_DEPENDENCIES = ../induction/libinduction.la
alliedtelesyn_so_LDADD = ../induction/libinduction.la
alliedtelesyn_so_LDFLAGS = @MODULE_LDFLAGS@

