CFLAGS = @CFLAGS@
LIBS = @LIBS@ 

moduledir = $(libdir)/lithium
module_PROGRAMS = client_handler.so

client_handler_so_SOURCES = main.c \
                            msgproc_core.c \
                            msgproc_child.c \
                            relay.c \
                            incoming_child.c \
                            incoming_client.c \
                            child.c \
							reqtime.c

noinst_HEADERS = child.h \
                 incoming_child.h \
                 incoming_client.h \
                 main.h \
                 msgproc_child.h \
                 msgproc_core.h \
                 relay.h \
				 reqtime.h
