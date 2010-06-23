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

#include "snmp.h"
#include "snmp_storage.h"
#include "snmp_hrfilesys.h"
#include "snmp_nsram.h"
#include "record.h"

/* 
 * SNMP Storage Resources - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_xsanvol_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_xsanvol_item *vol;
  i_entity_refresh_config refconfig;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_xsanvol_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create storage item struct */
  vol = v_xsanvol_item_create ();
  if (!vol)
  { i_printf (1, "v_xsanvol_objfact_fab failed to create volume item for object %s", obj->name_str); return -1; }
  obj->itemptr = vol;
  vol->obj = obj;
  vol->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  vol->writeable = l_snmp_metric_create (self, obj, "state", "State", METRIC_INTEGER, ".1.3.6.1.4.1.20038.2.1.1.1.1.4", index_oidstr, RECMETHOD_NONE, 0);

  vol->fs_block_size = l_snmp_metric_create (self, obj, "fs_block_size", "Block Size", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.1.1.1.13", index_oidstr, RECMETHOD_NONE, 0);
  vol->fs_block_size->alloc_unit = 1024;
  vol->devices = l_snmp_metric_create (self, obj, "devices", "Disk Devices", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.1.1.1.15", index_oidstr, RECMETHOD_NONE, 0);
  vol->stripe_groups = l_snmp_metric_create (self, obj, "stripe_groups", "Stripe Groups", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.1.1.1.16", index_oidstr, RECMETHOD_NONE, 0);

  vol->size = l_snmp_metric_create (self, obj, "size", "Size", METRIC_COUNTER64, ".1.3.6.1.4.1.20038.2.1.1.1.1.19", index_oidstr, RECMETHOD_NONE, 0);
  vol->size->alloc_unit_met = vol->fs_block_size;
  vol->size->valstr_func = i_string_volume_metric;
  vol->size->unit_str = strdup ("byte");
  vol->size->kbase = 1024;
  vol->size->summary_flag = 1;

  // 
  // vol->free = l_snmp_metric_create (self, obj, "free", "Free", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.4.1.1.6", index_oidstr, RECMETHOD_NONE, 0);
  // vol->free->alloc_unit = 1024 * 1024;
  // vol->free->valstr_func = i_string_volume_metric;
  // vol->free->unit_str = strdup ("byte");
  // vol->free->kbase = 1024;
  // 
  // vol->used_pc = l_snmp_metric_create (self, obj, "used_pc", "Used Percent", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.4.1.1.7", index_oidstr, RECMETHOD_RRD, 0);
  // vol->used_pc->record_defaultflag = 1;
  // vol->used_pc->unit_str = strdup ("%");

  
  /*
   * End Metric Creation
   */

  /* Enqueue the storage item */
  num = i_list_enqueue (cnt->item_list, vol);
  if (num != 0)
  { i_printf (1, "v_xsanvol_objfact_fab failed to enqueue storage item for object %s", obj->name_str); v_xsanvol_item_free (vol); return -1; }


  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_xsanvol_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_xsanvol_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  v_xsanvol_item *vol = obj->itemptr;

  /* Remove from item list */
  num = i_list_search (cnt->item_list, vol);
  if (num == 0) 
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;

  return 0;
}
