moduledir = $(libdir)/lithium/vendor_modules
module_PROGRAMS = hp.so

hp_so_SOURCES = main.c \
                   mempool.c \
                   mempool_item.c \
                   mempool_objfact.c \
                   mempool_cntform.c \
                   mempool_objform.c \
				   sensor.c \
				   sensor_item.c \
				   sensor_objfact.c \
				   sensor_cntform.c \
				   sensor_objform.c \
				   cpu.c \
                   cpu_item.c \
                   cpu_objfact.c \
                   cpu_cntform.c \
                   cpu_objform.c

hp_so_DEPENDENCIES = ../induction/libinduction.la
hp_so_LDADD = ../induction/libinduction.la
hp_so_LDFLAGS = @MODULE_LDFLAGS@

