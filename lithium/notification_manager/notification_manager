CFLAGS = @CFLAGS@
LIBS = @LIBS@

moduledir = $(libdir)/lithium
module_PROGRAMS = notification_manager.so

notification_manager_so_SOURCES = main.c \
                                  handler.c \
                                  email.c \
                                  sms.c

notification_manager_so_LDFLAGS = -shared

noinst_HEADERS = email.h \
                 handler.h \
                 sms.h
