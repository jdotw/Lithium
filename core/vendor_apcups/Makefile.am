moduledir = $(libdir)/lithium/vendor_modules
module_PROGRAMS = apcups.so

apcups_so_SOURCES = main.c \
					upsinfo.c \
					upsinfo_item.c \
					battery.c \
					battery_item.c \
					output.c \
					output_item.c \
					input.c \
					input_item.c \
					outphase.c \
					outphase_item.c \
					outphase_objfact.c \
					inphase.c \
					inphase_item.c \
					inphase_objfact.c \
					intenv.c \
					intenv_objfact.c \
					intenv_item.c \
					device.c \
					device_objfact.c \
					device_item.c
 
apcups_so_DEPENDENCIES = ../induction/libinduction.la
apcups_so_LDFLAGS = @MODULE_LDFLAGS@

