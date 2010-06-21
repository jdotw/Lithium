CFLAGS = @CFLAGS@
LIBS = @LIBS@

moduledir = $(libdir)/lithium
module_PROGRAMS = sms.so

sms_so_SOURCES = main.c \
                 gsmdevice.c \
                 gsmdevice_socket.c \
                 form_resource_config.c \
                 sms_config.c \
                 atcommand.c \
                 sigstrength.c \
                 handler.c \
                 sms.c

noinst_HEADERS = atcommand.h \
                 gsmdevice.h \
                 gsmdevice_socket.h \
                 handler.h \
                 sigstrength.h \
                 sms.h \
                 sms_config.h

