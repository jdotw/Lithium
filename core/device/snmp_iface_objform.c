#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/hierarchy.h>
#include <induction/interface.h>
#include <induction/ip.h>
#include <induction/ipregistry.h>
#include <induction/postgresql.h>
#include <induction/timeutil.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/form.h>
#include <induction/str.h>

#include "snmp.h"
#include "snmp_ipaddr.h"
#include "snmp_iface.h"

int l_snmp_iface_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  char *str;
  char *tp_labels[5] = { "Metric", "Input", "Output", "Input Last Updated", "Output Last Updated" };
  i_form_item *item;
  i_interface *iface = obj->itemptr;
  l_snmp_ipaddr *ipaddr;

  /* Start frame */
  asprintf (&str, "Interface '%s' - Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  /* Interface Info */

  i_form_string_add (reqdata->form_out, "device_name", "Device", self->hierarchy->device_desc);

  i_form_string_add (reqdata->form_out, "name", "Interface Name", obj->desc_str);

  str = i_metric_valstr (iface->alias, NULL);
  i_form_string_add (reqdata->form_out, "desc", "Description", str);
  free (str);

  for (i_list_move_head (iface->ip_list); (ipaddr=i_list_restore (iface->ip_list))!=NULL; i_list_move_next (iface->ip_list))
  {
    char *ip_str;
    char *mask_str;

    ip_str = i_metric_valstr (ipaddr->addr, NULL); 
    mask_str = i_metric_valstr (ipaddr->netmask, NULL); 

    asprintf (&str, "%16s / %16s", ip_str, mask_str);
    i_form_string_add (reqdata->form_out, ip_str, "IP Address", str);
    free (str);

    free (ip_str);
    free (mask_str);
  }
  
  str = i_metric_valstr (iface->speed, NULL);
  i_form_string_add (reqdata->form_out, "speed", "Speed", str); 
  free (str);

  /* Throughput rate table */

  item = i_form_table_create (reqdata->form_out, "tp_table", "Interface Throughput", 5);
  if (!item)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add Interface Throughput table"); return 1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */
  
  i_form_table_add_row (item, tp_labels);

  tp_labels[0] = "Bit Rate";
  tp_labels[1] = i_metric_valstr (iface->bps_in, NULL);
  tp_labels[2] = i_metric_valstr (iface->bps_out, NULL);
  tp_labels[3] = i_metric_lastupdatestr (iface->bps_in);
  tp_labels[4] = i_metric_lastupdatestr (iface->bps_in);
  i_form_table_add_row (item, tp_labels);
  if (tp_labels[1]) free (tp_labels[1]);
  if (tp_labels[2]) free (tp_labels[2]);
  if (tp_labels[3]) free (tp_labels[3]);
  if (tp_labels[4]) free (tp_labels[4]);
  
  tp_labels[0] = "Packet Rate";
  tp_labels[1] = i_metric_valstr (iface->pps_in, NULL);
  tp_labels[2] = i_metric_valstr (iface->pps_out, NULL);
  tp_labels[3] = i_metric_lastupdatestr (iface->pps_in);
  tp_labels[4] = i_metric_lastupdatestr (iface->pps_in);
  i_form_table_add_row (item, tp_labels);
  if (tp_labels[1]) free (tp_labels[1]);
  if (tp_labels[2]) free (tp_labels[2]);
  if (tp_labels[3]) free (tp_labels[3]);
  if (tp_labels[4]) free (tp_labels[4]);
  
  tp_labels[0] = "Error Rate";
  tp_labels[1] = i_metric_valstr (iface->eps_in, NULL);
  tp_labels[2] = i_metric_valstr (iface->eps_out, NULL);
  tp_labels[3] = i_metric_lastupdatestr (iface->eps_in);
  tp_labels[4] = i_metric_lastupdatestr (iface->eps_in);
  i_form_table_add_row (item, tp_labels);
  if (tp_labels[1]) free (tp_labels[1]);
  if (tp_labels[2]) free (tp_labels[2]);
  if (tp_labels[3]) free (tp_labels[3]);
  if (tp_labels[4]) free (tp_labels[4]);
  
  tp_labels[0] = "Discard Rate";
  tp_labels[1] = i_metric_valstr (iface->dps_in, NULL);
  tp_labels[2] = i_metric_valstr (iface->dps_out, NULL);
  tp_labels[3] = i_metric_lastupdatestr (iface->dps_in);
  tp_labels[4] = i_metric_lastupdatestr (iface->dps_in);
  i_form_table_add_row (item, tp_labels);
  if (tp_labels[1]) free (tp_labels[1]);
  if (tp_labels[2]) free (tp_labels[2]);
  if (tp_labels[3]) free (tp_labels[3]);
  if (tp_labels[4]) free (tp_labels[4]);

  /* End Current Frame */
  i_form_frame_end (reqdata->form_out, "iface_frame");

  return 1;
}

int l_snmp_iface_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *bc_labels[3] = { "Period", "Byte Count", "Last Updated" };
  i_form_item *item;
  i_object *obj = (i_object *) ent;
  i_interface *iface = obj->itemptr;

  if (iface->mb_day_in && iface->mb_day_out && iface->mb_month_in && iface->mb_month_out)
  {
    /* Byte Count Table */

    item = i_form_table_create (reqdata->form_out, "tp_table", "Interface Data Volume", 3);
    if (!item)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add Interface Data Volume table"); return 1; }
    i_form_item_notready (self, item);      /* Table isnt ready until the histvals are filled */
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */
  
    i_form_table_add_row (item, bc_labels); 

    i_metric_histval (self, iface->mb_day_in, VALPERIOD_DAY, reqdata->ref_sec, l_snmp_iface_objform_histvalcb, item);
    i_metric_histval (self, iface->mb_day_out, VALPERIOD_DAY, reqdata->ref_sec, l_snmp_iface_objform_histvalcb, item);
    i_metric_histval (self, iface->mb_month_in, VALPERIOD_MONTH, reqdata->ref_sec, l_snmp_iface_objform_histvalcb, item);
    i_metric_histval (self, iface->mb_month_out, VALPERIOD_MONTH, reqdata->ref_sec, l_snmp_iface_objform_histvalcb, item);
    i_form_spacer_add (reqdata->form_out);
  }

  /* Day Graph */
  item = i_form_metcgraph_add (self, reqdata->form_out, iface->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
  if (!item)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add throughput day graph"); return 1; }
  i_form_spacer_add (reqdata->form_out);

  /* Week Graph */
  item = i_form_metcgraph_add (self, reqdata->form_out, iface->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  if (!item)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add throughput week graph"); return 1; }
  i_form_spacer_add (reqdata->form_out);

  /* Month Graph */
  item = i_form_metcgraph_add (self, reqdata->form_out, iface->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  if (!item)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add throughput month graph"); return 1; }
  i_form_spacer_add (reqdata->form_out);

  /* Year Graph */
  item = i_form_metcgraph_add (self, reqdata->form_out, iface->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);
  if (!item)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add throughput year graph"); return 1; }
  
  return 1;
}

int l_snmp_iface_objform_histvalcb (i_resource *self, i_metric *met, i_list *val_list, void *passdata)
{
  char *labels[3];
  i_metric_value *val;
  i_interface *iface = met->obj->itemptr;
  i_form_item *table = passdata;

  /* Retrieve first value */
  i_list_move_head (val_list);
  val = i_list_restore (val_list);

  /* Set metric labels */
  if (met == iface->mb_day_in) labels[0] = "Daily Input Total";
  else if (met == iface->mb_day_out) labels[0] = "Daily Output Total";
  else if (met == iface->mb_month_in) labels[0] = "Monthly Input Total";
  else if (met == iface->mb_month_out) labels[0] = "Monthly Output Total";
  else labels[0] = "Unknown Metric";

  /* Common labels */
  if (val)
  {
    labels[1] = i_metric_valstr (met, val); 
    labels[2] = i_time_ctime ((time_t) val->tstamp.tv_sec);
  }
  else
  {
    labels[1] = strdup ("");
    labels[2] = strdup ("Never");
  }

  /* Add row */
  i_form_table_add_row (table, labels);
  if (labels[1]) free (labels[1]);
  if (labels[2]) free (labels[2]);

  /* Check row count. 5 rows = table is ready */
  if (i_form_table_rowcount(table) == 5)
  { i_form_item_ready (self, table); }

  return 0;
}