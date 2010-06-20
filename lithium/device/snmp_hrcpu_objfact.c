#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/interface.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>

#include "snmp.h"
#include "snmp_hrcpu.h"

/* 
 * SNMP Host Processor Resources - Object Factory Functions 
 */

/* Object Factory Fabrication */

int l_snmp_hrcpu_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  l_snmp_hrcpu_item *proc;

  /* Object Configuration */
  if (cnt->item_list)
  { asprintf (&obj->desc_str, "CPU %i",  cnt->item_list->size+1); }
  else
  { asprintf (&obj->desc_str, "CPU 1"); }
  obj->mainform_func = l_snmp_hrcpu_objform;
  obj->histform_func = l_snmp_hrcpu_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "l_snmp_hrcpu_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create hrcpu item struct */
  proc = l_snmp_hrcpu_item_create ();
  if (!proc)
  { i_printf (1, "l_snmp_hrcpu_objfact_fab failed to create hrcpu item for object %s", obj->name_str); return -1; }
  obj->itemptr = proc;
  proc->obj = obj;
  proc->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* System Uptime */
  proc->load_pc = l_snmp_metric_create (self, obj, "load_pc", "Load Percent", METRIC_INTEGER, ".1.3.6.1.2.1.25.3.3.1.2", index_oidstr, RECMETHOD_RRD, 0);
  proc->load_pc->min_val = i_metric_value_create ();
  proc->load_pc->min_val->integer = 0;
  proc->load_pc->max_val = i_metric_value_create ();
  proc->load_pc->max_val->integer = 100;
  proc->load_pc->unit_str = strdup ("%");
  proc->load_pc->record_defaultflag = 1;
  
  /*
   * End Metric Creation
   */

  /* Enqueue the hrcpu item */
  num = i_list_enqueue (cnt->item_list, proc);
  if (num != 0)
  { i_printf (1, "l_snmp_hrcpu_objfact_fab failed to enqueue hrcpu item for object %s", obj->name_str); l_snmp_hrcpu_item_free (proc); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int l_snmp_hrcpu_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int l_snmp_hrcpu_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  l_snmp_hrcpu_item *proc = obj->itemptr;

  /* Remove from item list */
  num = i_list_search (cnt->item_list, proc);
  if (num == 0) 
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;

  return 0;
}
