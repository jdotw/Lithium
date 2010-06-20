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

#include "cpu.h"

/* 
 * Cisco CPU Resources - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_cpu_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_cpu_item *cpu;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = v_cpu_objform;
  obj->histform_func = v_cpu_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_cpu_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create cpu item struct */
  cpu = v_cpu_item_create ();
  if (!cpu)
  { i_printf (1, "v_cpu_objfact_fab failed to create cpu item for object %s", obj->name_str); return -1; }
  cpu->obj = obj;
  obj->itemptr = cpu;
  cpu->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* Utilisation */
  cpu->util_pc = l_snmp_metric_create (self, obj, "util_pc", "Utilisation", METRIC_GAUGE, "enterprises.11.2.14.11.5.1.9.6.1.0", index_oidstr, RECMETHOD_RRD, 0);
  cpu->util_pc->record_defaultflag = 1;
  cpu->util_pc->min_val = i_metric_value_create ();
  cpu->util_pc->min_val->gauge = 0;
  cpu->util_pc->max_val = i_metric_value_create ();
  cpu->util_pc->max_val->gauge = 100;
  cpu->util_pc->unit_str = strdup ("%");
            
  
  /* Enqueue the cpu item */
  num = i_list_enqueue (cnt->item_list, cpu);
  if (num != 0)
  { i_printf (1, "v_cpu_objfact_fab failed to enqueue cpu for object %s", obj->name_str); v_cpu_item_free (cpu); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_cpu_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_cpu_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the cpu and remove
   * it from the item_list
   */

  int num;
  v_cpu_item *cpu = obj->itemptr;

  if (!cpu) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, cpu);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
