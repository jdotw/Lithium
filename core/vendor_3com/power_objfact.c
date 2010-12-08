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

#include "power.h"

/* 
 * 3Com powers - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_power_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_power_item *power;
 
  /* Make sure this object is a power */
  if (pdu->variables->name[pdu->variables->name_length-2] != 4) return -1;
  
  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = v_power_objform;
  obj->histform_func = v_power_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_power_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create power item struct */
  power = v_power_item_create ();
  if (!power)
  { i_printf (1, "v_power_objfact_fab failed to create power item for object %s", obj->name_str); return -1; }
  power->obj = obj;
  obj->itemptr = power;
  power->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */
  /* power Type */
  power->ent_type = l_snmp_metric_create (self, obj, "ent_type", "power Type", METRIC_INTEGER, "enterprises.43.43.1.1.4.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (power->ent_type,1,"Not Present");
  i_metric_enumstr_add (power->ent_type,2,"Input AC Connector");
  i_metric_enumstr_add (power->ent_type,3,"Type 1 Supply");
  i_metric_enumstr_add (power->ent_type,4,"Type 2 Supply");
  i_metric_enumstr_add (power->ent_type,5,"Type 3 Supply");
  i_metric_enumstr_add (power->ent_type,6,"Type 4 Supply");
  i_metric_enumstr_add (power->ent_type,7,"Type 5 Supply");
  i_metric_enumstr_add (power->ent_type,8,"Type 6 Supply");
  i_metric_enumstr_add (power->ent_type,9,"Type 7 Supply");
  i_metric_enumstr_add (power->ent_type,10,"Type 8 Supply");
  i_metric_enumstr_add (power->ent_type,11,"Type 9 Supply");
  i_metric_enumstr_add (power->ent_type,12,"Type 0 Supply");
  i_metric_enumstr_add (power->ent_type,13,"Type 11 Supply");
  i_metric_enumstr_add (power->ent_type,14,"Type 12 Supply");
  i_metric_enumstr_add (power->ent_type,15,"Type 13 Supply");
  i_metric_enumstr_add (power->ent_type,16,"Type 14 Supply");
  i_metric_enumstr_add (power->ent_type,17,"Type 15 Supply");
  i_metric_enumstr_add (power->ent_type,18,"Type 16 Supply");
    
  /* Hardware Version */
  power->hw_ver = l_snmp_metric_create (self, obj, "hw_ver", "Hardware Version", METRIC_STRING, "enterprises.43.43.1.1.5.1", index_oidstr, RECMETHOD_NONE, 0);

  /* Software Version */
  power->sw_ver = l_snmp_metric_create (self, obj, "sw_ver", "Software Version", METRIC_STRING, "enterprises.43.43.1.1.6.1", index_oidstr, RECMETHOD_NONE, 0);

  /* power State */
  power->state = l_snmp_metric_create (self, obj, "state", "Current State", METRIC_INTEGER, "enterprises.43.43.1.1.8.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (power->state,1,"Unknown");
  i_metric_enumstr_add (power->state,2,"Initialisingn");
  i_metric_enumstr_add (power->state,3,"Idle");
  i_metric_enumstr_add (power->state,4,"Operational");
  i_metric_enumstr_add (power->state,5,"Failure");
  i_metric_enumstr_add (power->state,6,"Not Present");
  

  /* Current Fault */
  power->fault = l_snmp_metric_create (self, obj, "fault", "Fault", METRIC_INTEGER, "enterprises.43.43.1.1.9.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (power->fault,0,"No Error");
  i_metric_enumstr_add (power->fault,1,"Power Supply not working");
  i_metric_enumstr_add (power->fault,2,"Power Supply Temperature Critical");
  i_metric_enumstr_add (power->fault,3,"Power Supply Temperature Too High");
  
  /* Enqueue the power item */
  num = i_list_enqueue (cnt->item_list, power);
  if (num != 0)
  { i_printf (1, "v_power_objfact_fab failed to enqueue power for object %s", obj->name_str); v_power_item_free (power); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_power_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_power_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the mempool and remove
   * it from the item_list
   */

  int num;
  v_power_item *power = obj->itemptr;

  if (!power) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, power);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
