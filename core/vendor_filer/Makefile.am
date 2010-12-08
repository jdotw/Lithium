moduledir = $(libdir)/lithium/vendor_modules
module_PROGRAMS = filer.so

filer_so_SOURCES = main.c \
				   raid.c \
				   raid_item.c \
				   raid_objfact.c \
				   raid_cntform.c \
				   raid_objform.c \
				   filesys.c \
				   filesys_item.c \
				   filesys_objfact.c \
				   filesys_cntform.c \
				   filesys_objform.c \
				   cpu.c \
				   cpu_item.c \
				   cpu_cntform.c \
				   cpu_objform.c

filer_so_DEPENDENCIES = ../induction/libinduction.la
filer_so_LDADD = ../induction/libinduction.la
filer_so_LDFLAGS = @MODULE_LDFLAGS@

