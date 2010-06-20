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

#include "sysinfo.h"

/* Xraid System Information Sub-System */

static i_container *static_cnt = NULL;
static v_sysinfo_item *static_item = NULL;

/* Variable Retrieval */

i_container* v_sysinfo_cnt ()
{ return static_cnt; }

v_sysinfo_item* v_sysinfo_static_item ()
{ return static_item; }

/* Enable / Disable */

int v_sysinfo_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("xrsysinfo", "System Information");
  if (!static_cnt)
  { i_printf (1, "v_sysinfo_enable failed to create container"); v_sysinfo_disable (self); return -1; }
  static_cnt->mainform_func = v_sysinfo_cntform;
  static_cnt->sumform_func = v_sysinfo_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_sysinfo_enable failed to register container"); v_sysinfo_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_sysinfo_enable failed to load and apply container refresh config"); v_sysinfo_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_sysinfo_enable failed to create item_list"); v_sysinfo_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create table object */
  obj = i_object_create ("master", "Master");
  obj->mainform_func = v_sysinfo_objform;

  /* Register table object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));


  /* Create item */
  static_item = v_sysinfo_item_create ();
  static_item->obj = obj;
  obj->itemptr = static_item;

  /*
   * Create Metrics 
   */ 
  
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  static_item->name = i_metric_create ("name", "Name", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->name));
  i_entity_refresh_config_apply (self, ENTITY(static_item->name), &refconfig);

  static_item->DN = i_metric_create ("DN", "DN", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->DN));
  i_entity_refresh_config_apply (self, ENTITY(static_item->DN), &refconfig);

  static_item->PN = i_metric_create ("PN", "PN", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->PN));
  i_entity_refresh_config_apply (self, ENTITY(static_item->PN), &refconfig);

  static_item->contact = i_metric_create ("contact", "contact", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->contact));
  i_entity_refresh_config_apply (self, ENTITY(static_item->contact), &refconfig);

  static_item->DS = i_metric_create ("DS", "DS", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->DS));
  i_entity_refresh_config_apply (self, ENTITY(static_item->DS), &refconfig);

  static_item->location = i_metric_create ("location", "Location", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->location));
  i_entity_refresh_config_apply (self, ENTITY(static_item->location), &refconfig);

  static_item->version = i_metric_create ("version", "Version", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->version));
  i_entity_refresh_config_apply (self, ENTITY(static_item->version), &refconfig);

  static_item->uptime = i_metric_create ("uptime", "uptime", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->uptime));
  i_entity_refresh_config_apply (self, ENTITY(static_item->uptime), &refconfig);

  static_item->Fl = i_metric_create ("Fl", "Fl", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->Fl));
  i_entity_refresh_config_apply (self, ENTITY(static_item->Fl), &refconfig);

  static_item->mac = i_metric_create ("mac", "MAC", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->mac));
  i_entity_refresh_config_apply (self, ENTITY(static_item->mac), &refconfig);

  static_item->ip = i_metric_create ("ip", "IP", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->ip));
  i_entity_refresh_config_apply (self, ENTITY(static_item->ip), &refconfig);

  static_item->subnet = i_metric_create ("subnet", "subnet", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->subnet));
  i_entity_refresh_config_apply (self, ENTITY(static_item->subnet), &refconfig);
  
  static_item->serial = i_metric_create ("serial", "serial", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->serial));
  i_entity_refresh_config_apply (self, ENTITY(static_item->serial), &refconfig);
  
  static_item->firmware_top = i_metric_create ("firmware_top", "Upper Controller Firmware", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->firmware_top));
  i_entity_refresh_config_apply (self, ENTITY(static_item->firmware_top), &refconfig);
  
  static_item->firmware_bottom = i_metric_create ("firmware_bottom", "Lower Controller Firmware", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->firmware_bottom));
  i_entity_refresh_config_apply (self, ENTITY(static_item->firmware_bottom), &refconfig);
  
  return 0;
}

int v_sysinfo_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

