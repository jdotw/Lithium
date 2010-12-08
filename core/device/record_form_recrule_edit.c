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

int form_record_recrule_edit (i_resource *self, i_form_reqdata *reqdata)
{ 
  int num;
  long ruleid = 0;
  char *str;
  char *metlabels[4] = { "Name", "Description", "Metric Type", "Current Value" };
  char *infolabels[5] = { "Site", "Device ID", "Device Description", "Management IP", "Vendor Module" };
  i_form_item *table;
  i_metric *met = NULL;
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
  { i_printf (1, "form_record_recruled_edit failed to create form"); return -1; }

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
    i_form_string_add (reqdata->form_out, "error", "Error", "Specified entity is not a metric"); 
    return 1;
  }

  /* Get rule id */
  if (reqdata->form_passdata)
  {
    ruleid = atol (reqdata->form_passdata);
    i_form_hidden_add (reqdata->form_out, "rule_id", reqdata->form_passdata);
  }

  /* Start Frame */
  asprintf (&str,  "%s (%s)", self->hierarchy->dev->desc_str, self->hierarchy->dev->name_str);
  i_form_frame_start (reqdata->form_out, self->hierarchy->dev->name_str, str);
  free (str);

  /* 
   * Basic device info frame 
   */

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
   * Metric Info
   */

  /* Start Frame */
  asprintf (&str,  "%s %s %s Recording Rules", met->obj->cnt->desc_str, met->obj->desc_str, met->desc_str);
  i_form_frame_start (reqdata->form_out, self->hierarchy->dev->name_str, str);
  free (str);

  /* Create Table */
  table = i_form_table_create (reqdata->form_out, "met_table", "Metric", 4);
  if (!table)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create table for metric info"); return 1; }
  i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  i_form_table_add_row (table, metlabels);

  /* Add metric info */
  metlabels[0] = met->name_str;
  metlabels[1] = met->desc_str;
  metlabels[2] = i_metric_typestr (met->met_type);
  metlabels[3] = i_metric_valstr (met, NULL);
  i_form_table_add_row (table, metlabels);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, self->hierarchy->dev->name_str);
  
  /*
   * Rule
   */

  if (ruleid == 0)
  {
    /* New rule being added */
    i_form_item *item;

    /* Start frame */
    asprintf (&str,  "%s %s %s Recording - Add new recording rule", met->obj->cnt->desc_str, met->obj->desc_str, met->desc_str);
    i_form_frame_start (reqdata->form_out, self->hierarchy->dev->name_str, str);
    free (str);

    /* Site drop-down */
    item = i_form_dropdown_create ("site_name", "Site");
    i_form_dropdown_add_option (item, "*", "** ALL **", 0);
    i_form_dropdown_add_option (item, met->obj->cnt->dev->site->name_str, met->obj->cnt->dev->site->desc_str, 1);
    i_form_add_item (reqdata->form_out, item);
    asprintf (&str, "site_%s_desc", met->obj->cnt->dev->site->name_str);
    i_form_hidden_add (reqdata->form_out, str, met->obj->cnt->dev->site->desc_str);
    free (str);

    /* Device drop-down */
    item = i_form_dropdown_create ("dev_name", "Device");
    i_form_dropdown_add_option (item, "*", "** ALL **", 0);
    i_form_dropdown_add_option (item, met->obj->cnt->dev->name_str, met->obj->cnt->dev->desc_str, 1);
    i_form_add_item (reqdata->form_out, item);
    asprintf (&str, "dev_%s_desc", met->obj->cnt->dev->name_str);
    i_form_hidden_add (reqdata->form_out, str, met->obj->cnt->dev->desc_str);
    free (str);

    /* Object drop-down */
    item = i_form_dropdown_create ("obj_name", "Object");
    i_form_dropdown_add_option (item, "*", "** ALL **", 0);
    i_form_dropdown_add_option (item, met->obj->name_str, met->obj->desc_str, 1);
    i_form_add_item (reqdata->form_out, item);
    asprintf (&str, "obj_%s_desc", met->obj->name_str);
    i_form_hidden_add (reqdata->form_out, str, met->obj->desc_str);
    free (str);

    /* Metric drop-down */
    item = i_form_dropdown_create ("met_name", "Metric");
    i_form_dropdown_add_option (item, "*", "** ALL **", 0);
    i_form_dropdown_add_option (item, met->name_str, met->desc_str, 1);
    i_form_add_item (reqdata->form_out, item);
    asprintf (&str, "met_%s_desc", met->name_str);
    i_form_hidden_add (reqdata->form_out, str, met->desc_str);
    free (str);

    /* Action drop-down */
    item = i_form_dropdown_create ("recordflag", "Action");
    i_form_dropdown_add_option (item, "0", "Do Not Record", 0);
    i_form_dropdown_add_option (item, "1", "Record", 1);
    i_form_add_item (reqdata->form_out, item);

    i_form_frame_end (reqdata->form_out, self->hierarchy->dev->name_str);
    
    return 1;
  }
  else
  {
    /* Rule specified */
    num = l_record_recrule_sql_get (self, ruleid, l_record_form_recrule_edit_rulecb, reqdata);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to get rule from SQL"); return 1; }
  }

  return 0;    /* Waiting for SQL */
}

int l_record_form_recrule_edit_rulecb (i_resource *self, l_record_recrule *rule, void *passdata)
{
  /* Existing rule being edited */
  char *str;
  long ruleid = 0;
  i_form_item *item;
  i_metric *met;
  i_form_reqdata *reqdata = passdata;

  /* Check rule */
  if (!rule)
  { 
    i_form_string_add (reqdata->form_out, "error", "Error", "Specified rule not found");
    i_form_deliver (self, reqdata);
    return 0;
  }

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
    i_form_string_add (reqdata->form_out, "error", "Error", "Specified entity is not a metric");
    return 1;
  }

  /* Get rule id */
  if (reqdata->form_passdata)
  {
    ruleid = atol (reqdata->form_passdata);
  }
  else
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "No recording rule specified");
    return 1;
  }
  
  /* Start frame */
  asprintf (&str,  "%s %s %s Recording Rules - Edit existing recording rule", met->obj->cnt->desc_str, met->obj->desc_str, met->desc_str);
  i_form_frame_start (reqdata->form_out, self->hierarchy->dev->name_str, str);
  free (str);

  /* ID Number */
  asprintf (&str, "%li", rule->id);
  i_form_string_add (reqdata->form_out, "idnum", "Rule ID", str);
  free (str);
  
  /* Site drop-down */
  if (rule->site_name)
  { i_form_string_add (reqdata->form_out, "current_site", "Site", rule->site_desc); }
  else
  { i_form_string_add (reqdata->form_out, "current_site", "Site", "** ALL **"); }
  item = i_form_dropdown_create ("site_name", "Change site to");
  i_form_dropdown_add_option (item, "*", "** ALL **", 0);
  i_form_dropdown_add_option (item, met->obj->cnt->dev->site->name_str, met->obj->cnt->dev->site->desc_str, 0);
  if (rule->site_name && strcmp(rule->site_name, met->obj->cnt->dev->site->name_str)!=0)
  { 
    i_form_dropdown_add_option (item, rule->site_name, rule->site_desc, 0); 
    asprintf (&str, "site_%s_desc", rule->site_name);
    i_form_hidden_add (reqdata->form_out, str, rule->site_desc);
    free (str);
  }
  if (rule->site_name)
  { i_form_dropdown_set_selected (item, rule->site_name); }
  else
  { i_form_dropdown_set_selected (item, "*"); }
  i_form_add_item (reqdata->form_out, item);
  asprintf (&str, "site_%s_desc", met->obj->cnt->dev->site->name_str);
  i_form_hidden_add (reqdata->form_out, str, met->obj->cnt->dev->site->desc_str);
  free (str);

  /* Device drop-down */
  if (rule->dev_name)
  { i_form_string_add (reqdata->form_out, "current_dev", "Device", rule->dev_desc); }
  else
  { i_form_string_add (reqdata->form_out, "current_dev", "Device", "** ALL **"); }
  item = i_form_dropdown_create ("dev_name", "Change device to");
  i_form_dropdown_add_option (item, "*", "** ALL **", 0);
  i_form_dropdown_add_option (item, met->obj->cnt->dev->name_str, met->obj->cnt->dev->desc_str, 0);
  if (rule->dev_name && strcmp(rule->dev_name, met->obj->cnt->dev->name_str)!=0)
  { 
    i_form_dropdown_add_option (item, rule->dev_name, rule->dev_desc, 0); 
    asprintf (&str, "dev_%s_desc", rule->dev_name);
    i_form_hidden_add (reqdata->form_out, str, rule->dev_desc);
    free (str);
  }
  if (rule->dev_name)
  { i_form_dropdown_set_selected (item, rule->dev_name); }
  else
  { i_form_dropdown_set_selected (item, "*"); }
  i_form_add_item (reqdata->form_out, item);
  asprintf (&str, "dev_%s_desc", met->obj->cnt->dev->name_str);
  i_form_hidden_add (reqdata->form_out, str, met->obj->cnt->dev->desc_str);
  free (str);

  /* Object drop-down */
  if (rule->obj_name)
  { i_form_string_add (reqdata->form_out, "current_obj", "Object", rule->obj_desc); }
  else
  { i_form_string_add (reqdata->form_out, "current_obj", "Object", "** ALL **"); }
  item = i_form_dropdown_create ("obj_name", "Change object to");
  i_form_dropdown_add_option (item, "*", "** ALL **", 0);
  i_form_dropdown_add_option (item, met->obj->name_str, met->obj->desc_str, 0);
  if (rule->obj_name && strcmp(rule->obj_name, met->obj->name_str)!=0)
  { 
    i_form_dropdown_add_option (item, rule->obj_name, rule->obj_desc, 0); 
    asprintf (&str, "obj_%s_desc", rule->obj_name);
    i_form_hidden_add (reqdata->form_out, str, rule->obj_desc);
    free (str);
  } 
  if (rule->obj_name)
  { i_form_dropdown_set_selected (item, rule->obj_name); }
  else
  { i_form_dropdown_set_selected (item, "*"); }
  i_form_add_item (reqdata->form_out, item);
  asprintf (&str, "obj_%s_desc", met->obj->name_str);
  i_form_hidden_add (reqdata->form_out, str, met->obj->desc_str);
  free (str);

  /* Metric drop-down */
  if (rule->met_name)
  { i_form_string_add (reqdata->form_out, "current_met", "Metric", rule->met_desc); }
  else
  { i_form_string_add (reqdata->form_out, "current_met", "Metric", "** ALL **"); }
  item = i_form_dropdown_create ("met_name", "Change metric to");
  i_form_dropdown_add_option (item, "*", "** ALL **", 0);
  i_form_dropdown_add_option (item, met->name_str, met->desc_str, 0);
  if (rule->obj_name && strcmp(rule->met_name, met->name_str)!=0)
  { 
    i_form_dropdown_add_option (item, rule->met_name, rule->met_desc, 0); 
    asprintf (&str, "met_%s_desc", rule->met_name);
    i_form_hidden_add (reqdata->form_out, str, rule->met_desc);
    free (str);
  } 
  if (rule->met_name)
  { i_form_dropdown_set_selected (item, rule->met_name); }
  else
  { i_form_dropdown_set_selected (item, "*"); }
  i_form_add_item (reqdata->form_out, item);
  asprintf (&str, "met_%s_desc", met->name_str);
  i_form_hidden_add (reqdata->form_out, str, met->desc_str);
  free (str);

  /* Recording flag drop-down */
  if (rule->recordflag == 1)
  { i_form_string_add (reqdata->form_out, "current_recordflag", "Action", "Record"); }
  else
  { i_form_string_add (reqdata->form_out, "current_recordflag", "Action", "Do Not Record"); }
  item = i_form_dropdown_create ("recordflag", "Change action to");
  i_form_dropdown_add_option (item, "0", "Do Not Record", 0);
  i_form_dropdown_add_option (item, "1", "Record", 0);
  if (rule->recordflag == 1)
  { i_form_dropdown_set_selected (item, "1"); }
  else
  { i_form_dropdown_set_selected (item, "0"); }
  i_form_add_item (reqdata->form_out, item);

  i_form_frame_end (reqdata->form_out, self->hierarchy->dev->name_str);

  i_form_deliver (self, reqdata);

  return 0;
}

int form_record_recrule_edit_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  long rule_id = 0;
  char *str;
  char *site_name = NULL;
  char *site_desc = NULL;
  char *dev_name = NULL;
  char *dev_desc = NULL;
  char *obj_name = NULL;
  char *obj_desc = NULL;
  char *met_name = NULL;
  char *met_desc = NULL;
  int recordflag = 0;
  i_form_item_option *opt;
  i_metric *met;
  l_record_recrule *rule;

  /* Check auth */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }

  /* Create (temp) form */
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
    i_form_string_add (reqdata->form_out, "error", "Error", "Specified entity is not an metric");
    return 1;
  }
  
  /* Retrieve site value */
  opt = i_form_get_value_for_item (reqdata->form_in, "site_name");
  if (opt)
  {
    site_name = (char *) opt->data;
    if (strcmp(site_name, "*")!=0)
    {
      /* Site specified */
      asprintf (&str, "site_%s_desc", site_name);
      opt = i_form_get_value_for_item (reqdata->form_in, str);
      free (str);
      if (opt)
      { site_desc = (char *) opt->data; }
    }
    else
    {
      /* All sites selected */ 
      site_name = NULL; 
    }
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "Site not specified"); return 1; }

  /* Retrieve device value */
  opt = i_form_get_value_for_item (reqdata->form_in, "dev_name");
  if (opt)
  {
    dev_name = (char *) opt->data;
    if (strcmp(dev_name, "*")!=0)
    {
      /* Device specified */
      asprintf (&str, "dev_%s_desc", dev_name);
      opt = i_form_get_value_for_item (reqdata->form_in, str);
      free (str);
      if (opt)
      { dev_desc = (char *) opt->data; }
    }
    else
    {
      /* All devices selected */
      dev_name = NULL;
    }
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "Device not specified"); return 1; }

  /* Retrieve object value */
  opt = i_form_get_value_for_item (reqdata->form_in, "obj_name");
  if (opt)
  {
    obj_name = (char *) opt->data;
    if (strcmp(obj_name, "*")!=0)
    {
      /* Object specified */
      asprintf (&str, "obj_%s_desc", obj_name);
      opt = i_form_get_value_for_item (reqdata->form_in, str);
      free (str);
      if (opt)
      { obj_desc = (char *) opt->data; }
    }
    else
    {
      /* All objects selected */
      obj_name = NULL;
    }
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "Object not specified"); return 1; }

  /* Retrieve metric value */
  opt = i_form_get_value_for_item (reqdata->form_in, "met_name");
  if (opt)
  {
    met_name = (char *) opt->data;
    if (strcmp(met_name, "*")!=0)
    {
      /* Metric specified */
      asprintf (&str, "met_%s_desc", obj_name);
      opt = i_form_get_value_for_item (reqdata->form_in, str);
      free (str);
      if (opt)
      { met_desc = (char *) opt->data; }
    }
    else
    {
      /* All metric selected */
      met_name = NULL;
    }
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "Metric not specified"); return 1; }

  /* Retrieve action value */
  opt = i_form_get_value_for_item (reqdata->form_in, "recordflag"); 
  if (opt)
  { recordflag = atoi ((char *) opt->data); }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "Action not specified"); return 1; }

  /* Create rule */
  rule = l_record_recrule_create ();
  rule->recordflag = recordflag;
  if (site_name) rule->site_name = strdup (site_name);
  if (site_desc) rule->site_desc = strdup (site_desc);
  if (dev_name) rule->dev_name = strdup (dev_name);
  if (dev_desc) rule->dev_desc = strdup (dev_desc);
  if (obj_name) rule->obj_name = strdup (obj_name);
  if (obj_desc) rule->obj_desc = strdup (obj_desc);
  if (met_name) rule->met_name = strdup (met_name);
  if (met_desc) rule->met_desc = strdup (met_desc);
  
  /* Check if a rule id is specified */
  opt = i_form_get_value_for_item (reqdata->form_in, "rule_id");
  if (opt)
  { rule_id = atol ((char *) opt->data); }
  if (rule_id > 0)
  {
    /* Editing existing rule */
    rule->id = rule_id;
    num = l_record_recrule_update (self, met, rule);
    l_record_recrule_free (rule);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to update existing rule"); return 1; }
  }
  else
  {
    /* Add new rule */
    num = l_record_recrule_add (self, met, rule);
    l_record_recrule_free (rule);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add new rule"); return 1; }
  }

  /* Free temp form */
  i_form_free (reqdata->form_out);
  reqdata->form_out = NULL;

  /* Return recrule list */
  return form_record_recrule (self, reqdata);
}

