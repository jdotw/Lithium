moduledir = $(libdir)/lithium/vendor_modules
module_PROGRAMS = airport.so

airport_so_SOURCES = main.c \
					 sysinfo_refcb.c \
					 client.c \
					 client_objfact.c \
					 client_cntform.c \
					 client_objform.c \
					 client_item.c \
					 clientcount.c \
					 clientcount_item.c \
					 clientcount_refcb.c \
					 clientcount_cntform.c \
					 clientcount_objform.c \
					 physical.c \
					 physical_objfact.c \
					 physical_cntform.c \
					 physical_objform.c \
					 physical_item.c

airport_so_DEPENDENCIES = ../induction/libinduction.la
airport_so_LDFLAGS = @MODULE_LDFLAGS@

