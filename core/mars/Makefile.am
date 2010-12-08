moduledir = $(libdir)/lithium
module_PROGRAMS = mars.so

mars_so_SOURCES = main.c	\
				  socket.c	\
				  status.c	\
				  rrd.c \
				  rrd_proc.c \
				  rrdcached.c

mars_so_DEPENDENCIES = ../induction/libinduction.la
mars_so_LDADD = ../induction/libinduction.la
mars_so_LDFLAGS = -shared -bundle
