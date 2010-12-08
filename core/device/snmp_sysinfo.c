#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>

#include "record.h"
#include "snmp.h"
#include "snmp_sysinfo.h"

/* SNMP System Information MIB */

static int static_enabled = 0;
static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Pointer retrieval */

int l_snmp_sysinfo_enabled ()
{ return static_enabled; }

i_container* l_snmp_sysinfo_cnt ()
{ return static_cnt; }

i_object* l_snmp_sysinfo_obj ()
{ return static_obj; }

/* Enable / Disable */

int l_snmp_sysinfo_enable (i_resource *self)
{
  int num;
  l_snmp_sysinfo_item *sysinfo;
  static i_entity_refresh_config defrefconfig;
  
  if (static_enabled == 1)
  { i_printf (1, "l_snmp_sysinfo_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("snmp_sysinfo", "System Information");
  if (!static_cnt)
  { i_printf (1, "l_snmp_sysinfo_enable failed to create container"); l_snmp_sysinfo_disable (self); return -1; }
  static_cnt->mainform_func = l_snmp_sysinfo_cntform;
  static_cnt->sumform_func = l_snmp_sysinfo_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register container */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "l_snmp_sysinfo_enable failed to register container"); l_snmp_sysinfo_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "l_snmp_sysinfo_enable failed to load and apply container refresh config"); l_snmp_sysinfo_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "l_snmp_sysinfo_enable failed to create item_list"); l_snmp_sysinfo_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "l_snmp_sysinfo_enable failed to create master object"); l_snmp_sysinfo_disable (self); return -1; }
  static_obj->mainform_func = l_snmp_sysinfo_objform;
  static_obj->navtree_expand = NAVTREE_EXP_ALWAYS;

  /* Create sysinfo item */
  sysinfo = l_snmp_sysinfo_item_create ();
  if (!sysinfo)
  { i_printf (1, "l_snmp_sysinfo_enable failed to create static_sysinfo struct"); l_snmp_sysinfo_disable (self); return -1; }
  sysinfo->obj = static_obj;
  static_obj->itemptr = sysinfo;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, sysinfo);
  if (num != 0)
  { i_printf (1, "l_snmp_sysinfo_enable failed to enqueue item into static_cnt->item_list"); l_snmp_sysinfo_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  sysinfo->uptime = l_snmp_metric_create (self, static_obj, "uptime", "Uptime", METRIC_INTERVAL, "sysUpTime", "0", RECMETHOD_NONE, 0);
  i_entity_refreshcb_add (ENTITY(sysinfo->uptime), l_snmp_sysinfo_uptime_refcb, NULL);
  sysinfo->name = l_snmp_metric_create (self, static_obj, "name", "Name", METRIC_STRING, "sysName", "0", RECMETHOD_NONE, 0);
  sysinfo->descr = l_snmp_metric_create (self, static_obj, "descr", "Description", METRIC_STRING, "sysDescr", "0", RECMETHOD_NONE, 0);
  i_entity_refreshcb_add (ENTITY(sysinfo->descr), l_snmp_sysinfo_descr_refcb, NULL);
  sysinfo->location = l_snmp_metric_create (self, static_obj, "location", "Location", METRIC_STRING, "sysLocation", "0", RECMETHOD_NONE, 0);
  sysinfo->contact = l_snmp_metric_create (self, static_obj, "contact", "Contact", METRIC_STRING, "sysContact", "0", RECMETHOD_NONE, 0);
  sysinfo->services = l_snmp_metric_create (self, static_obj, "services", "Services", METRIC_INTEGER, "sysServices", "0", RECMETHOD_NONE, 0);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);
  
  return 0;
}

int l_snmp_sysinfo_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "l_snmp_sysinfo_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  if (static_obj)
  { 
    l_snmp_sysinfo_item *item = static_obj->itemptr;
    if (item) { l_snmp_sysinfo_item_free (item); }
    i_entity_deregister (self, ENTITY(static_obj));
    i_entity_free (ENTITY(static_obj));
    static_obj = NULL;
  }

  if (static_cnt)
  {
    i_entity_deregister (self, ENTITY(static_cnt));
    i_entity_free (ENTITY(static_cnt));
    static_cnt = NULL;
  }

  return 0;
}

