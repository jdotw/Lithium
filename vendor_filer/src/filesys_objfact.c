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
#include <induction/str.h>
#include <lithium/snmp.h>

#include "filesys.h"

/* 
 * File System - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_filesys_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{  
  int num;
  v_filesys_item *filesys;

  /* Object Configuration */  
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = v_filesys_objform;
  obj->histform_func = v_filesys_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_filesys_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create filesys struct */
  filesys = v_filesys_item_create ();
  if (!filesys)
  { i_printf (1, "v_filesys_objfact_fab failed to create filesys item for obj %s", obj->name_str); return -1; }
  filesys->obj = obj;
  obj->itemptr = filesys;

  /*
   * Metric Creation
   */

  /* Bytes */
  
  filesys->bytes_total = l_snmp_metric_create (self, obj, "bytes_total", "Byte Capacity", METRIC_INTEGER, "enterprises.789.1.5.4.1.3", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  filesys->bytes_total->alloc_unit = 1024;
  filesys->bytes_total->unit_str = strdup ("byte");
  filesys->bytes_total->valstr_func = i_string_volume_metric;
  filesys->bytes_total->kbase = 1024;

  filesys->bytes_used = l_snmp_metric_create (self, obj, "bytes_used", "Bytes Used", METRIC_INTEGER, "enterprises.789.1.5.4.1.4", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  filesys->bytes_used->alloc_unit = 1024;
  filesys->bytes_used->unit_str = strdup ("byte");
  filesys->bytes_used->valstr_func = i_string_volume_metric;
  filesys->bytes_used->kbase = 1024;
  
  filesys->bytes_avail = l_snmp_metric_create (self, obj, "bytes_avail", "Bytes Available", METRIC_INTEGER, "enterprises.789.1.5.4.1.5", index_oidstr, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  filesys->bytes_avail->alloc_unit = 1024;
  filesys->bytes_avail->unit_str = strdup ("byte");
  filesys->bytes_avail->valstr_func = i_string_volume_metric;
  filesys->bytes_avail->kbase = 1024;
  
  filesys->bytes_used_pc = l_snmp_metric_create (self, obj, "bytes_used_pc", "Bytes Used Percent", METRIC_INTEGER, "enterprises.789.1.5.4.1.6", index_oidstr, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  filesys->bytes_used_pc->unit_str = strdup ("%");
  filesys->bytes_used_pc->record_defaultflag = 1;

  /* Inodes */
  
  filesys->inodes_used = l_snmp_metric_create (self, obj, "inodes_used", "Inodes Used", METRIC_INTEGER, "enterprises.789.1.5.4.1.7", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  filesys->inodes_used->unit_str = strdup ("node");
  filesys->inodes_used->valstr_func = i_string_volume_metric;

  filesys->inodes_avail = l_snmp_metric_create (self, obj, "inodes_avail", "Inodes Available", METRIC_INTEGER, "enterprises.789.1.5.4.1.8", index_oidstr, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  filesys->inodes_avail->unit_str = strdup ("node");
  filesys->inodes_avail->valstr_func = i_string_volume_metric;

  filesys->inodes_used_pc = l_snmp_metric_create (self, obj, "inodes_used_pc", "Inodes Used Percent", METRIC_INTEGER, "enterprises.789.1.5.4.1.9", index_oidstr, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  filesys->inodes_used_pc->unit_str = strdup ("%");
  filesys->inodes_used_pc->record_defaultflag = 1;

  filesys->inodes_total = i_metric_acsum_create (self, obj, "inodes_total", "Inode Capacity", METRIC_GAUGE, RECMETHOD_NONE, filesys->inodes_used, filesys->inodes_avail, ACSUM_REFCB_YMET);
  filesys->inodes_total->unit_str = strdup ("node");
  filesys->inodes_total->valstr_func = i_string_volume_metric;

  /* Files */
  
  filesys->files_maxavail = l_snmp_metric_create (self, obj, "files_maxavail", "Maximum Files Available", METRIC_INTEGER, "enterprises.789.1.5.4.1.11", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  filesys->files_maxavail->unit_str = strdup ("file");
  filesys->files_maxavail->valstr_func = i_string_volume_metric;

  filesys->files_maxpossible = l_snmp_metric_create (self, obj, "files_maxpossible", "Maximum Files Possible", METRIC_INTEGER, "enterprises.789.1.5.4.1.13", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  filesys->files_maxpossible->unit_str = strdup ("file");
  filesys->files_maxpossible->valstr_func = i_string_volume_metric;

  filesys->files_used = l_snmp_metric_create (self, obj, "files_used", "Files Used", METRIC_INTEGER, "enterprises.789.1.5.4.1.12", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  filesys->files_used->unit_str = strdup ("file");
  filesys->files_used->valstr_func = i_string_volume_metric;

  filesys->files_avail = i_metric_acdiff_create (self, obj, "files_avail", "Files Available", METRIC_FLOAT, RECMETHOD_NONE, filesys->files_maxavail, filesys->files_used, ACDIFF_REFCB_YMET);
  filesys->files_avail->unit_str = strdup ("file");
  filesys->files_avail->valstr_func = i_string_volume_metric;
  
  filesys->files_used_pc = i_metric_acpcent_create (self, obj, "files_used_pc", "Files Used Percent", RECMETHOD_RRD, filesys->files_used, filesys->files_maxavail, ACPCENT_REFCB_GAUGE);
  filesys->files_used_pc->record_defaultflag = 1;

  /* Mount Point */
  filesys->mountpoint = l_snmp_metric_create (self, obj, "mountpoint", "Mount Point", METRIC_STRING, "enterprises.789.1.5.4.1.10", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  /*
   * Custom/Combined Graphs
   */

  filesys->capacity_cg = i_metric_cgraph_create (obj, "capacity_cg", "%");
  filesys->capacity_cg->title_str = strdup ("File System Capacity");
  asprintf (&filesys->capacity_cg->render_str, "\"LINE1:met_%s_bytes_used_pc_min#000E73:Min.\" \"LINE1:met_%s_bytes_used_pc_avg#001EFF:Avg.\" \"LINE1:met_%s_bytes_used_pc_max#00B4FF:Max. Bytes Used Percent\" \"GPRINT:met_%s_bytes_used_pc_min:MIN:   Min %%.2lf %%%%\" \"GPRINT:met_%s_bytes_used_pc_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_bytes_used_pc_max:MAX: Max. %%.2lf %%%%\\n\" \"LINE1:met_%s_inodes_used_pc_min#006B00:Min.\" \"LINE1:met_%s_inodes_used_pc_avg#009B00:Avg.\" \"LINE1:met_%s_inodes_used_pc_max#00ED00:Max. Inodes Used Percent\" \"GPRINT:met_%s_inodes_used_pc_min:MIN:    Min %%.2lf %%%%\" \"GPRINT:met_%s_inodes_used_pc_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_inodes_used_pc_max:MAX: Max. %%.2lf %%%%\\n\" \"LINE1:met_%s_files_used_pc_min#6B0000:Min.\" \"LINE1:met_%s_files_used_pc_avg#9B0000:Avg.\" \"LINE1:met_%s_files_used_pc_max#ED0000:Max. Files Used Percent\" \"GPRINT:met_%s_files_used_pc_min:MIN:   Min %%.2lf %%%%\" \"GPRINT:met_%s_files_used_pc_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_files_used_pc_max:MAX: Max. %%.2lf %%%%\\n\"",
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str);
  i_list_enqueue (filesys->capacity_cg->met_list, filesys->bytes_used_pc);
  i_list_enqueue (filesys->capacity_cg->met_list, filesys->inodes_used_pc);
  i_list_enqueue (filesys->capacity_cg->met_list, filesys->files_used_pc);

  /* 
   * Misc
   */

  /* Enqueue the filesys item */
  num = i_list_enqueue (cnt->item_list, filesys);
  if (num != 0)
  { i_printf (1, "v_filesys_objfact_fab failed to enqueue filesys for object %s", obj->name_str); v_filesys_item_free (filesys); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_filesys_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_filesys_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  v_filesys_item *filesys = obj->itemptr;

  if (!filesys) return 0;

  return 0;
}

