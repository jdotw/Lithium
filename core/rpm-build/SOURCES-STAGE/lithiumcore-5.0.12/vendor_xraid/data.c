#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/parser.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/navtree.h"
#include "induction/navform.h"
#include "induction/hierarchy.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "device/snmp.h"

#include "plist.h"
#include "data.h"

/* Xraid Data Retrieval Sub-System */

static i_container *static_cnt = NULL;
static v_data_item *static_item = NULL;

/* Variable Retrieval */

i_container* v_data_cnt ()
{ return static_cnt; }

v_data_item* v_data_static_item ()
{ return static_item; }

/* Enable / Disable */

int v_data_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("xrdata", "Data Retrieval");
  if (!static_cnt)
  { i_printf (1, "v_data_enable failed to create container"); v_data_disable (self); return -1; }
  static_cnt->hidden = 1;
//  static_cnt->mainform_func = v_data_cntform;
//  static_cnt->sumform_func = v_data_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_data_enable failed to register container"); v_data_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_data_enable failed to load and apply container refresh config"); v_data_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_data_enable failed to create item_list"); v_data_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create table object */
  obj = i_object_create ("master", "Master");
//  obj->mainform_func = v_ipacct_data_objform;

  /* Register table object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Create item */
  static_item = v_data_item_create ();
  static_item->obj = obj;
  obj->itemptr = static_item;

  /*
   * Create Metrics 
   */ 

  /* Standard refresh config */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;  

  /* Sysinfo metric */
  static_item->sysinfo = i_metric_create ("sysinfo", "System Info", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->sysinfo, 0, "Invalid");
  i_metric_enumstr_add (static_item->sysinfo, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->sysinfo));
  i_entity_refresh_config_apply (self, ENTITY(static_item->sysinfo), &refconfig);
  static_item->sysinfo->refresh_func = v_data_sysinfo_refresh;

  /* Power state metric */
  static_item->powerstate = i_metric_create ("powerstate", "Power State", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->powerstate, 0, "Invalid");
  i_metric_enumstr_add (static_item->powerstate, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->powerstate));
  i_entity_refresh_config_apply (self, ENTITY(static_item->powerstate), &refconfig);
  static_item->powerstate->refresh_func = v_data_powerstate_refresh;
  
  /* Top ambient temp metric */
  static_item->ambient_top = i_metric_create ("ambient_top", "Ambient (Top)", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->ambient_top, 0, "Invalid");
  i_metric_enumstr_add (static_item->ambient_top, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->ambient_top));
  i_entity_refresh_config_apply (self, ENTITY(static_item->ambient_top), &refconfig);
  static_item->ambient_top->refresh_func = v_data_ambient_refresh;
  
  /* Bottom ambient temp metric */
  static_item->ambient_bottom = i_metric_create ("ambient_bottom", "Ambient (bottom)", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->ambient_bottom, 0, "Invalid");
  i_metric_enumstr_add (static_item->ambient_bottom, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->ambient_bottom));
  i_entity_refresh_config_apply (self, ENTITY(static_item->ambient_bottom), &refconfig);
  static_item->ambient_bottom->refresh_func = v_data_ambient_refresh;
  
  /* System Status (top) */
  static_item->status_top = i_metric_create ("status_top", "Upper Controller Status", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->status_top, 0, "Invalid");
  i_metric_enumstr_add (static_item->status_top, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->status_top));
  i_entity_refresh_config_apply (self, ENTITY(static_item->status_top), &refconfig);
  static_item->status_top->refresh_func = v_data_status_refresh;

  /* System Status (bottom) */
  static_item->status_bottom = i_metric_create ("status_bottom", "Lower Controller Status", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->status_bottom, 0, "Invalid");
  i_metric_enumstr_add (static_item->status_bottom, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->status_bottom));
  i_entity_refresh_config_apply (self, ENTITY(static_item->status_bottom), &refconfig);
  static_item->status_bottom->refresh_func = v_data_status_refresh;
  
  /* Drive Info (top) */
  static_item->drives_top = i_metric_create ("drives_top", "Drive Status (Top)", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->drives_top, 0, "Invalid");
  i_metric_enumstr_add (static_item->drives_top, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->drives_top));
  i_entity_refresh_config_apply (self, ENTITY(static_item->drives_top), &refconfig);
  static_item->drives_top->refresh_func = v_data_drives_refresh;
  
  /* Drive Info (bottom) */
  static_item->drives_bottom = i_metric_create ("drives_bottom", "Drive Status (Bottom)", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->drives_bottom, 0, "Invalid");
  i_metric_enumstr_add (static_item->drives_bottom, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->drives_bottom));
  i_entity_refresh_config_apply (self, ENTITY(static_item->drives_bottom), &refconfig);
  static_item->drives_bottom->refresh_func = v_data_drives_refresh;

  /* Array Info (top) */
  static_item->arrays_top = i_metric_create ("arrays_top", "Array Status (Top)", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->arrays_top, 0, "Invalid");
  i_metric_enumstr_add (static_item->arrays_top, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->arrays_top));
  i_entity_refresh_config_apply (self, ENTITY(static_item->arrays_top), &refconfig);
  static_item->arrays_top->refresh_func = v_data_arrays_refresh;
  
  /* Array Info (bottom) */
  static_item->arrays_bottom = i_metric_create ("arrays_bottom", "Array Status (Bottom)", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->arrays_bottom, 0, "Invalid");
  i_metric_enumstr_add (static_item->arrays_bottom, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->arrays_bottom));
  i_entity_refresh_config_apply (self, ENTITY(static_item->arrays_bottom), &refconfig);
  static_item->arrays_bottom->refresh_func = v_data_arrays_refresh;

  /* Host Interfaces (top) */
  static_item->hostifaces_top = i_metric_create ("hostifaces_top", "Host Interfaces (Top)", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->hostifaces_top, 0, "Invalid");
  i_metric_enumstr_add (static_item->hostifaces_top, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->hostifaces_top));
  i_entity_refresh_config_apply (self, ENTITY(static_item->hostifaces_top), &refconfig);
  static_item->hostifaces_top->refresh_func = v_data_hostifaces_refresh;

  /* Host Interfaces (bottom) */
  static_item->hostifaces_bottom = i_metric_create ("hostifaces_bottom", "Host Interfaces (Bottom)", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->hostifaces_bottom, 0, "Invalid");
  i_metric_enumstr_add (static_item->hostifaces_bottom, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->hostifaces_bottom));
  i_entity_refresh_config_apply (self, ENTITY(static_item->hostifaces_bottom), &refconfig);
  static_item->hostifaces_bottom->refresh_func = v_data_hostifaces_refresh;

  /* Power Supply (left) */
  static_item->power_left = i_metric_create ("power_left", "Power Supply (Left)", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->power_left, 0, "Invalid");
  i_metric_enumstr_add (static_item->power_left, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->power_left));
  i_entity_refresh_config_apply (self, ENTITY(static_item->power_left), &refconfig);
  static_item->power_left->refresh_func = v_data_power_refresh;

  /* Power Supply (right) */
  static_item->power_right = i_metric_create ("power_right", "Power Supply (Right)", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->power_right, 0, "Invalid");
  i_metric_enumstr_add (static_item->power_right, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->power_right));
  i_entity_refresh_config_apply (self, ENTITY(static_item->power_right), &refconfig);
  static_item->power_right->refresh_func = v_data_power_refresh;

  /* Blower (top) */
  static_item->blower_top = i_metric_create ("blower_top", "Blower (Top)", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->blower_top, 0, "Invalid");
  i_metric_enumstr_add (static_item->blower_top, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->blower_top));
  i_entity_refresh_config_apply (self, ENTITY(static_item->blower_top), &refconfig);
  static_item->blower_top->refresh_func = v_data_blower_refresh;
  
  /* Blower (bottom) */
  static_item->blower_bottom = i_metric_create ("blower_bottom", "Blower (Bottom)", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->blower_bottom, 0, "Invalid");
  i_metric_enumstr_add (static_item->blower_bottom, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->blower_bottom));
  i_entity_refresh_config_apply (self, ENTITY(static_item->blower_bottom), &refconfig);
  static_item->blower_bottom->refresh_func = v_data_blower_refresh;
  
  /* Battery (left) */
  static_item->battery_left = i_metric_create ("battery_left", "Battery Supply (Left)", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->battery_left, 0, "Invalid");
  i_metric_enumstr_add (static_item->battery_left, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->battery_left));
  i_entity_refresh_config_apply (self, ENTITY(static_item->battery_left), &refconfig);
  static_item->battery_left->refresh_func = v_data_battery_refresh;

  /* Battery (right) */
  static_item->battery_right = i_metric_create ("battery_right", "Battery Supply (Right)", METRIC_INTEGER);
  i_metric_enumstr_add (static_item->battery_right, 0, "Invalid");
  i_metric_enumstr_add (static_item->battery_right, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->battery_right));
  i_entity_refresh_config_apply (self, ENTITY(static_item->battery_right), &refconfig);
  static_item->battery_right->refresh_func = v_data_battery_refresh;

  return 0;
}

int v_data_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

