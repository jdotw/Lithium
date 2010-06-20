#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>

#include <lithium/snmp.h>

#include "fantray.h"

/* 
 * 3Com fantrays - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_fantray_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_fantray_item *fantray;
 
  /* Make sure this object is a fantray */
  if (pdu->variables->name[pdu->variables->name_length-2] != 5) return -1;
  
  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = v_fantray_objform;
  obj->histform_func = v_fantray_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_fantray_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create fantray item struct */
  fantray = v_fantray_item_create ();
  if (!fantray)
  { i_printf (1, "v_fantray_objfact_fab failed to create fantray item for object %s", obj->name_str); return -1; }
  fantray->obj = obj;
  obj->itemptr = fantray;
  fantray->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */
  /* fantray Type */
  fantray->ent_type = l_snmp_metric_create (self, obj, "ent_type", "Number of Fans", METRIC_INTEGER, "enterprises.43.43.1.1.4.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fantray->ent_type,1,"Not Present");
  i_metric_enumstr_add (fantray->ent_type,2,"One Fan");
  i_metric_enumstr_add (fantray->ent_type,3,"Two Fans");
  i_metric_enumstr_add (fantray->ent_type,4,"Three Fans");
  i_metric_enumstr_add (fantray->ent_type,5,"Four Fans");
  i_metric_enumstr_add (fantray->ent_type,6,"Five Fans");
    
  /* Hardware Version */
  fantray->hw_ver = l_snmp_metric_create (self, obj, "hw_ver", "Hardware Version", METRIC_STRING, "enterprises.43.43.1.1.5.1", index_oidstr, RECMETHOD_NONE, 0);

  /* Software Version */
  fantray->sw_ver = l_snmp_metric_create (self, obj, "sw_ver", "Software Version", METRIC_STRING, "enterprises.43.43.1.1.6.1", index_oidstr, RECMETHOD_NONE, 0);

  /* fantray State */
  fantray->state = l_snmp_metric_create (self, obj, "state", "Current State", METRIC_INTEGER, "enterprises.43.43.1.1.8.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fantray->state,1,"Unknown");
  i_metric_enumstr_add (fantray->state,2,"Initialisingn");
  i_metric_enumstr_add (fantray->state,3,"Idle");
  i_metric_enumstr_add (fantray->state,4,"Operational");
  i_metric_enumstr_add (fantray->state,5,"Failure");
  i_metric_enumstr_add (fantray->state,6,"Not Present");
  

  /* Current Fault */
  fantray->fault = l_snmp_metric_create (self, obj, "fault", "Fault", METRIC_INTEGER, "enterprises.43.43.1.1.9.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fantray->fault,0,"No Error");
  i_metric_enumstr_add (fantray->fault,1,"One Fan Failed");
  i_metric_enumstr_add (fantray->fault,2,"Two Fans Failed");
  i_metric_enumstr_add (fantray->fault,3,"Three Fans Failed");
  i_metric_enumstr_add (fantray->fault,4,"Four Fans Failed");
  i_metric_enumstr_add (fantray->fault,5,"Five Fans Failed");
  i_metric_enumstr_add (fantray->fault,99,"Indeterminate number of Fans Failed");
    
    /* Enqueue the fantray item */
  num = i_list_enqueue (cnt->item_list, fantray);
  if (num != 0)
  { i_printf (1, "v_fantray_objfact_fab failed to enqueue fantray for object %s", obj->name_str); v_fantray_item_free (fantray); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_fantray_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_fantray_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the mempool and remove
   * it from the item_list
   */

  int num;
  v_fantray_item *fantray = obj->itemptr;

  if (!fantray) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, fantray);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
