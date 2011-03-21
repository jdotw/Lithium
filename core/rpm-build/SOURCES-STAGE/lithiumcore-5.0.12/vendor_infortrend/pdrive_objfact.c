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

#include "index.h"
#include "pdrive.h"

/* 
 * Physical Drive - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_pdrive_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_pdrive_item *pdrive;

  /* Object Configuration */
  asprintf (&obj->desc_str, "Drive Slot %s", obj->name_str); 
  //obj->mainform_func = v_pdrive_objform;
  //obj->histform_func = v_pdrive_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_pdrive_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create pdrive item struct */
  pdrive = v_pdrive_item_create ();
  if (!pdrive)
  { i_printf (1, "v_pdrive_objfact_fab failed to create pdrive item for object %s", obj->name_str); return -1; }
  pdrive->obj = obj;
  obj->itemptr = pdrive;

  /* 
   * Metric Creation 
   */

  /* Logical Channel */
  pdrive->logical_channel = l_snmp_metric_create (self, obj, "logical_channel", "Logical Channel", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.6.1.2", index_oidstr, RECMETHOD_NONE, 0);
  
  /* Physical Channel */
  pdrive->physical_channel = l_snmp_metric_create (self, obj, "physical_channel", "Physical Channel", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.6.1.3", index_oidstr, RECMETHOD_NONE, 0);
  
  /* SCSI ID */
  pdrive->scsi_id = l_snmp_metric_create (self, obj, "scsi_id", "SCSI ID", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.6.1.4", index_oidstr, RECMETHOD_NONE, 0);
  
  /* SCSI LUN */
  pdrive->scsi_lun = l_snmp_metric_create (self, obj, "scsi_lun", "SCSI LUN", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.6.1.5", index_oidstr, RECMETHOD_NONE, 0);
  
  /* Logical Disk ID */
  pdrive->ldrive_id = l_snmp_metric_create (self, obj, "ldrive_id", "Member of Logical Drive ID", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.6.1.6", index_oidstr, RECMETHOD_NONE, 0);
  
  /* Size (in blocks) */
  pdrive->size_blocks = l_snmp_metric_create (self, obj, "size_blocks", "Size (in blocks)", METRIC_GAUGE, ".1.3.6.1.4.1.1714.1.6.1.7", index_oidstr, RECMETHOD_NONE, 0);
  pdrive->size_blocks->unit_str = strdup ("blocks");

  /* Block Size Index (power of 2)*/
  pdrive->blocksize_index = l_snmp_metric_create (self, obj, "blocksize_index", "Block Size Index", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.6.1.8", index_oidstr, RECMETHOD_NONE, 0);
  pdrive->blocksize = i_metric_create ("blocksize", "Blocksize", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(pdrive->blocksize));
  i_entity_refreshcb_add (ENTITY(pdrive->blocksize_index), v_index_refcb, pdrive->blocksize);

  /* Size */
  pdrive->size = l_snmp_metric_create (self, obj, "size", "Size", METRIC_GAUGE, ".1.3.6.1.4.1.1714.1.6.1.7", index_oidstr, RECMETHOD_NONE, 0);
  pdrive->size->alloc_unit_met = pdrive->blocksize;
  pdrive->size->valstr_func = i_string_volume_metric;
  pdrive->size->unit_str = strdup ("byte");
  pdrive->size->kbase = 1024;

  /* Speed */
  pdrive->speed = l_snmp_metric_create (self, obj, "speed", "Speed", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.6.1.9", index_oidstr, RECMETHOD_NONE, 0);
  
  /* Data Width */
  pdrive->data_width = l_snmp_metric_create (self, obj, "data_width", "Data Width", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.6.1.10", index_oidstr, RECMETHOD_NONE, 0);
  
  /* Status */
  pdrive->status = l_snmp_metric_create (self, obj, "status", "Status", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.6.1.11", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (pdrive->status, 0, "New Drive");
  i_metric_enumstr_add (pdrive->status, 1, "Online");
  i_metric_enumstr_add (pdrive->status, 2, "Used");
  i_metric_enumstr_add (pdrive->status, 3, "Spare");
  i_metric_enumstr_add (pdrive->status, 4, "Initializing");
  i_metric_enumstr_add (pdrive->status, 5, "Rebuilding");
  i_metric_enumstr_add (pdrive->status, 6, "Adding to Logical");
  i_metric_enumstr_add (pdrive->status, 9, "Global Spare");
  i_metric_enumstr_add (pdrive->status, 0x11, "Cloning");
  i_metric_enumstr_add (pdrive->status, 0x12, "Valid Clone");
  i_metric_enumstr_add (pdrive->status, 0x13, "Copying");
  i_metric_enumstr_add (pdrive->status, 0x3f, "Absent");
  i_metric_enumstr_add (pdrive->status, 0xfc, "Missing Global Spare");
  i_metric_enumstr_add (pdrive->status, 0xfd, "Missing Spare");
  i_metric_enumstr_add (pdrive->status, 0xfe, "Missing");
  i_metric_enumstr_add (pdrive->status, 0xff, "FAILED");

  /* State */
  pdrive->state = l_snmp_metric_create (self, obj, "state", "State", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.6.1.12", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (pdrive->state, 0, "Online");
  i_metric_enumstr_add (pdrive->state, 128, "Offline");

  /* Model */
  pdrive->model = l_snmp_metric_create (self, obj, "model", "Model", METRIC_STRING, ".1.3.6.1.4.1.1714.1.6.1.15", index_oidstr, RECMETHOD_NONE, 0);

  /* Firmware */
  pdrive->firmware = l_snmp_metric_create (self, obj, "firmware", "Firmware", METRIC_STRING, ".1.3.6.1.4.1.1714.1.6.1.16", index_oidstr, RECMETHOD_NONE, 0);

  /* Model */
  pdrive->serial = l_snmp_metric_create (self, obj, "serial", "Serial Number", METRIC_STRING, ".1.3.6.1.4.1.1714.1.6.1.17", index_oidstr, RECMETHOD_NONE, 0);

  /* Enqueue the pdrive item */
  num = i_list_enqueue (cnt->item_list, pdrive);
  if (num != 0)
  { i_printf (1, "v_pdrive_objfact_fab failed to enqueue pdrive for object %s", obj->name_str); v_pdrive_item_free (pdrive); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_pdrive_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_pdrive_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the pdrive and remove
   * it from the item_list
   */

  int num;
  v_pdrive_item *pdrive = obj->itemptr;

  if (!pdrive) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, pdrive);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
