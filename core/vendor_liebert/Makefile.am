moduledir = $(libdir)/lithium/vendor_modules
module_PROGRAMS = liebert.so

liebert_so_SOURCES = main.c \
					 temp.c \
					 temp_item.c \
					 temp_cntform.c \
					 temp_sumform.c \
					 temp_objform.c \
					 humid.c \
					 humid_item.c \
					 humid_cntform.c \
					 humid_sumform.c \
					 humid_objform.c \
					 state.c \
					 state_item.c \
					 state_cntform.c \
					 state_sumform.c \
					 state_objform.c \
					 unit_config.c \
					 unit_config_item.c \
					 unit_config_cntform.c \
					 unit_config_objform.c \
					 opstats.c \
					 opstats_item.c \
					 opstats_cntform.c \
					 opstats_objform.c

liebert_so_DEPENDENCIES = ../induction/libinduction.la
liebert_so_LDADD = ../induction/libinduction.la
liebert_so_LDFLAGS = @MODULE_LDFLAGS@

