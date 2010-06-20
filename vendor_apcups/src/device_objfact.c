#include <stdlib.h>

#include <induction.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>
#include <induction/hierarchy.h>
#include <induction/list.h>

#include <lithium/snmp.h>

#include "device.h"

/* 
 * UPS Devices - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_device_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_device_item *device;

  /* Object Configuration */
  char *id_str = l_snmp_get_string_from_pdu (pdu);
  asprintf (&obj->desc_str, "Device %s", id_str);
  //obj->mainform_func = v_device_objform;
  //obj->histform_func = v_device_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_device_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create device item struct */
  device = v_device_item_create ();
  if (!device)
  { i_printf (1, "v_device_objfact_fab failed to create device item for object %s", obj->name_str); return -1; }
  device->obj = obj;
  obj->itemptr = device;

  /* 
   * Metric Creation 
   */

  /* Five Sec */
  device->rating = l_snmp_metric_create (self, obj, "rating", "Rating", METRIC_GAUGE, ".1.3.6.1.4.1.318.1.1.1.5.1.2.1.3", index_oidstr, RECMETHOD_RRD, 0);
  device->rating->record_defaultflag = 1;
  device->rating->unit_str = strdup ("VA");

  /* Enqueue the device item */
  num = i_list_enqueue (cnt->item_list, device);
  if (num != 0)
  { i_printf (1, "v_device_objfact_fab failed to enqueue device for object %s", obj->name_str); v_device_item_free (device); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_device_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_device_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the device and remove
   * it from the item_list
   */

  int num;
  v_device_item *device = obj->itemptr;

  if (!device) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, device);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
