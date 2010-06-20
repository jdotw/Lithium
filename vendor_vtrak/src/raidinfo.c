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

#include "raidinfo.h"

/* RAID Info */

static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Variable Retrieval */

i_container* v_raidinfo_cnt ()
{ return static_cnt; }

/* Enable / Disable */

int v_raidinfo_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("vtraidinfo", "System Information");
  if (!static_cnt)
  { i_printf (1, "v_raidinfo_enable failed to create container"); v_raidinfo_disable (self); return -1; }

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_raidinfo_enable failed to register container"); v_raidinfo_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_raidinfo_enable failed to load and apply container refresh config"); v_raidinfo_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_raidinfo_enable failed to create item_list"); v_raidinfo_disable (self); return -1; }
  i_list_set_destructor (static_cnt->item_list, v_raidinfo_item_free);
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "v_raidinfo_enable failed to create master object"); v_raidinfo_disable (self); return -1; }

  /* Create raidinfo item */
  v_raidinfo_item *raidinfo = v_raidinfo_item_create ();
  if (!raidinfo)
  { i_printf (1, "v_raidinfo_enable failed to create raidinfo item struct"); v_raidinfo_disable (self); return -1; }
  raidinfo->obj = static_obj;
  static_obj->itemptr = raidinfo;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, raidinfo);
  if (num != 0)
  { i_printf (1, "v_raidinfo_enable failed to enqueue item into static_cnt->item_list"); v_raidinfo_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  /* Info */
  raidinfo->descr = l_snmp_metric_create (self, static_obj, "descr", "Description", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.1.1.3", "1", RECMETHOD_NONE, 0);
  raidinfo->alias = l_snmp_metric_create (self, static_obj, "alias", "Alias", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.2.1.1", "1", RECMETHOD_NONE, 0);
  raidinfo->wwn = l_snmp_metric_create (self, static_obj, "wwn", "WWN", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.2.1.2", "1", RECMETHOD_NONE, 0);

  /* State */
  raidinfo->redundancy = l_snmp_metric_create (self, static_obj, "redundancy", "Redundancy", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.1.1.1.6", "1", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (raidinfo->redundancy, 0, "Non-Redundant");
  i_metric_enumstr_add (raidinfo->redundancy, 1, "Redundant");
  i_metric_enumstr_add (raidinfo->redundancy, 2, "Critical");
  i_metric_enumstr_add (raidinfo->redundancy, 255, "Not-Supported");
  raidinfo->controllers = l_snmp_metric_create (self, static_obj, "controllers", "Controllers", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.1.1.1.7", "1", RECMETHOD_NONE, 0);
  raidinfo->interconnect = l_snmp_metric_create (self, static_obj, "interconnect", "Interconnect Type", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.1.1.1.8", "1", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (raidinfo->interconnect, 0, "Not-Supported");
  i_metric_enumstr_add (raidinfo->interconnect, 1, "SAS");
  raidinfo->enclosures = l_snmp_metric_create (self, static_obj, "enclosures", "Enclosures", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.1.1.1.9", "1", RECMETHOD_NONE, 0);

  /* Type */
  raidinfo->vendor = l_snmp_metric_create (self, static_obj, "vendor", "Vendor", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.2.1.3", "1", RECMETHOD_NONE, 0);
  raidinfo->model = l_snmp_metric_create (self, static_obj, "model", "Model", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.2.1.4", "1", RECMETHOD_NONE, 0);
  raidinfo->serial = l_snmp_metric_create (self, static_obj, "serial", "Serial Number", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.2.1.5", "1", RECMETHOD_NONE, 0);
  raidinfo->revision = l_snmp_metric_create (self, static_obj, "revision", "Revision", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.2.1.7", "1", RECMETHOD_NONE, 0);
  raidinfo->manufactured = l_snmp_metric_create (self, static_obj, "manufactured", "Manufactured", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.2.1.8", "1", RECMETHOD_NONE, 0);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);

  return 0;
}

int v_raidinfo_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

