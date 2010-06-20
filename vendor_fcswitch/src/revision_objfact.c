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

#include "revision.h"

/* 
 * Cisco CPU Resources - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_revision_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_revision_item *revision;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
//  obj->mainform_func = v_revision_objform;
//  obj->histform_func = v_revision_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_revision_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create revision item struct */
  revision = v_revision_item_create ();
  if (!revision)
  { i_printf (1, "v_revision_objfact_fab failed to create revision item for object %s", obj->name_str); return -1; }
  revision->obj = obj;
  obj->itemptr = revision;

  /* 
   * Metric Creation 
   */

  revision->version = l_snmp_metric_create (self, obj, "version", "Version", METRIC_STRING, ".1.3.6.1.3.94.1.7.1.3", index_oidstr, RECMETHOD_NONE, 0);
  
  /* Enqueue the revision item */
  num = i_list_enqueue (cnt->item_list, revision);
  if (num != 0)
  { i_printf (1, "v_revision_objfact_fab failed to enqueue revision for object %s", obj->name_str); v_revision_item_free (revision); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_revision_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_revision_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the revision and remove
   * it from the item_list
   */

  int num;
  v_revision_item *revision = obj->itemptr;

  if (!revision) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, revision);
  if (num == 0)
  { i_list_delete (cnt->item_list); }

  obj->itemptr = NULL;
  
  return 0;
}
