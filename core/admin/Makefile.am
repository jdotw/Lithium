moduledir = $(libdir)/lithium
pkglib_LTLIBRARIES = admin.la

admin_la_SOURCES = main.c \
				   customer.c \
				   customer_sort.c \
				   customer_resource.c \
				   autoconf.c \
				   restart.c \
				   restart_form.c \
				   snmpagent.c \
				   snmpagent_fdset.c \
				   lic_admin.c \
				   mib2c/lcCustomerTable.c \
                   mib2c/lcSiteTable.c \
 				   mib2c/lcDeviceTable.c \
				   mib2c/lcContainerTable.c \
				   mib2c/lcObjectTable.c \
				   mib2c/lcMetricTable.c \
				   mib2c/lcTriggerTable.c \
				   ../customer/lic_loadkeys.c

admin_la_LDFLAGS = -module @MODULE_LDFLAGS@
