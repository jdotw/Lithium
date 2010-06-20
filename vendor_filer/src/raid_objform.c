#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/timeutil.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/form.h>
#include <induction/str.h>

#include "raid.h"

int v_raid_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  char *labels[5] = { "Metric", "Capacity", "Available", "Used", "Percent Used" };
  i_form_item *item;
  i_object *obj = (i_object *) ent;
  v_raid_item *raid = obj->itemptr;

  /* 
   * Storage Resource
   */

  asprintf (&str, "RAID '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  /* 
   * Capacity table 
   */

  item = i_form_table_create (reqdata->form_out, "capacitytable", NULL, 5);
  if (!item) { i_printf (1, "v_raid_objform failed to create table"); return -1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  /* Header row */
  i_form_table_add_row (item, labels);

  /* Bytes row */
  labels[0] = "Bytes";
  labels[1] = i_metric_valstr (raid->bytes_total, NULL);
  labels[2] = i_metric_valstr (raid->bytes_avail, NULL);
  labels[3] = i_metric_valstr (raid->bytes_used, NULL);
  labels[4] = i_metric_valstr (raid->bytes_used_pc, NULL);
  i_form_table_add_row (item, labels);
  free (labels[1]);
  free (labels[2]);
  free (labels[3]);
  free (labels[4]);

  /* Blocks row */
  labels[0] = "Blocks";
  labels[1] = i_metric_valstr (raid->blocks_total, NULL);
  labels[2] = i_metric_valstr (raid->blocks_avail, NULL);
  labels[3] = i_metric_valstr (raid->blocks_used, NULL);
  labels[4] = i_metric_valstr (raid->blocks_used_pc, NULL);
  i_form_table_add_row (item, labels);
  free (labels[1]);
  free (labels[2]);
  free (labels[3]);
  free (labels[4]);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_raid_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_raid_item *raid = obj->itemptr;

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metcgraph_add (self, reqdata->form_out, raid->capacity_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
  /* Week */
  i_form_metcgraph_add (self, reqdata->form_out, raid->capacity_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metcgraph_add (self, reqdata->form_out, raid->capacity_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metcgraph_add (self, reqdata->form_out, raid->capacity_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);

  return 1;
}



