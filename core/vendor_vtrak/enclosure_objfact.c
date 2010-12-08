#include <stdlib.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/timeutil.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/str.h"

#include "device/snmp.h"

#include "cooling.h"
#include "psu.h"
#include "voltage.h"
#include "temp.h"
#include "battery.h"
#include "enclosure.h"

/* 
 * Cisco Memory Pools - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_enclosure_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_enclosure_item *enclosure;

  /* Object Configuration */
  char *desc_index_str = l_snmp_get_string_from_pdu (pdu);
  asprintf (&obj->desc_str, "Enclosure %s", desc_index_str);
  free (desc_index_str);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_enclosure_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create enclosure item struct */
  enclosure = v_enclosure_item_create ();
  if (!enclosure)
  { i_printf (1, "v_enclosure_objfact_fab failed to create enclosure item for object %s", obj->name_str); return -1; }
  enclosure->obj = obj;
  obj->itemptr = enclosure;
  enclosure->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* Info */
  enclosure->type = l_snmp_metric_create (self, obj, "type", "Type", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.10.1.2", index_oidstr, RECMETHOD_NONE, 0);

  /* State */
  enclosure->opstate = l_snmp_metric_create (self, obj, "opstate", "Operational Status", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.10.1.3", index_oidstr, RECMETHOD_NONE, 0);
  enclosure->opstate_desc = l_snmp_metric_create (self, obj, "opstate_desc", "Status Description", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.10.1.4", index_oidstr, RECMETHOD_NONE, 0);

  /* Enqueue the enclosure item */
  num = i_list_enqueue (cnt->item_list, enclosure);
  if (num != 0)
  { i_printf (1, "v_enclosure_objfact_fab failed to enqueue enclosure for object %s", obj->name_str); v_enclosure_item_free (enclosure); return -1; }

  /* 
   * Enable enclosure-specific monitoring 
   */

  v_cooling_enable (self, enclosure->index, index_oidstr);
  v_psu_enable (self, enclosure->index, index_oidstr);
  v_temp_enable (self, enclosure->index, index_oidstr);
  v_voltage_enable (self, enclosure->index, index_oidstr);
  v_battery_enable (self, enclosure->index, index_oidstr);

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_enclosure_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_enclosure_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the enclosure and remove
   * it from the item_list
   */

  int num;
  v_enclosure_item *enclosure = obj->itemptr;

  if (!enclosure) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, enclosure);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
