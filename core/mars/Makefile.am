moduledir = $(libdir)/lithium
module_PROGRAMS = mars.so

mars_so_SOURCES = main.c	\
				  socket.c	\
				  status.c	\
				  rrd.c \
				  rrd_proc.c \
				  rrdcached.c

mars_so_LDFLAGS = -shared -bundle -flat_namespace -undefined suppress
