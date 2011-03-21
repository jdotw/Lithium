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

/* 
 * Cisco Memory Pools - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_cooling_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_cooling_item *cooling;

  /* Object Configuration */
  char *desc_index_str = l_snmp_get_string_from_pdu (pdu);
  asprintf (&obj->desc_str, "Cooling Device %s", desc_index_str);
  free (desc_index_str);

  /* Adjust index_oidstr */
  asprintf (&index_oidstr, "%i.%i.%s", 
    (int) pdu->variables->name[pdu->variables->name_length-3],
    (int) pdu->variables->name[pdu->variables->name_length-2],
    index_oidstr);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_cooling_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create cooling item struct */
  cooling = v_cooling_item_create ();
  if (!cooling)
  { i_printf (1, "v_cooling_objfact_fab failed to create cooling item for object %s", obj->name_str); return -1; }
  cooling->obj = obj;
  obj->itemptr = cooling;
  cooling->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* Info */
  cooling->type = l_snmp_metric_create (self, obj, "type", "Type", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.1.11.1.2", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (cooling->type, 1, "Blower");
  i_metric_enumstr_add (cooling->type, 0, "Fan");

  /* State */
  cooling->opstate = l_snmp_metric_create (self, obj, "opstate", "Operational Status", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.11.1.3", index_oidstr, RECMETHOD_NONE, 0);
  cooling->speed = l_snmp_metric_create (self, obj, "speed", "speed", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.11.1.4", index_oidstr, RECMETHOD_RRD, 0);
  cooling->speed->record_defaultflag = 1;

  /* Enqueue the cooling item */
  num = i_list_enqueue (cnt->item_list, cooling);
  if (num != 0)
  { i_printf (1, "v_cooling_objfact_fab failed to enqueue cooling for object %s", obj->name_str); v_cooling_item_free (cooling); return -1; }

  /* Free our custom indexoid */
  free (index_oidstr);

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_cooling_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_cooling_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the cooling and remove
   * it from the item_list
   */

  int num;
  v_cooling_item *cooling = obj->itemptr;

  if (!cooling) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, cooling);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
