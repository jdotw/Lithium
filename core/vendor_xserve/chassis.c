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

#include "osx_server.h"
#include "data.h"
#include "chassis.h"

/* Xserver Intel Chassis Sub-System */

static i_container *static_cnt = NULL;
static v_chassis_item *static_item = NULL;

/* Variable Retrieval */

i_container* v_chassis_cnt ()
{ return static_cnt; }

v_chassis_item* v_chassis_static_item ()
{ return static_item; }

/* Enable / Disable */

int v_chassis_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("xsichassis", "Chassis");
  if (!static_cnt)
  { i_printf (1, "v_chassis_enable failed to create container"); v_chassis_disable (self); return -1; }
  static_cnt->mainform_func = v_chassis_cntform;
  static_cnt->sumform_func = v_chassis_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;
  
  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_chassis_enable failed to register container"); v_chassis_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_chassis_enable failed to load and apply container refresh config"); v_chassis_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_chassis_enable failed to create item_list"); v_chassis_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create table object */
  obj = i_object_create ("master", "Master");
  obj->mainform_func = v_chassis_objform;

  /* Register table object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* 
   * Trigger sets 
   */

  i_triggerset *tset;

  tset = i_triggerset_create ("powercontrolfault", "Power Control Fault", "powercontrolfault");
  i_triggerset_addtrg (self, tset, "present", "Present", VALTYPE_INTEGER, TRGTYPE_EQUAL, 1, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("mainspowerfault", "Mains Power Fault", "mainspowerfault");
  i_triggerset_addtrg (self, tset, "present", "Present", VALTYPE_INTEGER, TRGTYPE_EQUAL, 1, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("poweroverload", "Power Overload", "poweroverload");
  i_triggerset_addtrg (self, tset, "present", "Present", VALTYPE_INTEGER, TRGTYPE_EQUAL, 1, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

//  tset = i_triggerset_create ("coolingfault", "Cooling Fault", "coolingfault");
//  i_triggerset_addtrg (self, tset, "present", "Present", VALTYPE_INTEGER, TRGTYPE_EQUAL, 1, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
//  i_triggerset_assign (self, static_cnt, tset);

//  tset = i_triggerset_create ("drivefault", "Drive Fault", "drivefault");
//  i_triggerset_addtrg (self, tset, "present", "Present", VALTYPE_INTEGER, TRGTYPE_EQUAL, 1, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
//  i_triggerset_assign (self, static_cnt, tset);

  /*
   * Create Metrics 
   */ 
  
  /* Create item */
  static_item = v_chassis_item_create ();
  static_item->obj = obj;
  obj->itemptr = static_item;

  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  static_item->serial = i_metric_create ("serial", "Serial Number", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->serial));
  i_entity_refresh_config_apply (self, ENTITY(static_item->serial), &refconfig);

  static_item->model = i_metric_create ("model", "Model", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->model));
  i_entity_refresh_config_apply (self, ENTITY(static_item->model), &refconfig);

  static_item->systempower = i_metric_create ("systempower", "System Power", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->systempower, 0, "Off");
  i_metric_enumstr_add (static_item->systempower, 1, "On");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->systempower));
  i_entity_refresh_config_apply (self, ENTITY(static_item->systempower), &refconfig);

  static_item->powercontrolfault = i_metric_create ("powercontrolfault", "Power Control Fault", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->powercontrolfault, 0, "False");
  i_metric_enumstr_add (static_item->powercontrolfault, 1, "True");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->powercontrolfault));
  i_entity_refresh_config_apply (self, ENTITY(static_item->powercontrolfault), &refconfig);

  static_item->mainspowerfault = i_metric_create ("mainspowerfault", "Mains Power Fault", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->mainspowerfault, 0, "False");
  i_metric_enumstr_add (static_item->mainspowerfault, 1, "True");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->mainspowerfault));
  i_entity_refresh_config_apply (self, ENTITY(static_item->mainspowerfault), &refconfig);

  static_item->poweroverload = i_metric_create ("poweroverload", "Power Overload", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->poweroverload, 0, "False");
  i_metric_enumstr_add (static_item->poweroverload, 1, "True");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->poweroverload));
  i_entity_refresh_config_apply (self, ENTITY(static_item->poweroverload), &refconfig);

  static_item->poweroncause = i_metric_create ("poweroncause", "Power On Cause", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->poweroncause, 0, "Manual");
  i_metric_enumstr_add (static_item->poweroncause, 1, "IPMI");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->poweroncause));
  i_entity_refresh_config_apply (self, ENTITY(static_item->poweroncause), &refconfig);

  static_item->poweroffcause = i_metric_create ("poweroffcause", "Power Off Cause", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->poweroffcause, 0, "Unknown");
  i_metric_enumstr_add (static_item->poweroffcause, 2, "Power Overload");
  i_metric_enumstr_add (static_item->poweroffcause, 4, "Power Fault");
  i_metric_enumstr_add (static_item->poweroffcause, 1, "AC Failed");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->poweroffcause));
  i_entity_refresh_config_apply (self, ENTITY(static_item->poweroffcause), &refconfig);

//  static_item->coolingfault = i_metric_create ("coolingfault", "Cooling Fault", METRIC_INTEGER);
//  i_metric_enumstr_add (static_item->coolingfault, 0, "False");
//  i_metric_enumstr_add (static_item->coolingfault, 1, "True");
//  i_entity_register (self, ENTITY(obj), ENTITY(static_item->coolingfault));
//  i_entity_refresh_config_apply (self, ENTITY(static_item->coolingfault), &refconfig);

//  static_item->drivefault = i_metric_create ("drivefault", "Drive Fault", METRIC_INTEGER);
//  i_metric_enumstr_add (static_item->drivefault, 0, "False");
//  i_metric_enumstr_add (static_item->drivefault, 1, "True");
//  i_entity_register (self, ENTITY(obj), ENTITY(static_item->drivefault));
//  i_entity_refresh_config_apply (self, ENTITY(static_item->drivefault), &refconfig);

  static_item->keylock = i_metric_create ("keylock", "Keylock", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->keylock, 0, "Off");
  i_metric_enumstr_add (static_item->keylock, 1, "On");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->keylock));
  i_entity_refresh_config_apply (self, ENTITY(static_item->keylock), &refconfig);

  static_item->idlight = i_metric_create ("idlight", "System ID Light", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->idlight, 0, "Off");
  i_metric_enumstr_add (static_item->idlight, 1, "On");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->idlight));
  i_entity_refresh_config_apply (self, ENTITY(static_item->idlight), &refconfig);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  return 0;
}

int v_chassis_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

