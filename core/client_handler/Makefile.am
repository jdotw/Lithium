moduledir = $(libdir)/lithium
pkglib_LTLIBRARIES = client_handler.la

client_handler_la_SOURCES = main.c \
                            msgproc_core.c \
                            msgproc_child.c \
                            relay.c \
                            incoming_child.c \
                            incoming_client.c \
                            child.c \
			    reqtime.c

client_handler_la_LDFLAGS = -module @MODULE_LDFLAGS@

