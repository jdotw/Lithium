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

core_so_DEPENDENCIES = ../induction/libinduction.la
core_so_LDADD = ../induction/libinduction.la
core_so_LDFLAGS = -shared -bundle -flat_namespace -undefined suppress

