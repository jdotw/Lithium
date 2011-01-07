#include <stdlib.h>

#include "induction.h"
#include "induction/timer.h"
#include "induction/timeutil.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/str.h"
#include "induction/hierarchy.h"
#include "induction/list.h"

#include "device/snmp.h"

#include "unit.h"
#include "port.h"
#include "sensor.h"

/* 
 * Physical Unit - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_unit_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_unit_item *unit;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu(pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_unit_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create unit item struct */
  unit = v_unit_item_create ();
  if (!unit)
  { i_printf (1, "v_unit_objfact_fab failed to create unit item for object %s", obj->name_str); return -1; }
  unit->obj = obj;
  obj->itemptr = unit;
  unit->oid_suffix = strdup(index_oidstr);

  /* 
   * Metric Creation 
   */

  i_entity_refresh_config refconfig;
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* Type */
  unit->type = l_snmp_metric_create (self, obj, "type", "Type", METRIC_INTEGER, ".1.3.6.1.3.94.1.6.1.3", index_oidstr, RECMETHOD_NONE, 0);
  unit->type->summary_flag = 1;
  i_metric_enumstr_add (unit->type, 1, "Unknown");
  i_metric_enumstr_add (unit->type, 2, "Other");
  i_metric_enumstr_add (unit->type, 3, "Hub");
  i_metric_enumstr_add (unit->type, 4, "Switch");
  i_metric_enumstr_add (unit->type, 5, "Gateway");
  i_metric_enumstr_add (unit->type, 6, "Converter");
  i_metric_enumstr_add (unit->type, 7, "HBA");
  i_metric_enumstr_add (unit->type, 8, "Proxy-Agent");
  i_metric_enumstr_add (unit->type, 9, "Storage");
  i_metric_enumstr_add (unit->type, 10, "Host");
  i_metric_enumstr_add (unit->type, 11, "Storage");
  i_metric_enumstr_add (unit->type, 12, "Module");
  i_metric_enumstr_add (unit->type, 13, "Software");
  i_metric_enumstr_add (unit->type, 14, "StorageAccess");
  i_metric_enumstr_add (unit->type, 15, "WDM");
  i_metric_enumstr_add (unit->type, 16, "UPS");
  i_metric_enumstr_add (unit->type, 17, "NAS");

  /* Port Count */
  unit->port_count = l_snmp_metric_create (self, obj, "port_count", "Port Count", METRIC_GAUGE, ".1.3.6.1.3.94.1.6.1.4", index_oidstr, RECMETHOD_NONE, 0);
  unit->port_count->summary_flag = 1;

  /* Product */
  unit->product = l_snmp_metric_create (self, obj, "product", "Product", METRIC_STRING, ".1.3.6.1.3.94.1.6.1.7", index_oidstr, RECMETHOD_NONE, 0);
  unit->product->summary_flag = 1;

  /* Serial */
  unit->serial = l_snmp_metric_create (self, obj, "serial", "Serial", METRIC_STRING, ".1.3.6.1.3.94.1.6.1.8", index_oidstr, RECMETHOD_NONE, 0);
  unit->serial->summary_flag = 1;

  /* Uptime */
  unit->uptime = l_snmp_metric_create (self, obj, "uptime", "Uptime", METRIC_INTERVAL, ".1.3.6.1.3.94.1.6.1.9", index_oidstr, RECMETHOD_NONE, 0);
  unit->uptime->summary_flag = 1;
  
  /* URL */
  unit->url = l_snmp_metric_create (self, obj, "url", "URL", METRIC_STRING, ".1.3.6.1.3.94.1.6.1.10", index_oidstr, RECMETHOD_NONE, 0);

  /* Proxy Master */
  unit->proxy_master = l_snmp_metric_create (self, obj, "proxy_master", "Proxy Master", METRIC_INTEGER, ".1.3.6.1.3.94.1.6.1.12", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (unit->proxy_master, 1, "Unknown");
  i_metric_enumstr_add (unit->proxy_master, 2, "No");
  i_metric_enumstr_add (unit->proxy_master, 3, "Yes");

  /* Principal */
  unit->principal = l_snmp_metric_create (self, obj, "principal", "Principal", METRIC_INTEGER, ".1.3.6.1.3.94.1.6.1.13", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (unit->proxy_master, 1, "Unknown");
  i_metric_enumstr_add (unit->proxy_master, 2, "No");
  i_metric_enumstr_add (unit->proxy_master, 3, "Yes");

  /* Enqueue the unit item */
  num = i_list_enqueue (cnt->item_list, unit);
  if (num != 0)
  { i_printf (1, "v_unit_objfact_fab failed to enqueue unit for object %s", obj->name_str); v_unit_item_free (unit); return -1; }

  /* Create port/sensor containers */
  unit->port_cnt = v_port_enable (self, unit);
  unit->sensor_cnt = v_sensor_enable (self, unit);

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_unit_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
{
  /* Check the result */
  if (result == SNMP_ERROR_NOERROR)
  {
    /* No errors, set item list state to NORMAL */
    cnt->item_list_state = ITEMLIST_STATE_NORMAL;
  }

  return 0;
}

/* Object Factory Clean Func
 *
 * Called when an object is obsolete prior to it being deregistered and free
 */

int v_unit_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the unit and remove
   * it from the item_list
   */

  int num;
  v_unit_item *unit = obj->itemptr;

  if (!unit) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, unit);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
