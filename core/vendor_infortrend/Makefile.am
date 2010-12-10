moduledir = $(libdir)/lithium/vendor_modules
module_PROGRAMS = infortrend.so

infortrend_so_SOURCES = main.c \
						ldrive.c \
						ldrive_item.c \
						ldrive_objfact.c \
						pdrive.c \
						pdrive_item.c \
						pdrive_objfact.c \
						index_refcb.c \
						info.c \
						info_item.c \
						cache.c \
						cache_item.c

infortrend_so_DEPENDENCIES = ../induction/libinduction.la
infortrend_so_LDFLAGS = @MODULE_LDFLAGS@

