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

#include "snmp.h"
#include "record.h"
#include "snmp_nsram.h"

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

  i_triggerset *tset = i_triggerset_create ("real_usedpc", "Total RAM Utilization", "real_usedpc");
  tset->default_duration = 600;
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 95, NULL, 98, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 98, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  tset = i_triggerset_create ("real_activepc", "Active RAM Utilization", "real_activepc");
  tset->default_duration = 600;
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 70, NULL, 85, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 85, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
        
  tset = i_triggerset_create ("swap_usedpc", "Swap Utilization", "swap_usedpc");
  tset->default_duration = 600;
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 80, NULL, 97, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 97, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
        
  tset = i_triggerset_create ("swap_in", "Swapping In", "swap_in");
  tset->default_duration = 600;
  tset->default_applyflag = 0;
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 10000000, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);

  tset = i_triggerset_create ("swap_out", "Swapping Out", "swap_out");
  tset->default_duration = 600;
  tset->default_applyflag = 0;
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 10000000, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);

  /* 
   * Items and Master Object
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "l_snmp_nsram_enable failed to create item_list"); l_snmp_nsram_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create real object */
  i_object *real_obj = i_object_create ("master", "RAM");
  if (!real_obj)
  { i_printf (1, "l_snmp_nsram_enable failed to create object"); l_snmp_nsram_disable (self); return -1; }
  real_obj->cnt = static_cnt;
  real_obj->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Create swap object */
  i_object *swap_obj = i_object_create ("swap", "Swap");
  if (!swap_obj)
  { i_printf (1, "l_snmp_nsram_enable failed to create object"); l_snmp_nsram_disable (self); return -1; }
  swap_obj->cnt = static_cnt;
  swap_obj->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Create item */
  ram = l_snmp_nsram_item_create ();
  if (!ram)
  { i_printf (1, "l_snmp_nsram_enable failed to create item"); l_snmp_nsram_disable (self); return -1; }
  ram->real_obj = real_obj;
  ram->swap_obj = swap_obj;
  real_obj->itemptr = ram;
  swap_obj->itemptr = ram;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(real_obj));
  i_entity_register (self, ENTITY(static_cnt), ENTITY(swap_obj));

  /* Set refresh params */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* Check for Xsnmp 
   *
   * - If Xsnmp is enabled, the static Xsnmp OIDs are used
   * - If Xsnmp is not enabled then the RAM info is taken from
   *   storage resources objects found during Storage Resource object creation
   */
  if (l_snmp_xsnmp_enabled())
  {
    /*
     * Use Xsnmp metrics 
     */

    ram->real_used = l_snmp_metric_create (self, real_obj, "real_used", "Used", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.3.4.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
    ram->real_used->alloc_unit = 1024 * 1024;
    ram->real_used->valstr_func = i_string_volume_metric;
    ram->real_used->kbase = 1024;
    ram->real_used->unit_str = strdup ("byte");
    ram->real_used->record_defaultflag = 1;
  
    ram->real_free = l_snmp_metric_create (self, real_obj, "real_free", "Free", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.3.5.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
    ram->real_free->alloc_unit = 1024 * 1024;
    ram->real_free->valstr_func = i_string_volume_metric;
    ram->real_free->kbase = 1024;
    ram->real_free->unit_str = strdup ("byte");

    ram->real_total = i_metric_acsum_create (self, real_obj, "real_total", "Total", METRIC_FLOAT, RECMETHOD_RRD, ram->real_used, ram->real_free, ACSUM_REFCB_YMET);
    ram->real_total->valstr_func = i_string_volume_metric;
    ram->real_total->kbase = 1024;
    ram->real_total->unit_str = strdup ("byte");
    ram->real_total->summary_flag = 1;
  
    ram->real_wired = l_snmp_metric_create (self, real_obj, "real_wired", "Wired", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.3.1.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
    ram->real_wired->record_defaultflag = 1;
    ram->real_wired->alloc_unit = 1024 * 1024;
    ram->real_wired->valstr_func = i_string_volume_metric;
    ram->real_wired->kbase = 1024;
    ram->real_wired->unit_str = strdup ("byte");
  
    ram->real_active = l_snmp_metric_create (self, real_obj, "real_active", "Active", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.3.2.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
    ram->real_active->record_defaultflag = 1;
    ram->real_active->alloc_unit = 1024 * 1024;
    ram->real_active->valstr_func = i_string_volume_metric;
    ram->real_active->kbase = 1024;
    ram->real_active->unit_str = strdup ("byte");

    ram->real_totalactive = i_metric_acsum_create(self, real_obj, "real_totalactive", "Total Active", METRIC_FLOAT, RECMETHOD_RRD, ram->real_wired, ram->real_active, ACSUM_REFCB_YMET);
    ram->real_totalactive->record_defaultflag = 1;
    ram->real_totalactive->valstr_func = i_string_volume_metric;
    ram->real_totalactive->kbase = 1024;
    ram->real_totalactive->unit_str = strdup ("byte");
  
    ram->real_inactive = l_snmp_metric_create (self, real_obj, "real_inactive", "Inactive", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.3.3.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
    ram->real_inactive->alloc_unit = 1024 * 1024;
    ram->real_inactive->valstr_func = i_string_volume_metric;
    ram->real_inactive->kbase = 1024;
    ram->real_inactive->unit_str = strdup ("byte");
    ram->real_inactive->summary_flag = 1;
  
    ram->real_usedpc = i_metric_acpcent_create (self, real_obj, "real_usedpc", "Used Percent", RECMETHOD_RRD, ram->real_used, ram->real_total, ACPCENT_REFCB_MAX);
    ram->real_usedpc->record_defaultflag = 1;

    ram->real_activepc = i_metric_acpcent_create (self, real_obj, "real_activepc", "Active Percent", RECMETHOD_RRD, ram->real_totalactive, ram->real_total, ACPCENT_REFCB_GAUGE);
    ram->real_activepc->record_defaultflag = 1;
  }

  /* 
   * Swap 
   */

  ram->swap_in = l_snmp_metric_create (self, swap_obj, "swap_in", "Swapping In", METRIC_GAUGE, ".1.3.6.1.4.1.2021.11.3.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  ram->swap_in->record_defaultflag = 1;
  ram->swap_in->alloc_unit = 1000;
  ram->swap_in->valstr_func = i_string_rate_metric;
  ram->swap_in->unit_str = strdup ("byte/min");

  ram->swap_out = l_snmp_metric_create (self, swap_obj, "swap_out", "Swapping Out", METRIC_GAUGE, ".1.3.6.1.4.1.2021.11.4.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  ram->swap_out->record_defaultflag = 1;
  ram->swap_out->alloc_unit = 1000;
  ram->swap_out->valstr_func = i_string_rate_metric;
  ram->swap_out->unit_str = strdup ("byte/min");

  /* Load/Apply refresh configuration for the object */
  i_entity_refresh_config_loadapply (self, ENTITY(real_obj), &defrefconfig);
  i_entity_refresh_config_loadapply (self, ENTITY(swap_obj), &defrefconfig);

  /* Evaluate recording rules */
  l_record_eval_recrules_obj (self, real_obj);
  l_record_eval_recrules_obj (self, swap_obj);
  
  /* Apply all triggersets */
  i_triggerset_evalapprules_allsets (self, real_obj);
  i_triggerset_evalapprules_allsets (self, swap_obj);

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

