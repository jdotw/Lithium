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

#include "expmodule.h"

/* 
 * 3Com expmodules - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_expmodule_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_expmodule_item *expmodule;
 
  /* Make sure this object is a expmodule */
  if (pdu->variables->name[pdu->variables->name_length-2] != 6) return -1;
  
  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = v_expmodule_objform;
  obj->histform_func = v_expmodule_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_expmodule_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create expmodule item struct */
  expmodule = v_expmodule_item_create ();
  if (!expmodule)
  { i_printf (1, "v_expmodule_objfact_fab failed to create expmodule item for object %s", obj->name_str); return -1; }
  expmodule->obj = obj;
  obj->itemptr = expmodule;
  expmodule->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */
  /* expmodule Type */
  expmodule->ent_type = l_snmp_metric_create (self, obj, "ent_type", "expmodule Type", METRIC_INTEGER, "enterprises.43.43.1.1.4.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (expmodule->ent_type,1,"Not Present");
  i_metric_enumstr_add (expmodule->ent_type,2,"Unknown");
  i_metric_enumstr_add (expmodule->ent_type,3,"a3c17710");
  i_metric_enumstr_add (expmodule->ent_type,4,"a3c17711");
  i_metric_enumstr_add (expmodule->ent_type,5,"a3c17712");
  i_metric_enumstr_add (expmodule->ent_type,6,"a3c17714");
  i_metric_enumstr_add (expmodule->ent_type,7,"a3c17716");
  i_metric_enumstr_add (expmodule->ent_type,8,"a3c17814");
  i_metric_enumstr_add (expmodule->ent_type,9,"a3c17816");
  i_metric_enumstr_add (expmodule->ent_type,10,"a3c17818");
  i_metric_enumstr_add (expmodule->ent_type,11,"a3c17220");
  i_metric_enumstr_add (expmodule->ent_type,12,"a3c17221");
  i_metric_enumstr_add (expmodule->ent_type,13,"a3c17222");
  i_metric_enumstr_add (expmodule->ent_type,14,"a3c17223");
  i_metric_enumstr_add (expmodule->ent_type,15,"a3c17224");
  i_metric_enumstr_add (expmodule->ent_type,16,"a3c17226");
  
  /* Hardware Version */
  expmodule->hw_ver = l_snmp_metric_create (self, obj, "hw_ver", "Hardware Version", METRIC_STRING, "enterprises.43.43.1.1.5.1", index_oidstr, RECMETHOD_NONE, 0);

  /* Software Version */
  expmodule->sw_ver = l_snmp_metric_create (self, obj, "sw_ver", "Software Version", METRIC_STRING, "enterprises.43.43.1.1.6.1", index_oidstr, RECMETHOD_NONE, 0);

  /* expmodule State */
  expmodule->state = l_snmp_metric_create (self, obj, "state", "Current State", METRIC_INTEGER, "enterprises.43.43.1.1.8.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (expmodule->state,1,"Unknown");
  i_metric_enumstr_add (expmodule->state,2,"Initialisingn");
  i_metric_enumstr_add (expmodule->state,3,"Idle");
  i_metric_enumstr_add (expmodule->state,4,"Operational");
  i_metric_enumstr_add (expmodule->state,5,"Failure");
  i_metric_enumstr_add (expmodule->state,6,"Not Present");
  

  /* Current Fault */
  expmodule->fault = l_snmp_metric_create (self, obj, "fault", "Fault", METRIC_INTEGER, "enterprises.43.43.1.1.9.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (expmodule->fault,0,"No Error");
  i_metric_enumstr_add (expmodule->fault,1,"Self Test Failed");
  i_metric_enumstr_add (expmodule->fault,2,"Unsupported Card");
  i_metric_enumstr_add (expmodule->fault,3,"Power up Failed");
  i_metric_enumstr_add (expmodule->fault,4,"Temperature Critical");
  i_metric_enumstr_add (expmodule->fault,5,"Temperature too high. Card Shut down");
  i_metric_enumstr_add (expmodule->fault,6,"Reset required");
  i_metric_enumstr_add (expmodule->fault,7,"Unspecified Failure");
  i_metric_enumstr_add (expmodule->fault,8,"Not in use");
  
  /* Enqueue the expmodule item */
  num = i_list_enqueue (cnt->item_list, expmodule);
  if (num != 0)
  { i_printf (1, "v_expmodule_objfact_fab failed to enqueue expmodule for object %s", obj->name_str); v_expmodule_item_free (expmodule); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_expmodule_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_expmodule_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the mempool and remove
   * it from the item_list
   */

  int num;
  v_expmodule_item *expmodule = obj->itemptr;

  if (!expmodule) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, expmodule);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
