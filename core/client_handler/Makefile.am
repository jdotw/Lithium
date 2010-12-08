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

client_handler_so_DEPENDENCIES = ../induction/libinduction.la
client_handler_so_LDADD = ../induction/libinduction.la
client_handler_so_LDFLAGS = @MODULE_LDFLAGS@

