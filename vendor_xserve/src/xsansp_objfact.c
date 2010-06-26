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
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/adminstate.h>
#include <induction/triggerset.h>
#include <induction/str.h>
#include <lithium/snmp.h>

#include "xsansp.h"

/* 
 * SNMP Storage Resources - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_xsansp_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_xsansp_item *sp;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_xsansp_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create storage item struct */
  sp = v_xsansp_item_create ();
  if (!sp)
  { i_printf (1, "v_xsansp_objfact_fab failed to create spume item for object %s", obj->name_str); return -1; }
  obj->itemptr = sp;
  sp->obj = obj;
  sp->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  sp->state = l_snmp_metric_create (self, obj, "state", "State", METRIC_INTEGER, ".1.3.6.1.4.1.20038.2.1.1.1.1.4", index_oidstr, RECMETHOD_NONE, 0);

  sp->fs_block_size = l_snmp_metric_create (self, obj, "fs_block_size", "Block Size", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.1.1.1.13", index_oidstr, RECMETHOD_NONE, 0);
  sp->fs_block_size->alloc_unit = 1024;
  sp->devices = l_snmp_metric_create (self, obj, "devices", "Disk Devices", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.1.1.1.15", index_oidstr, RECMETHOD_NONE, 0);
  sp->stripe_groups = l_snmp_metric_create (self, obj, "stripe_groups", "Stripe Groups", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.1.1.1.16", index_oidstr, RECMETHOD_NONE, 0);

  sp->bytes_total = l_snmp_metric_create (self, obj, "bytes_total", "Size", METRIC_COUNT64, ".1.3.6.1.4.1.20038.2.1.1.1.1.19", index_oidstr, RECMETHOD_NONE, 0);
  sp->bytes_total->alloc_unit_met = sp->fs_block_size;
  sp->bytes_total->valstr_func = i_string_spume_metric;
  sp->bytes_total->unit_str = strdup ("byte");
  sp->bytes_total->kbase = 1024;
  sp->bytes_total->summary_flag = 1;

  // 
  // sp->free = l_snmp_metric_create (self, obj, "free", "Free", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.4.1.1.6", index_oidstr, RECMETHOD_NONE, 0);
  // sp->free->alloc_unit = 1024 * 1024;
  // sp->free->valstr_func = i_string_spume_metric;
  // sp->free->unit_str = strdup ("byte");
  // sp->free->kbase = 1024;
  // 
  // sp->used_pc = l_snmp_metric_create (self, obj, "used_pc", "Used Percent", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.4.1.1.7", index_oidstr, RECMETHOD_RRD, 0);
  // sp->used_pc->record_defaultflag = 1;
  // sp->used_pc->unit_str = strdup ("%");

  
  /*
   * End Metric Creation
   */

  /* Enqueue the storage item */
  num = i_list_enqueue (cnt->item_list, sp);
  if (num != 0)
  { i_printf (1, "v_xsansp_objfact_fab failed to enqueue storage item for object %s", obj->name_str); v_xsansp_item_free (sp); return -1; }


  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_xsansp_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_xsansp_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  v_xsansp_item *sp = obj->itemptr;

  /* Remove from item list */
  num = i_list_search (cnt->item_list, sp);
  if (num == 0) 
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;

  return 0;
}
