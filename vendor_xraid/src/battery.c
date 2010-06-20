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
#include <lithium/record.h>
#include <lithium/snmp.h>

#include "battery.h"

/* Xraid Battery Sub-System */

static i_container *static_cnt = NULL;
static v_battery_item *static_left_item = NULL;
static v_battery_item *static_right_item = NULL;

/* Variable Retrieval */

i_container* v_battery_cnt ()
{ return static_cnt; }

v_battery_item* v_battery_static_left_item ()
{ return static_left_item; }

v_battery_item* v_battery_static_right_item ()
{ return static_right_item; }

/* Enable / Disable */

int v_battery_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("xrbattery", "Battery Units");
  if (!static_cnt)
  { i_printf (1, "v_battery_enable failed to create container"); v_battery_disable (self); return -1; }
  static_cnt->mainform_func = v_battery_cntform;
  static_cnt->sumform_func = v_battery_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_battery_enable failed to register container"); v_battery_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_battery_enable failed to load and apply container refresh config"); v_battery_disable (self); return -1; }

  /*
   * Triggers
   */

  i_triggerset *tset;

  /* Drive Temp */
  tset = i_triggerset_create ("battery", "Battery", "present");
  i_triggerset_addtrg (self, tset, "notpresent", "Not Present", VALTYPE_INTEGER, TRGTYPE_EQUAL, 0, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);  
  i_triggerset_assign (self, static_cnt, tset);
  
  /* 
   * Items and objects
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_battery_enable failed to create item_list"); v_battery_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create top and bottom items */
  int i;
  for (i=0; i < 2; i++)
  {  
    /* Create table object */
    if (i == 0)
    { obj = i_object_create ("left", "Left Battery Unit"); }
    else
    { obj = i_object_create ("right", "Right Battery Unit"); }
    
    obj->mainform_func = v_battery_objform; 

    /* Register table object */
    i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

    /* Create item */
    v_battery_item *item;
    item = v_battery_item_create ();
    item->obj = obj;
    obj->itemptr = item;
    if (i == 0) static_left_item = item;
    else static_right_item = item;

    /*
     * Create Metrics 
     */ 
  
    memset (&refconfig, 0, sizeof(i_entity_refresh_config));
    refconfig.refresh_method = REFMETHOD_EXTERNAL;
    refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
    refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

    item->present = i_metric_create ("present", "Present", METRIC_INTEGER);
    i_entity_register (self, ENTITY(obj), ENTITY(item->present));
    i_entity_refresh_config_apply (self, ENTITY(item->present), &refconfig);
    i_metric_enumstr_add (item->present, 0, "No");
    i_metric_enumstr_add (item->present, 1, "Yes");

    /* Evaluate apprules for all triggersets */
    i_triggerset_evalapprules_allsets (self, obj);

    /* Evaluate recrules for all metrics */
    l_record_eval_recrules_obj (self, obj);
  }

  return 0;
}

int v_battery_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

