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
#include "snmp_storage.h"
#include "snmp_hrfilesys.h"

/* 
 * SNMP Storage Resources - Object Factory Functions 
 */

/* Object Factory Fabrication */

int l_snmp_hrfilesys_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);

  /* Find related SNMP hrStorage object */
  i_container *store_cnt = (i_container *) i_entity_child_get (ENTITY(cnt->dev), "storage");
  i_object *store_obj;
  l_snmp_storage_item *store_item = NULL;
  for (i_list_move_head(store_cnt->obj_list); (store_obj=i_list_restore(store_cnt->obj_list))!=NULL; i_list_move_next(store_cnt->obj_list))
  {
    store_item = (l_snmp_storage_item *) store_obj->itemptr;
    if (store_item && store_item->index == (unsigned long) *pdu->variables->val.integer)
    { 
      /* Match found! */
      break; 
    }
  }
  if (!store_obj || !store_item)
  {
    /* No matching store found, discard */
    return -1;
  }
  obj->itemptr = store_item;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "l_snmp_hrfilesys_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* 
   * Metric Creation 
   */

  /* Remote Mount-Point */
  store_item->remote_mp = l_snmp_metric_create (self, store_obj, "remote_mp", "Remote Mount-Point", METRIC_STRING, ".1.3.6.1.2.1.25.3.8.1.3", index_oidstr, RECMETHOD_NONE, 0);

  /* Access */
  store_item->access = l_snmp_metric_create (self, store_obj, "access", "Access", METRIC_INTEGER, ".1.3.6.1.2.1.25.3.8.1.5", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (store_item->access, 1, "Read/Write");
  i_metric_enumstr_add (store_item->access, 2, "Read Only");
  store_item->access->summary_flag = 1;
  i_entity_refreshcb_add (ENTITY(store_item->access), l_snmp_hrfilesys_access_refcb, NULL);

  /* Bootable */
  store_item->bootable = l_snmp_metric_create (self, store_obj, "bootable", "Bootable", METRIC_INTEGER, ".1.3.6.1.2.1.25.3.8.1.6", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (store_item->bootable, 1, "Yes");
  i_metric_enumstr_add (store_item->bootable, 2, "No");

  /*
   * End Metric Creation
   */

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int l_snmp_hrfilesys_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int l_snmp_hrfilesys_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{

  return 0;
}
