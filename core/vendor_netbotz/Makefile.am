moduledir = $(libdir)/lithium/vendor_modules
module_PROGRAMS = netbotz.so

netbotz_so_SOURCES = main.c	\
					 envmon.c	\
					 envmon_item.c	\
					 envmon_cntform.c \
					 envmon_objform.c \
					 envmon_campic.c \
					 envmon_campicform.c

netbotz_so_DEPENDENCIES = ../induction/libinduction.la
netbotz_so_LDFLAGS = @MODULE_LDFLAGS@

