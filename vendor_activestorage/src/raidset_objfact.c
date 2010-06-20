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

#include "raidset.h"

/* 
 * raidset Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_raidset_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_raidset_item *raidset;

  /* Object setup */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_raidset_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create raidset item struct */
  raidset = v_raidset_item_create ();
  raidset->obj = obj;
  obj->itemptr = raidset;
  raidset->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* State */
  raidset->status = l_snmp_metric_create (self, obj, "status", "Status", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.8.1.1.2", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (raidset->status, -1, "None");
  i_metric_enumstr_add (raidset->status, 0, "Initializing");
  i_metric_enumstr_add (raidset->status, 1, "Optimal");
  i_metric_enumstr_add (raidset->status, 2, "Maintenance");
  i_metric_enumstr_add (raidset->status, 3, "Array Init.");
  i_metric_enumstr_add (raidset->status, 4, "Unstable");
  i_metric_enumstr_add (raidset->status, 5, "Init. Faulty");
  i_metric_enumstr_add (raidset->status, 65535, "Invalid");

  /* Size */
  raidset->size = l_snmp_metric_create (self, obj, "size", "Size", METRIC_GAUGE, ".1.3.6.1.4.1.31165.1.1.8.1.1.6", index_oidstr, RECMETHOD_NONE, 0);
  raidset->size->summary_flag = 1;
  raidset->size->unit_str = strdup ("MB");

  /* Drive counts */
  raidset->drivecount_orig = l_snmp_metric_create (self, obj, "drivecount_orig", "Original Drive Count", METRIC_GAUGE, ".1.3.6.1.4.1.31165.1.1.8.1.1.3", index_oidstr, RECMETHOD_NONE, 0);
  raidset->drivecount_orig->summary_flag = 1;
  raidset->drivecount_cur = l_snmp_metric_create (self, obj, "drivecount_cur", "Current Drive Count", METRIC_GAUGE, ".1.3.6.1.4.1.31165.1.1.8.1.1.4", index_oidstr, RECMETHOD_NONE, 0);
  raidset->drivecount_cur->summary_flag = 1;
  raidset->drivecount_spare = l_snmp_metric_create (self, obj, "drivecount_spare", "Spare Drive Count", METRIC_GAUGE, ".1.3.6.1.4.1.31165.1.1.8.1.1.5", index_oidstr, RECMETHOD_NONE, 0);
  raidset->drivecount_spare->summary_flag = 1;

  /* Enqueue the raidset item */
  num = i_list_enqueue (cnt->item_list, raidset);
  if (num != 0)
  { i_printf (1, "v_raidset_objfact_fab failed to enqueue raidset for object %s", obj->name_str); v_raidset_item_free (raidset); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_raidset_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_raidset_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the raidset and remove
   * it from the item_list
   */

  int num;
  v_raidset_item *raidset = obj->itemptr;

  if (!raidset) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, raidset);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
