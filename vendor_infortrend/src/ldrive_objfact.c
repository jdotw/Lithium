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

#include "index.h"
#include "ldrive.h"

/* 
 * Logical Drive - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_ldrive_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_ldrive_item *ldrive;

  /* Object Configuration */
  asprintf (&obj->desc_str, "Logical Drive %s", obj->name_str); 
  //obj->mainform_func = v_ldrive_objform;
  //obj->histform_func = v_ldrive_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_ldrive_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create ldrive item struct */
  ldrive = v_ldrive_item_create ();
  if (!ldrive)
  { i_printf (1, "v_ldrive_objfact_fab failed to create ldrive item for object %s", obj->name_str); return -1; }
  ldrive->obj = obj;
  obj->itemptr = ldrive;
  ldrive->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* Size (in blocks) */
  ldrive->size_blocks = l_snmp_metric_create (self, obj, "size_blocks", "Size (in blocks)", METRIC_GAUGE, ".1.3.6.1.4.1.1714.1.2.1.3", index_oidstr, RECMETHOD_NONE, 0);
  ldrive->size_blocks->unit_str = strdup ("blocks");

  /* Block Size Index (power of 2)*/
  ldrive->blocksize_index = l_snmp_metric_create (self, obj, "blocksize_index", "Block Size Index", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.2.1.4", index_oidstr, RECMETHOD_NONE, 0);
  ldrive->blocksize = i_metric_create ("blocksize", "Blocksize", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(ldrive->blocksize));
  i_entity_refreshcb_add (ENTITY(ldrive->blocksize_index), v_index_refcb, ldrive->blocksize);

  /* Size */
  ldrive->size = l_snmp_metric_create (self, obj, "size", "Size", METRIC_GAUGE, ".1.3.6.1.4.1.1714.1.2.1.3", index_oidstr, RECMETHOD_NONE, 0);
  ldrive->size->alloc_unit_met = ldrive->blocksize;
  ldrive->size->valstr_func = i_string_volume_metric;
  ldrive->size->unit_str = strdup ("byte");
  ldrive->size->kbase = 1024;

  /* Operating Mode */
  ldrive->mode = l_snmp_metric_create (self, obj, "mode", "Operating Mode", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.2.1.5", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (ldrive->mode, 0, "Single Drive");
  i_metric_enumstr_add (ldrive->mode, 1, "Non-RAID");
  i_metric_enumstr_add (ldrive->mode, 2, "RAID 0");
  i_metric_enumstr_add (ldrive->mode, 3, "RAID 1");
  i_metric_enumstr_add (ldrive->mode, 4, "RAID 3");
  i_metric_enumstr_add (ldrive->mode, 5, "RAID 4");
  i_metric_enumstr_add (ldrive->mode, 6, "RAID 5");
  i_metric_enumstr_add (ldrive->mode, 7, "RAID 6");

  /* Status */
  ldrive->status = l_snmp_metric_create (self, obj, "status", "Status", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.2.1.6", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (ldrive->status, 0, "Good");
  i_metric_enumstr_add (ldrive->status, 1, "Rebuilding");
  i_metric_enumstr_add (ldrive->status, 2, "Initializing");
  i_metric_enumstr_add (ldrive->status, 3, "Degraded");
  i_metric_enumstr_add (ldrive->status, 4, "Dead");
  i_metric_enumstr_add (ldrive->status, 5, "Invalid");
  i_metric_enumstr_add (ldrive->status, 6, "Incomplete");
  i_metric_enumstr_add (ldrive->status, 7, "Drive Missing");

  /* State */
  ldrive->state = l_snmp_metric_create (self, obj, "state", "State", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.2.1.7", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (ldrive->state, 0, "Normal");
  i_metric_enumstr_add (ldrive->state, 1, "Rebuilding Degraded");
  i_metric_enumstr_add (ldrive->state, 2, "Expanding");
  i_metric_enumstr_add (ldrive->state, 4, "Adding SCSI Drives");

  /* Total Drive Count */
  ldrive->drive_count = l_snmp_metric_create (self, obj, "drive_count", "Total Drive Count", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.2.1.8", index_oidstr, RECMETHOD_NONE, 0);

  /* Online Drive Count */
  ldrive->online_count = l_snmp_metric_create (self, obj, "online_count", "Online Drive Count", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.2.1.9", index_oidstr, RECMETHOD_NONE, 0);

  /* Space Drive Count */
  ldrive->spare_count = l_snmp_metric_create (self, obj, "spare_count", "Spare Drive Count", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.2.1.10", index_oidstr, RECMETHOD_NONE, 0);

  /* Failed Drive Count */
  ldrive->failed_count = l_snmp_metric_create (self, obj, "failed_count", "Failed Drive Count", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.2.1.11", index_oidstr, RECMETHOD_NONE, 0);

  /* Enqueue the ldrive item */
  num = i_list_enqueue (cnt->item_list, ldrive);
  if (num != 0)
  { i_printf (1, "v_ldrive_objfact_fab failed to enqueue ldrive for object %s", obj->name_str); v_ldrive_item_free (ldrive); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_ldrive_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_ldrive_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the ldrive and remove
   * it from the item_list
   */

  int num;
  v_ldrive_item *ldrive = obj->itemptr;

  if (!ldrive) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, ldrive);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
