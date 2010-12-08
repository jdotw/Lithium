#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/navtree.h"
#include "induction/navform.h"
#include "induction/hierarchy.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "induction/name.h"
#include "induction/str.h"
#include "device/snmp.h"
#include "device/record.h"

#include "osx_server.h"
#include "data.h"
#include "drives.h"

/* Xserve Drives Info */

static i_container *static_cnt = NULL;

/* Variable Retrieval */

i_container* v_drives_cnt ()
{ return static_cnt; }

v_drives_item* v_drives_get (char *desc_str)
{
  v_drives_item *item;
  for (i_list_move_head(static_cnt->item_list); (item=i_list_restore(static_cnt->item_list))!=NULL; i_list_move_next(static_cnt->item_list))
  {
    if (strcmp(item->obj->desc_str, desc_str) == 0)
    { return item; }
  }

  return NULL;
}

/* Enable / Disable */

int v_drives_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("xsdrives", "Drives");
  if (!static_cnt)
  { i_printf (1, "v_drives_enable failed to create container"); v_drives_disable (self); return -1; }
  static_cnt->mainform_func = v_drives_cntform;
  static_cnt->sumform_func = v_drives_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_drives_enable failed to register container"); v_drives_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_drives_enable failed to load and apply container refresh config"); v_drives_disable (self); return -1; }

  /*
   * Triggers
   */
  i_triggerset *tset;

  tset = i_triggerset_create ("smart", "SMART", "smart");
  i_triggerset_addtrg (self, tset, "warning", "Pre-Failure Warning", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("raid_status", "RAID Status", "raid_status");
  i_triggerset_addtrg (self, tset, "degraded", "Degraded", VALTYPE_INTEGER, TRGTYPE_EQUAL, 1, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "offline", "Offline", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "forming", "Forming", VALTYPE_INTEGER, TRGTYPE_EQUAL, 3, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* 
   * Item and objects 
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_drives_enable failed to create item_list"); v_drives_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  return 0;
}

v_drives_item* v_drives_create (i_resource *self, char *desc_str)
{
  char *name_str;
  v_drives_item *item;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Set name/desc */
  name_str = strdup (desc_str);
  i_name_parse (name_str);

  /* Create object */
  obj = i_object_create (name_str, desc_str);
  free (name_str);
  obj->mainform_func = v_drives_objform;
  obj->histform_func = v_drives_objform_hist;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Create item */
  item = v_drives_item_create ();
  item->obj = obj;
  obj->itemptr = item;
  i_list_enqueue (static_cnt->item_list, item);

  /*
   * Create Metrics 
   */

  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* Capacity */
  item->capacity = i_metric_create ("capacity", "Capacity", METRIC_GAUGE);
  item->capacity->alloc_unit = (1024 * 1024);
  item->capacity->unit_str = strdup ("Byte");
  item->capacity->valstr_func = i_string_volume_metric;
  i_entity_register (self, ENTITY(obj), ENTITY(item->capacity));
  i_entity_refresh_config_apply (self, ENTITY(item->capacity), &refconfig);

  /* Interconnect */
  item->interconnect = i_metric_create ("interconnect", "Interconnect", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->interconnect));
  i_entity_refresh_config_apply (self, ENTITY(item->interconnect), &refconfig);

  /* Disk ID */
  item->diskid = i_metric_create ("diskid", "Disk ID", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->diskid));
  i_entity_refresh_config_apply (self, ENTITY(item->diskid), &refconfig);

  /* Location */
  item->location = i_metric_create ("location", "Location", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->location));
  i_entity_refresh_config_apply (self, ENTITY(item->location), &refconfig);

  /* Vendor */
  item->vendor = i_metric_create ("vendor", "Vendor", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->vendor));
  i_entity_refresh_config_apply (self, ENTITY(item->vendor), &refconfig);

  /* Model */
  item->model = i_metric_create ("model", "Model", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->model));
  i_entity_refresh_config_apply (self, ENTITY(item->model), &refconfig);

  /* SMART */
  item->smart_desc = i_metric_create ("smart_desc", "SMART Report", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->smart_desc));
  i_entity_refresh_config_apply (self, ENTITY(item->smart_desc), &refconfig);
  item->smart = i_metric_create ("smart", "SMART Status", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->smart));
  i_entity_refresh_config_apply (self, ENTITY(item->smart), &refconfig);
  i_metric_enumstr_add (item->smart, 0, "Unknown");
  i_metric_enumstr_add (item->smart, 1, "OK");
  i_metric_enumstr_add (item->smart, 2, "Pre-Failure Warning");
  i_metric_enumstr_add (item->smart, 3, "Not Supported");
  i_entity_refreshcb_add (ENTITY(item->smart_desc), v_drives_smart_refcb, item->smart);

  /* Bytes Read */
  item->bytes_read = i_metric_create ("bytes_read", "Bytes Read", METRIC_FLOAT);
  item->bytes_read->unit_str = strdup ("B");
  i_entity_register (self, ENTITY(obj), ENTITY(item->bytes_read));
  i_entity_refresh_config_apply (self, ENTITY(item->bytes_read), &refconfig);

  /* Bytes Written */
  item->bytes_written = i_metric_create ("bytes_written", "Bytes Written", METRIC_FLOAT);
  item->bytes_written->unit_str = strdup ("B");
  i_entity_register (self, ENTITY(obj), ENTITY(item->bytes_written));
  i_entity_refresh_config_apply (self, ENTITY(item->bytes_written), &refconfig);

  /* Read Byte/sec */
  item->read_bps = i_metric_acrate_create (self, obj, "read_bps", "Read Throughput", "Bps", RECMETHOD_RRD, item->bytes_read, 0);
  item->read_bps->record_defaultflag = 1;

  /* Write Byte/sec */
  item->write_bps = i_metric_acrate_create (self, obj, "write_bps", "Write Throughput", "Bps", RECMETHOD_RRD, item->bytes_written, 0);
  item->write_bps->record_defaultflag = 1;
  
  /* Read Errors */
  item->read_errors = i_metric_create ("read_errors", "Read Errors", METRIC_FLOAT);
  i_entity_register (self, ENTITY(obj), ENTITY(item->read_errors));
  i_entity_refresh_config_apply (self, ENTITY(item->read_errors), &refconfig);

  /* Write Errors */
  item->write_errors = i_metric_create ("write_errors", "Write Errors", METRIC_FLOAT);
  i_entity_register (self, ENTITY(obj), ENTITY(item->write_errors));
  i_entity_refresh_config_apply (self, ENTITY(item->write_errors), &refconfig);

  /* Read Byte/sec */
  item->read_eps = i_metric_acrate_create (self, obj, "read_eps", "Read Error Rate", "err/s", RECMETHOD_RRD, item->read_errors, 0);
  item->read_eps->record_defaultflag = 1;

  /* Write Byte/sec */
  item->write_eps = i_metric_acrate_create (self, obj, "write_eps", "Write Error Rate", "err/s", RECMETHOD_RRD, item->write_errors, 0);
  item->write_eps->record_defaultflag = 1;

  /* RAID */
  item->raid_status = i_metric_create ("raid_status", "RAID Status", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->raid_status));
  i_entity_refresh_config_apply (self, ENTITY(item->raid_status), &refconfig);
  i_metric_enumstr_add (item->raid_status, 0, "OK");
  i_metric_enumstr_add (item->raid_status, 1, "Degraded");
  i_metric_enumstr_add (item->raid_status, 2, "Offline");
  i_metric_enumstr_add (item->raid_status, 3, "Forming");
  item->raid_desc = i_metric_create ("raid_desc", "RAID Description", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->raid_desc));
  i_entity_refresh_config_apply (self, ENTITY(item->raid_desc), &refconfig);


  /*
   * Combined/Custom Graphs
   */
  item->tput_cg = i_metric_cgraph_create (obj, "tput_cg", "Bits/sec");
  asprintf (&item->tput_cg->title_str, "Drive '%s' Throughput", obj->desc_str);
  asprintf (&item->tput_cg->render_str, "\"CDEF:kread_bps_min=met_%s_read_bps_min,1000,/\" \"CDEF:kread_bps_avg=met_%s_read_bps_avg,1000,/\" \"CDEF:kread_bps_max=met_%s_read_bps_max,1000,/\" \"CDEF:kwrite_bps_min=met_%s_write_bps_min,1000,/\" \"CDEF:kwrite_bps_avg=met_%s_write_bps_avg,1000,/\" \"CDEF:kwrite_bps_max=met_%s_write_bps_max,1000,/\" \"LINE1:met_%s_read_bps_min#000E73:Min.\" \"LINE1:met_%s_read_bps_avg#001EFF:Avg.\" \"LINE1:met_%s_read_bps_max#00B4FF: Max. Read\" \"GPRINT:kread_bps_min:MIN:   Min %%.2lf Kbits/s\" \"GPRINT:kread_bps_avg:AVERAGE: Avg %%.2lf Kbits/s\" \"GPRINT:kread_bps_max:MAX: Max %%.2lf Kbits/s\\n\" \"LINE1:met_%s_write_bps_min#006B00:Min.\" \"LINE1:met_%s_write_bps_avg#009B00:Avg.\" \"LINE1:met_%s_write_bps_max#00ED00: Max. Write\" \"GPRINT:kwrite_bps_min:MIN:  Min %%.2lf Kbits/s\" \"GPRINT:kwrite_bps_avg:AVERAGE: Avg %%.2lf Kbits/s\" \"GPRINT:kwrite_bps_max:MAX: Max %%.2lf Kbits/s\\n\"",
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str, 
    obj->name_str, obj->name_str, obj->name_str);
  i_list_enqueue (item->tput_cg->met_list, item->read_bps);
  i_list_enqueue (item->tput_cg->met_list, item->write_bps);
  
  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  return item;
}

int v_drives_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

