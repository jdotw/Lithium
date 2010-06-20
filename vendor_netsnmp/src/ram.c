#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/value.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/str.h>
#include <lithium/snmp.h>
#include <lithium/record.h>

#include "ram.h"

/* RAM - RAM Usage Sub-System */

static int static_enabled = 0;                        /* 0 = disabled / 1 = enabled */
static i_container *static_cnt = NULL;                /* Cement Container */

/* Variable Fetching */

i_container* l_snmp_nsram_cnt ()
{ return static_cnt; }

/* Sub-System Enable / Disable */

int l_snmp_nsram_enable (i_resource *self)
{
  int num;
  l_snmp_nsram_item *ram;
  i_object *obj;
  static i_entity_refresh_config defrefconfig;

  if (static_enabled == 1)
  { i_printf (1, "l_snmp_nsram_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("nsram", "Memory");
  if (!static_cnt)
  { i_printf (1, "l_snmp_nsram_enable failed to create container"); l_snmp_nsram_disable (self); return -1; }
  //static_cnt->mainform_func = l_snmp_nsram_cntform;
  //static_cnt->sumform_func = l_snmp_nsram_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "l_snmp_nsram_enable failed to register container"); l_snmp_nsram_disable (self); return -1; }

  /* Normal Handling */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "l_snmp_nsram_enable failed to ram and apply container refresh config"); l_snmp_nsram_disable (self); return -1; }

  /* 
   * Trigger sets 
   */

//  tset = i_triggerset_create ("user", "User Utilization", "user");
//  tset->default_duration = 600;
//  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 95, NULL, 0, NULL, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
//  i_triggerset_assign (self, static_cnt, tset);
        
//  tset = i_triggerset_create ("system", "System Utilization", "system");
//  tset->default_duration = 600;
//  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 95, NULL, 0, NULL, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
//  i_triggerset_assign (self, static_cnt, tset);
        
  /* 
   * Items and Master Object
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "l_snmp_nsram_enable failed to create item_list"); l_snmp_nsram_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create object */
  obj = i_object_create ("master", "Master");
  if (!obj)
  { i_printf (1, "l_snmp_nsram_enable failed to create object"); l_snmp_nsram_disable (self); return -1; }
  obj->cnt = static_cnt;
  //obj->mainform_func = l_snmp_nsram_objform;
  //obj->histform_func = l_snmp_nsram_objform_hist;
  obj->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Create item */
  ram = l_snmp_nsram_item_create ();
  if (!ram)
  { i_printf (1, "l_snmp_nsram_enable failed to create item"); l_snmp_nsram_disable (self); return -1; }
  ram->obj = obj;
  obj->itemptr = ram;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Set refresh params */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* 
   * Swap 
   */

  ram->swap_total = l_snmp_metric_create (self, obj, "swap_total", "Total Swap", METRIC_GAUGE, ".1.3.6.1.4.1.2021.4.3.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  ram->swap_total->alloc_unit = 1024;
  ram->swap_total->valstr_func = i_string_volume_metric;
  ram->swap_total->kbase = 1024;
  ram->swap_total->unit_str = strdup ("byte");
  
  ram->swap_avail = l_snmp_metric_create (self, obj, "swap_avail", "Available Swap", METRIC_GAUGE, ".1.3.6.1.4.1.2021.4.4.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  ram->swap_avail->record_defaultflag = 1;
  ram->swap_avail->alloc_unit = 1024;
  ram->swap_avail->valstr_func = i_string_volume_metric;
  ram->swap_avail->kbase = 1024;
  ram->swap_avail->unit_str = strdup ("byte");

  ram->swap_used = i_metric_acdiff_create (self, obj, "used", "Used", METRIC_FLOAT, RECMETHOD_NONE, ram->swap_total, ram->swap_avail, ACDIFF_REFCB_YMET);

  ram->swap_usedpc = i_metric_acpcent_create (self, obj, "swap_usedpc", "Swap Used Percent", RECMETHOD_RRD, ram->swap_used, ram->swap_total, ACPCENT_REFCB_GAUGE);
  ram->swap_usedpc->record_defaultflag = 1;

  ram->swap_in = l_snmp_metric_create (self, obj, "swap_in", "Swapping In", METRIC_GAUGE, ".1.3.6.1.4.1.2021.11.3.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  ram->swap_in->record_defaultflag = 1;
  ram->swap_in->alloc_unit = 1000;
  ram->swap_in->valstr_func = i_string_rate_metric;
  ram->swap_in->unit_str = strdup ("byte/sec");

  ram->swap_out = l_snmp_metric_create (self, obj, "swap_out", "Swapping Out", METRIC_GAUGE, ".1.3.6.1.4.1.2021.11.3.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  ram->swap_out->record_defaultflag = 1;
  ram->swap_out->alloc_unit = 1000;
  ram->swap_out->valstr_func = i_string_rate_metric;
  ram->swap_out->unit_str = strdup ("byte/sec");

  /* 
   * Real 
   */

  ram->real_total = l_snmp_metric_create (self, obj, "real_total", "Total Real", METRIC_GAUGE, ".1.3.6.1.4.1.2021.4.5.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  ram->real_total->alloc_unit = 1024;
  ram->real_total->valstr_func = i_string_volume_metric;
  ram->real_total->kbase = 1024;
  ram->real_total->unit_str = strdup ("byte");
  
  ram->real_avail = l_snmp_metric_create (self, obj, "real_avail", "Available Real", METRIC_GAUGE, ".1.3.6.1.4.1.2021.4.6.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  ram->real_avail->record_defaultflag = 1;
  ram->real_avail->alloc_unit = 1024;
  ram->real_avail->valstr_func = i_string_volume_metric;
  ram->real_avail->kbase = 1024;
  ram->real_avail->unit_str = strdup ("byte");

  ram->real_used = i_metric_acdiff_create (self, obj, "used", "Used", METRIC_FLOAT, RECMETHOD_NONE, ram->real_total, ram->real_avail, ACDIFF_REFCB_YMET);

  ram->real_usedpc = i_metric_acpcent_create (self, obj, "real_usedpc", "Real Used Percent", RECMETHOD_RRD, ram->real_used, ram->real_total, ACPCENT_REFCB_GAUGE);
  ram->real_usedpc->record_defaultflag = 1;

  ram->shared = l_snmp_metric_create (self, obj, "shared", "Shared", METRIC_GAUGE, ".1.3.6.1.4.1.2021.4.13.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  ram->shared->record_defaultflag = 1;
  ram->shared->alloc_unit = 1024;
  ram->shared->valstr_func = i_string_volume_metric;
  ram->shared->kbase = 1024;
  ram->shared->unit_str = strdup ("byte");
  
  ram->buffers = l_snmp_metric_create (self, obj, "buffers", "Buffers", METRIC_GAUGE, ".1.3.6.1.4.1.2021.4.14.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  ram->buffers->record_defaultflag = 1;
  ram->buffers->alloc_unit = 1024;
  ram->buffers->valstr_func = i_string_volume_metric;
  ram->buffers->kbase = 1024;
  ram->buffers->unit_str = strdup ("byte");
  
  ram->cached = l_snmp_metric_create (self, obj, "cached", "Cached", METRIC_GAUGE, ".1.3.6.1.4.1.2021.4.15.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  ram->cached->record_defaultflag = 1;
  ram->cached->alloc_unit = 1024;
  ram->cached->valstr_func = i_string_volume_metric;
  ram->cached->kbase = 1024;
  ram->cached->unit_str = strdup ("byte");
  
  
  /* Load/Apply refresh configuration for the object */
  i_entity_refresh_config_loadapply (self, ENTITY(obj), &defrefconfig);

  /* Evaluate recording rules */
  l_record_eval_recrules_obj (self, obj);
  
  /* Apply all triggersets */
  num = i_triggerset_evalapprules_allsets (self, obj);
  if (num != 0)
  { i_printf (1, "l_snmp_nsram_enable warning, failed to apply all triggersets for master object"); }

  /* Enqueue the ram item */
  i_list_enqueue (static_cnt->item_list, ram);

  return 0;
}

int l_snmp_nsram_disable (i_resource *self)
{
  /* Disable the sub-system */

  if (static_enabled == 0)
  { i_printf (1, "l_snmp_nsram_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

