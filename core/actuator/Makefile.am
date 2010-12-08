moduledir = $(libdir)/lithium
module_PROGRAMS = actuator.so

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

actuator_so_DEPENDENCIES = ../induction/libinduction.la
actuator_so_LDADD = ../induction/libinduction.la
actuator_so_LDFLAGS = @MODULE_LDFLAGS@

