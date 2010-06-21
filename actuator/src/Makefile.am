moduledir = $(libdir)/lithium
module_PROGRAMS = actuator.so

CFLAGS=@CFLAGS@ -g

actuator_so_SOURCES = customer.c \
                      form_get.c \
                      form_render.c \
                      main.c \
                      resource_address.c \
                      shutdown.c \
                      form_submit.c \
					  strutil.c \
					  script_upload.c \
					  script_download.c \
					  xml_get.c \
					  xml_render.c

noinst_HEADERS = customer.h \
                 form_get.h \
                 form_render.h \
                 form_submit.h \
                 resource_address.h \
                 shutdown.h \
				 script_upload.h \
				 script_download.h \
				 xml_get.h \
				 xml_render.h \
				 strutil.h
