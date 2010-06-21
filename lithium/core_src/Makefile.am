CFLAGS = @CFLAGS@
LIBS = @LIBS@

moduledir = $(libdir)/lithium
module_PROGRAMS = core.so

core_so_SOURCES = main.c \
                  resource_message.c \
                  spawn.c \
                  incoming.c \
                  route.c \
                  resource_spawn.c \
                  resource_terminate.c \
                  resource_destroy.c \
                  resource_xml_list.c \
                  fork_clean_up.c \
                  heartbeat.c

noinst_HEADERS = incoming.h \
                 main.h \
                 resource_destroy.h \
                 resource_message.h \
                 resource_spawn.h \
                 resource_terminate.h \
                 route.h \
                 heartbeat.h \
                 spawn.h
