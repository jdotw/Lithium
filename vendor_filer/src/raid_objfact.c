#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/interface.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/str.h>
#include <lithium/snmp.h>

#include "raid.h"

/* 
 * RAID - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_raid_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{  
  int num;
  v_raid_item *raid;

  /* Object Configuration */  
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = v_raid_objform;
  obj->histform_func = v_raid_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_raid_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create raid struct */
  raid = v_raid_item_create ();
  if (!raid)
  { i_printf (1, "v_raid_objfact_fab failed to create raid item for obj %s", obj->name_str); return -1; }
  raid->obj = obj;
  obj->itemptr = raid;

  /*
   * Metric Creation
   */

  /* Status */

  raid->opstate = l_snmp_metric_create (self, obj, "opstate", "Operational State", METRIC_INTEGER, "enterprises.789.1.6.2.1.3", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_metric_enumstr_add (raid->opstate, 0, "Unknown");
  i_metric_enumstr_add (raid->opstate, 1, "Active");
  i_metric_enumstr_add (raid->opstate, 2, "Reconstruction");
  i_metric_enumstr_add (raid->opstate, 3, "Parity Reconstruction");
  i_metric_enumstr_add (raid->opstate, 4, "Parity Verification");
  i_metric_enumstr_add (raid->opstate, 5, "Scrubbing");
  i_metric_enumstr_add (raid->opstate, 6, "Failed");

  /* ID */

  raid->diskid = l_snmp_metric_create (self, obj, "diskid", "Disk ID", METRIC_INTEGER, "enterprises.789.1.6.2.1.4", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  raid->scsiadaptor = l_snmp_metric_create (self, obj, "scsiadaptor", "SCSI Adaptor", METRIC_INTEGER, "enterprises.789.1.6.2.1.5", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  raid->scsiid = l_snmp_metric_create (self, obj, "scsiid", "SCSI ID", METRIC_INTEGER, "enterprises.789.1.6.2.1.6", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  /* Byte Capacity/Used */

  raid->bytes_total = l_snmp_metric_create (self, obj, "bytes_total", "Byte Capacity", METRIC_INTEGER, "enterprises.789.1.6.2.1.9", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  raid->bytes_total->alloc_unit = (1024 * 1024);
  raid->bytes_total->unit_str = strdup ("byte");
  raid->bytes_total->valstr_func = i_string_volume_metric;
  raid->bytes_total->kbase = 1024;
  
  raid->bytes_used = l_snmp_metric_create (self, obj, "bytes_used", "Bytes Used", METRIC_INTEGER, "enterprises.789.1.6.2.1.7", index_oidstr, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  raid->bytes_used->alloc_unit = (1024 * 1024);
  raid->bytes_used->unit_str = strdup ("byte");
  raid->bytes_used->valstr_func = i_string_volume_metric;
  raid->bytes_used->kbase = 1024;

  raid->bytes_avail = i_metric_acdiff_create (self, obj, "bytes_avail", "Bytes Available", METRIC_FLOAT, RECMETHOD_NONE, raid->bytes_total, raid->bytes_used, ACDIFF_REFCB_YMET);
  raid->bytes_avail->alloc_unit = (1024 * 1024);
  raid->bytes_avail->unit_str = strdup ("byte");
  raid->bytes_avail->valstr_func = i_string_volume_metric;
  raid->bytes_avail->kbase = 1024;
        
  raid->bytes_used_pc = i_metric_acpcent_create (self, obj, "bytes_used_pc", "Bytes Used Percent", RECMETHOD_RRD, raid->bytes_used, raid->bytes_total, ACPCENT_REFCB_GAUGE);
  raid->bytes_used_pc->record_defaultflag = 1;

  /* Block Capacity/Used */

  raid->blocks_total = l_snmp_metric_create (self, obj, "blocks_total", "Block Capacity", METRIC_INTEGER, "enterprises.789.1.6.2.1.10", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  raid->blocks_total->unit_str = strdup ("block");
  raid->blocks_total->valstr_func = i_string_volume_metric;
  
  raid->blocks_used = l_snmp_metric_create (self, obj, "blocks_used", "Blocks Used", METRIC_INTEGER, "enterprises.789.1.6.2.1.8", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  raid->blocks_used->unit_str = strdup ("block");
  raid->blocks_used->valstr_func = i_string_volume_metric;

  raid->blocks_avail = i_metric_acdiff_create (self, obj, "blocks_avail", "Blocks Available", METRIC_FLOAT, RECMETHOD_NONE, raid->blocks_total, raid->blocks_used, ACDIFF_REFCB_YMET);
  raid->blocks_avail->unit_str = strdup ("block");
  raid->blocks_avail->valstr_func = i_string_volume_metric;
        
  raid->blocks_used_pc = i_metric_acpcent_create (self, obj, "blocks_used_pc", "Blocks Used Percent", RECMETHOD_RRD, raid->blocks_used, raid->blocks_total, ACPCENT_REFCB_GAUGE);
  raid->blocks_used_pc->record_defaultflag = 1;

  /* Progress */

  raid->complete_pc = l_snmp_metric_create (self, obj, "complete_pc", "Completion Percent", METRIC_INTEGER, "enterprises.789.1.6.2.1.11", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  /* RAID */

  raid->vol = l_snmp_metric_create (self, obj, "vol", "Volume", METRIC_INTEGER, "enterprises.789.1.6.2.1.12", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  raid->group = l_snmp_metric_create (self, obj, "group", "Group", METRIC_INTEGER, "enterprises.789.1.6.2.1.13", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  raid->diskcount = l_snmp_metric_create (self, obj, "diskcount", "Total Drives in Group", METRIC_INTEGER, "enterprises.789.1.6.2.1.14", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  raid->groupcount = l_snmp_metric_create (self, obj, "groupcount", "Total Groups in Volume", METRIC_INTEGER, "enterprises.789.1.6.2.1.15", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  /*
   * Custom/Combined Graphs
   */

  raid->capacity_cg = i_metric_cgraph_create (obj, "capacity_cg", "%");
  raid->capacity_cg->title_str = strdup ("RAID Drive Capacity");
  asprintf (&raid->capacity_cg->render_str, "\"LINE1:met_%s_bytes_used_pc_min#000E73:Min.\" \"LINE1:met_%s_bytes_used_pc_avg#001EFF:Avg.\" \"LINE1:met_%s_bytes_used_pc_max#00B4FF:Max. Bytes Used Percent\" \"GPRINT:met_%s_bytes_used_pc_min:MIN:   Min %%.2lf %%%%\" \"GPRINT:met_%s_bytes_used_pc_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_bytes_used_pc_max:MAX: Max. %%.2lf %%%%\\n\" \"LINE1:met_%s_blocks_used_pc_min#006B00:Min.\" \"LINE1:met_%s_blocks_used_pc_avg#009B00:Avg.\" \"LINE1:met_%s_blocks_used_pc_max#00ED00:Max. blocks Used Percent\" \"GPRINT:met_%s_blocks_used_pc_min:MIN:    Min %%.2lf %%%%\" \"GPRINT:met_%s_blocks_used_pc_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_blocks_used_pc_max:MAX: Max. %%.2lf %%%%\\n\"",
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str);
  i_list_enqueue (raid->capacity_cg->met_list, raid->bytes_used_pc);
  i_list_enqueue (raid->capacity_cg->met_list, raid->blocks_used_pc);


  /* 
   * Misc
   */

  /* Enqueue the raid item */
  num = i_list_enqueue (cnt->item_list, raid);
  if (num != 0)
  { i_printf (1, "v_raid_objfact_fab failed to enqueue raid for object %s", obj->name_str); v_raid_item_free (raid); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_raid_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_raid_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  v_raid_item *raid = obj->itemptr;

  if (!raid) return 0;

  return 0;
}

