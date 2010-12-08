#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/interface.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/adminstate.h>
#include <induction/triggerset.h>
#include <induction/str.h>
#include <induction/name.h>
#include "device/snmp.h"

#include "xsanvol.h"
#include "xsanvisdisk.h"

/* 
 * SNMP Storage Resources - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_xsanvisdisk_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_xsanvisdisk_item *visdisk;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_xsanvisdisk_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create storage item struct */
  visdisk = v_xsanvisdisk_item_create ();
  if (!visdisk)
  { i_printf (1, "v_xsanvisdisk_objfact_fab failed to create visdisk item for object %s", obj->name_str); return -1; }
  obj->itemptr = visdisk;
  visdisk->obj = obj;
  visdisk->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];
  
  /* 
   * Metric Creation 
   */

  
  /* Enqueue the storage item */
  num = i_list_enqueue (cnt->item_list, visdisk);
  if (num != 0)
  { i_printf (1, "v_xsanvisdisk_objfact_fab failed to enqueue storage item for object %s", obj->name_str); v_xsanvisdisk_item_free (visdisk); return -1; }


  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_xsanvisdisk_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_xsanvisdisk_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  v_xsanvisdisk_item *visdisk = obj->itemptr;
  
  /* Remove from item list */
  num = i_list_search (cnt->item_list, visdisk);
  if (num == 0) 
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;

  return 0;
}
