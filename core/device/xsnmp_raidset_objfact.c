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
#include "xsnmp_raidset.h"

/* 
 * Xsnmp RAID Sets
 */

/* Object Factory Fabrication */

int l_xsnmp_raidset_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  l_xsnmp_raidset_item *set;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "l_xsnmp_raidset_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create hrcpu item struct */
  set = l_xsnmp_raidset_item_create ();
  if (!set)
  { i_printf (1, "l_xsnmp_raidset_objfact_fab failed to create hrcpu item for object %s", obj->name_str); return -1; }
  obj->itemptr = set;
  set->obj = obj;
  set->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  set->type = l_snmp_metric_create (self, obj, "type", "Type", METRIC_STRING, ".1.3.6.1.4.1.20038.2.1.6.2.1.3", index_oidstr, RECMETHOD_NONE, 0);
  set->size = l_snmp_metric_create (self, obj, "size", "Size", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.6.2.1.4", index_oidstr, RECMETHOD_NONE, 0);
  set->size->alloc_unit = (1024 * 1024);
  set->size->unit_str = strdup("byte");
  set->unused = l_snmp_metric_create (self, obj, "unused", "Unused", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.6.2.1.5", index_oidstr, RECMETHOD_NONE, 0);
  set->unused->alloc_unit = (1024 * 1024);
  set->unused->unit_str = strdup("byte");
  set->comments = l_snmp_metric_create (self, obj, "comments", "Comments", METRIC_STRING, ".1.3.6.1.4.1.20038.2.1.6.2.1.6", index_oidstr, RECMETHOD_NONE, 0);
  
  /*
   * End Metric Creation
   */

  /* Enqueue the hrcpu item */
  num = i_list_enqueue (cnt->item_list, set);
  if (num != 0)
  { i_printf (1, "l_xsnmp_raidset_objfact_fab failed to enqueue hrcpu item for object %s", obj->name_str); l_xsnmp_raidset_item_free (set); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int l_xsnmp_raidset_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int l_xsnmp_raidset_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  l_xsnmp_raidset_item *set = obj->itemptr;

  /* Remove from item list */
  num = i_list_search (cnt->item_list, set);
  if (num == 0) 
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;

  return 0;
}
