CFLAGS = @CFLAGS@
LIBS = @LIBS@

moduledir = $(libdir)/lithium
module_PROGRAMS = admin.so

admin_so_SOURCES = main.c \
				   customer.c \
				   customer_sort.c \
				   customer_resource.c \
				   userauth.c \
				   autoconf.c \
				   restart.c \
				   restart_form.c \
				   snmpagent.c \
				   snmpagent_fdset.c \
				   lic_admin.c \
				   ../customer/lic.c \
				   ../customer/lic_key.c \
				   mib2c/lcCustomerTable.c \
                   mib2c/lcSiteTable.c \
 				   mib2c/lcDeviceTable.c \
				   mib2c/lcContainerTable.c \
				   mib2c/lcObjectTable.c \
				   mib2c/lcMetricTable.c \
				   mib2c/lcTriggerTable.c


noinst_HEADERS = customer.h \
				 autoconf.h \
				 userauth.h \
				 snmpagent.h \
				 restart.h


