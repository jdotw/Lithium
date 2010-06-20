#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>
#include <lithium/snmp.h>
#include <lithium/record.h>

#include "info.h"

/* RAID Info */

static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Variable Retrieval */

i_container* v_info_cnt ()
{ return static_cnt; }

/* Enable / Disable */

int v_info_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("iftinfo", "Controller");
  if (!static_cnt)
  { i_printf (1, "v_info_enable failed to create container"); v_info_disable (self); return -1; }
  //static_cnt->mainform_func = v_info_cntform;
  //static_cnt->sumform_func = v_info_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_info_enable failed to register container"); v_info_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_info_enable failed to load and apply container refresh config"); v_info_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_info_enable failed to create item_list"); v_info_disable (self); return -1; }
  i_list_set_destructor (static_cnt->item_list, v_info_item_free);
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "v_info_enable failed to create master object"); v_info_disable (self); return -1; }
  //static_obj->mainform_func = v_info_objform;

  /* Create info item */
  v_info_item *info = v_info_item_create ();
  if (!info)
  { i_printf (1, "v_info_enable failed to create info item struct"); v_info_disable (self); return -1; }
  info->obj = static_obj;
  static_obj->itemptr = info;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, info);
  if (num != 0)
  { i_printf (1, "v_info_enable failed to enqueue item into static_cnt->item_list"); v_info_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  info->controller_name = l_snmp_metric_create (self, static_obj, "controller_name", "Controller Name", METRIC_STRING, ".1.3.6.1.4.1.1714.1.1.1.11", "0", RECMETHOD_NONE, 0);
  info->logo_vendor = l_snmp_metric_create (self, static_obj, "logo_vendor", "Vendor", METRIC_STRING, ".1.3.6.1.4.1.1714.1.1.1.14", "0", RECMETHOD_NONE, 0);
  info->logo_model = l_snmp_metric_create (self, static_obj, "logo_model", "Model", METRIC_STRING, ".1.3.6.1.4.1.1714.1.1.1.13", "0", RECMETHOD_NONE, 0);
  
  info->cpu = l_snmp_metric_create (self, static_obj, "cpu", "CPU", METRIC_STRING, ".1.3.6.1.4.1.1714.1.1.1.1", "0", RECMETHOD_NONE, 0);
  info->cachesize = l_snmp_metric_create (self, static_obj, "cachesize", "Cache Size", METRIC_GAUGE, ".1.3.6.1.4.1.1714.1.1.1.2", "0", RECMETHOD_NONE, 0);
  info->cachesize->unit_str = strdup ("MB");
  info->memtype = l_snmp_metric_create (self, static_obj, "memtype", "Memory Type", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.1.1.3", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (info->memtype, 0, "FPG");
  i_metric_enumstr_add (info->memtype, 1, "EDO");
  i_metric_enumstr_add (info->memtype, 2, "SDRAM");
  i_metric_enumstr_add (info->memtype, 3, "ECC SDRAM");
  i_metric_enumstr_add (info->memtype, 4, "DDR");
  i_metric_enumstr_add (info->memtype, 5, "ECC DDR");

  info->fwrev_major = l_snmp_metric_create (self, static_obj, "fwrev_major", "Firmware Revision Major", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.1.1.4", "0", RECMETHOD_NONE, 0);
  info->fwrev_minor = l_snmp_metric_create (self, static_obj, "fwrev_minor", "Firmware Revision Minor", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.1.1.5", "0", RECMETHOD_NONE, 0);
  info->fwrev_eng = l_snmp_metric_create (self, static_obj, "fwrev_eng", "Firmware Revision Engineering", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.1.1.6", "0", RECMETHOD_NONE, 0);

  info->brrev_major = l_snmp_metric_create (self, static_obj, "brrev_major", "Boot Record Revision Major", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.1.1.7", "0", RECMETHOD_NONE, 0);
  info->brrev_minor = l_snmp_metric_create (self, static_obj, "brrev_minor", "Boot Record Revision Minor", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.1.1.8", "0", RECMETHOD_NONE, 0);
  info->brrev_eng = l_snmp_metric_create (self, static_obj, "brrev_eng", "Boot Record Revision Engineering", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.1.1.9", "0", RECMETHOD_NONE, 0);

  info->serial = l_snmp_metric_create (self, static_obj, "serial", "Serial Number", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.1.1.10", "0", RECMETHOD_NONE, 0);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);

  return 0;
}

int v_info_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

