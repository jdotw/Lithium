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
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>

#include "record.h"
#include "navtree.h"
#include "mformprio.h"
#include "snmp.h"
#include "snmp_users.h"

/* SNMP Host-resources User Count Sub-System */

static int static_enabled = 0;                        /* 0 = disabled / 1 = enabled */
static i_container *static_cnt = NULL;                /* Cement Container */
static i_object *static_masterobj = NULL;             /* The 'master' snmp_users obj */

/* Variable Fetching */

i_container* l_snmp_users_cnt ()
{ return static_cnt; }

i_object* l_snmp_users_masterobj ()
{ return static_masterobj; }

/* Sub-System Enable / Disable */

int l_snmp_users_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config refconfig;
  i_object *obj;
  l_snmp_users_item *item;

  if (static_enabled == 1)
  { i_printf (1, "l_snmp_users_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("user_count", "User Count");
  if (!static_cnt)
  { i_printf (1, "l_snmp_users_enable failed to create container"); l_snmp_users_disable (self); return -1; }
  static_cnt->mainform_func = l_snmp_users_cntform;
  static_cnt->sumform_func = l_snmp_users_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "l_snmp_users_enable failed to register container"); l_snmp_users_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &refconfig);
  if (num != 0)
  { i_printf (1, "l_snmp_users_enable failed to load and apply container refresh config"); l_snmp_users_disable (self); return -1; }

  /*
   * Item List and Master Object
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "l_snmp_users_enable failed to create item_list"); l_snmp_users_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create object */
  obj = i_object_create ("master", "Master User Count");
  if (!obj)
  { i_printf (1, "l_snmp_users_enable failed to create object"); return -1; }
  obj->cnt = l_snmp_users_cnt ();
  obj->mainform_func = l_snmp_users_objform;
  obj->histform_func = l_snmp_users_objform_hist;
  static_masterobj = obj;

  /* Create item */
  item = l_snmp_users_item_create ();
  if (!item)
  { i_printf (1, "l_snmp_users_enable failed to create item"); i_entity_free (obj); return -1; }
  item->obj = obj;
  obj->itemptr = item;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));
  /* Setup refresh configuration default
   * to default to a REFMETHOD_PARENT 
   * refresh method. This default is applied
   * to all metrics and to the object
   * itself
   */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* Operation count metrics */
  item->user_count = l_snmp_metric_create (self, obj, "user_count", "User Count", METRIC_GAUGE, ".1.3.6.1.2.1.25.1.5", "0", RECMETHOD_RRD, 0);
  item->user_count->record_defaultflag = 1;

  /* Load/Apply refresh configuration for the object */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), &refconfig);
  if (num != 0)
  {
    i_printf (1, "l_snmp_users_enable failed to load and apply object refresh config for object master object");
    return -1;
  }

  /* Enqueue the snmp_users item */
  i_list_enqueue (static_cnt->item_list, item);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_masterobj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_masterobj);
  
  return 0;  
}

int l_snmp_users_disable (i_resource *self)
{
  /* Disable the sub-system */

  if (static_enabled == 0)
  { i_printf (1, "l_snmp_users_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  /* Deregister/Free container */
  if (static_cnt) 
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}


