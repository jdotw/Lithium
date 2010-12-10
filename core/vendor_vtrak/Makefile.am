moduledir = $(libdir)/lithium/vendor_modules
module_PROGRAMS = vtrak.so

vtrak_so_SOURCES = main.c \
                   raidinfo.c \
                   raidinfo_item.c \
                   ctrl.c \
                   ctrl_item.c \
                   ctrl_objfact.c \
                   enclosure.c \
                   enclosure_item.c \
                   enclosure_objfact.c \
                   cooling.c \
                   cooling_item.c \
                   cooling_objfact.c \
                   psu.c \
                   psu_item.c \
                   psu_objfact.c \
                   temp.c \
                   temp_item.c \
                   temp_objfact.c \
                   voltage.c \
                   voltage_item.c \
                   voltage_objfact.c \
                   battery.c \
                   battery_item.c \
                   battery_objfact.c \
                   phydrv.c \
                   phydrv_item.c \
                   phydrv_objfact.c \
                   array.c \
                   array_item.c \
                   array_objfact.c \
                   logdrv.c \
                   logdrv_item.c \
                   logdrv_objfact.c

vtrak_so_DEPENDENCIES = ../induction/libinduction.la
vtrak_so_LDFLAGS = @MODULE_LDFLAGS@

