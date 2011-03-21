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

#include "mempool.h"

/* 
 * Cisco Memory Pools - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_mempool_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_mempool_item *mempool;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = v_mempool_objform;
  obj->histform_func = v_mempool_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_mempool_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create mempool item struct */
  mempool = v_mempool_item_create ();
  if (!mempool)
  { i_printf (1, "v_mempool_objfact_fab failed to create mempool item for object %s", obj->name_str); return -1; }
  mempool->obj = obj;
  obj->itemptr = mempool;
  mempool->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* Used */
  mempool->used = l_snmp_metric_create (self, obj, "used", "Used", METRIC_GAUGE, "enterprises.9.9.48.1.1.1.5", index_oidstr, RECMETHOD_NONE, 0);
  mempool->used->unit_str = strdup ("byte");
  mempool->used->valstr_func = i_string_volume_metric;

  /* Free */
  mempool->free = l_snmp_metric_create (self, obj, "free", "Free", METRIC_GAUGE, "enterprises.9.9.48.1.1.1.6", index_oidstr, RECMETHOD_NONE, 0);
  mempool->free->unit_str = strdup ("byte");
  mempool->free->valstr_func = i_string_volume_metric;
  
  /* Largest Free */
  mempool->largest_free = l_snmp_metric_create (self, obj, "largest_free", "Largest Free", METRIC_GAUGE, "enterprises.9.9.48.1.1.1.7", index_oidstr, RECMETHOD_NONE, 0);
  mempool->largest_free->unit_str = strdup ("byte");
  mempool->largest_free->valstr_func = i_string_volume_metric;

  /* Size */
  mempool->size = i_metric_acsum_create (self, obj, "size", "Size", METRIC_GAUGE, RECMETHOD_NONE, mempool->used, mempool->free, ACSUM_REFCB_YMET);
  mempool->size->unit_str = strdup ("byte");
  mempool->size->valstr_func = i_string_volume_metric;

  /* Used Percent */
  mempool->used_pc = i_metric_acpcent_create (self, obj, "used_pc", "Used Percent", RECMETHOD_RRD, mempool->used, mempool->size, ACPCENT_REFCB_MAX);
  mempool->used_pc->record_defaultflag = 1;

  /* Contig Free Percent */
  mempool->contig_free_pc = i_metric_acpcent_create (self, obj, "contig_free_pc", "Contiguous Free Percent", RECMETHOD_NONE, mempool->largest_free, mempool->size, ACPCENT_REFCB_GAUGE);

  /* Enqueue the mempool item */
  num = i_list_enqueue (cnt->item_list, mempool);
  if (num != 0)
  { i_printf (1, "v_mempool_objfact_fab failed to enqueue mempool for object %s", obj->name_str); v_mempool_item_free (mempool); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_mempool_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_mempool_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the mempool and remove
   * it from the item_list
   */

  int num;
  v_mempool_item *mempool = obj->itemptr;

  if (!mempool) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, mempool);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
