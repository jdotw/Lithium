pkglibdir = $(libdir)/lithium/vendor_modules
pkglib_LTLIBRARIES = qlogic.la genericfcswitch.la

common_SOURCES = unit.c \
				 unit_objfact.c \
				 port.c \
				 port_objfact.c \
				 port_refcb.c \
				 sensor.c \
				 sensor_objfact.c

qlogic_la_SOURCES = qlogic.c \
					$(common_SOURCES)
qlogic_la_LDFLAGS = -module @MODULE_LDFLAGS@

genericfcswitch_la_SOURCES = generic.c \
							 $(common_SOURCES)
genericfcswitch_la_LDFLAGS = -module @MODULE_LDFLAGS@


