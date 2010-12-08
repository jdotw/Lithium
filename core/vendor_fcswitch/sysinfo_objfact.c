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

#include "sysinfo.h"

/* 
 * Cisco CPU Resources - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_sysinfo_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_sysinfo_item *sysinfo;

  /* Object Configuration */
  obj->desc_str = strdup("Master");
  obj->mainform_func = v_sysinfo_objform;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_sysinfo_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create sysinfo item struct */
  sysinfo = v_sysinfo_item_create ();
  if (!sysinfo)
  { i_printf (1, "v_sysinfo_objfact_fab failed to create sysinfo item for object %s", obj->name_str); return -1; }
  sysinfo->obj = obj;
  obj->itemptr = sysinfo;

  /* 
   * Metric Creation 
   */

  sysinfo->uptime = l_snmp_metric_create (self, obj, "uptime", "Uptime", METRIC_INTERVAL, ".1.3.6.1.3.94.1.6.1.9", index_oidstr, RECMETHOD_NONE, 0);
  sysinfo->descr = l_snmp_metric_create (self, obj, "descr", "Description", METRIC_STRING, ".1.3.6.1.3.94.1.6.1.7", index_oidstr, RECMETHOD_NONE, 0);
  sysinfo->contact = l_snmp_metric_create (self, obj, "contact", "Contact", METRIC_STRING, ".1.3.6.1.3.94.1.6.1.23", index_oidstr, RECMETHOD_NONE, 0);
  sysinfo->name = l_snmp_metric_create (self, obj, "name", "Name", METRIC_STRING, ".1.3.6.1.3.94.1.6.1.20", index_oidstr, RECMETHOD_NONE, 0);
  sysinfo->location = l_snmp_metric_create (self, obj, "location", "Location", METRIC_STRING, ".1.3.6.1.3.94.1.6.1.24", index_oidstr, RECMETHOD_NONE, 0);
  sysinfo->vendor = l_snmp_metric_create (self, obj, "vendor", "Vendor", METRIC_STRING, ".1.3.6.1.3.94.1.6.1.31", index_oidstr, RECMETHOD_NONE, 0);
  sysinfo->serial = l_snmp_metric_create (self, obj, "serial", "Serial", METRIC_STRING, ".1.3.6.1.3.94.1.6.1.8", index_oidstr, RECMETHOD_NONE, 0);
  sysinfo->product = l_snmp_metric_create (self, obj, "product", "Product", METRIC_STRING, ".1.3.6.1.3.94.1.6.1.7", index_oidstr, RECMETHOD_NONE, 0);
  sysinfo->type = l_snmp_metric_create (self, obj, "type", "Type", METRIC_INTEGER, ".1.3.6.1.3.94.1.6.1.3", index_oidstr, RECMETHOD_NONE, 0);

  sysinfo->state = l_snmp_metric_create (self, obj, "state", "State", METRIC_INTEGER, ".1.3.6.1.3.94.1.6.1.5", index_oidstr, RECMETHOD_NONE, 0);
  sysinfo->status = l_snmp_metric_create (self, obj, "status", "Status", METRIC_INTEGER, ".1.3.6.1.3.94.1.6.1.6", index_oidstr, RECMETHOD_NONE, 0);
  
  /* Enqueue the sysinfo item */
  num = i_list_enqueue (cnt->item_list, sysinfo);
  if (num != 0)
  { i_printf (1, "v_sysinfo_objfact_fab failed to enqueue sysinfo for object %s", obj->name_str); v_sysinfo_item_free (sysinfo); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_sysinfo_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_sysinfo_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the sysinfo and remove
   * it from the item_list
   */

  int num;
  v_sysinfo_item *sysinfo = obj->itemptr;

  if (!sysinfo) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, sysinfo);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
