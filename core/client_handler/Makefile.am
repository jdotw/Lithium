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

client_handler_so_LDFLAGS = -shared -bundle -flat_namespace -undefined suppress

