#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timeutil.h>
#include <induction/hierarchy.h>
#include <induction/vendor.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/site.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>

#include "record.h"

int form_record_recrule_remove (i_resource *self, i_form_reqdata *reqdata)
{ 
  long ruleid = 0;
  char *str;
  char *infolabels[5] = { "Site", "Device ID", "Device Description", "Management IP", "Vendor Module" };
  i_form_item *table;
  i_vendor *vendor;

  /* 
   * Form Setup 
   */
  
  /* Check auth */
  if (!reqdata || !reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }

  /* Create form */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_record_recrule_remove failed to create form"); return -1; }

  /* Get rule id */
  if (reqdata->form_passdata)
  {
    ruleid = atol (reqdata->form_passdata);

    /* Add to form */
    i_form_hidden_add (reqdata->form_out, "rule_id", reqdata->form_passdata);
  }
  else
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "No rule specified"); 
    return 1;
  }
  
  /* 
   * Basic device info frame 
   */

  /* Start Frame */
  asprintf (&str,  "%s (%s)", self->hierarchy->dev->desc_str, self->hierarchy->dev->name_str);
  i_form_frame_start (reqdata->form_out, self->hierarchy->dev->name_str, str);
  free (str);

  /* Create Table */
  table = i_form_table_create (reqdata->form_out, "deviceinfo_table", NULL, 5);
  if (!table)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create table for basic device info"); return -1; }
  i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  /* Add header row */
  i_form_table_add_row (table, infolabels);

  /* Set labels */
  if (self->hierarchy)
  {
    if (self->hierarchy->site_desc) infolabels[0] = self->hierarchy->site_desc;
    if (self->hierarchy->device_id) infolabels[1] = self->hierarchy->device_id;
    if (self->hierarchy->device_desc) infolabels[2] = self->hierarchy->device_desc;
    if (self->hierarchy->dev && self->hierarchy->dev->ip_str) infolabels[3] = self->hierarchy->dev->ip_str;
  }
  vendor = i_vendor_get ();
  if (vendor)
  { infolabels[4] = vendor->desc_str; }
  else
  { infolabels[4] = "None Loaded"; }

  /* Add data label row */
  i_form_table_add_row (table, infolabels);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, self->hierarchy->dev->name_str);

  /*
   * Recording Rule Removal confirmation
   */

  /* Start Frame */
  asprintf (&str,  "Confirm Metric Recording Rule Removal");
  i_form_frame_start (reqdata->form_out, self->hierarchy->dev->name_str, str);
  free (str);

  asprintf (&str, "%li", ruleid);
  i_form_string_add (reqdata->form_out, "ruleid", "Rule ID", str);
  free (str);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, self->hierarchy->dev->name_str);

  return 1;
}

int form_record_recrule_remove_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  long rule_id = 0;
  i_metric *met;
  i_form_item_option *opt;

  /* Check auth */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }

  /* Create form */
  reqdata->form_out = i_form_create (reqdata, 0);

  /* Get local entity */
  if (reqdata->entaddr)
  {
    met = (i_metric *) i_entity_local_get (self, reqdata->entaddr);
    if (!met)
    {
      i_form_string_add (reqdata->form_out, "error", "Error", "Specified entity not found");
      return 1;
    }
  }
  else
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "No entity specified");
    return 1;
  }
  if (met->ent_type != ENT_METRIC)
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "Specified entity is not an object");
    return 1;
  }

  /* Get rule id is specified */
  opt = i_form_get_value_for_item (reqdata->form_in, "rule_id");
  if (opt)
  { 
    rule_id = atol ((char *) opt->data);
    num = l_record_recrule_remove (self, met, rule_id);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to remove existing rule"); return 1; }
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No rule specified"); return 1; }

  /* Free temp form */
  i_form_free (reqdata->form_out);
  reqdata->form_out = NULL;

  /* Return apprule list */
  return form_record_recrule (self, reqdata);
}

