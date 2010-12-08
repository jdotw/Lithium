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
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>

#include "record.h"

int form_record_recrule (i_resource *self, i_form_reqdata *reqdata)
{ 
  int num;
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
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_record_recrule failed to create form"); return -1; }

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
   * Trigger list
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
   * Metric recording rules 
   */

  /* Start Frame */
  asprintf (&str,  "%s %s %s - Recording Rules", met->obj->cnt->desc_str, met->obj->desc_str, met->desc_str);
  i_form_frame_start (reqdata->form_out, self->hierarchy->dev->name_str, str);
  free (str);

  /* Load rules */
  num = l_record_recrule_sql_load (self, met, l_record_form_recrule_rulecb, reqdata);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to load metric recording rules"); return 1; }

  /* Return, waiting for rule list */
  return 0;
}

int l_record_form_recrule_rulecb (i_resource *self, i_list *list, void *passdata)
{
  i_metric *met;
  i_form_item *item;
  i_form_item *table;
  i_form_reqdata *reqdata = passdata;
  l_record_recrule *rule;
  char *labels[7] = { "Site", "Device", "Object", "Metric", "Action", "Edit", "Remove" };

  /* Get local entity */
  if (reqdata->entaddr)
  {
    met = (i_metric *) i_entity_local_get (self, reqdata->entaddr);
    if (!met)
    {
      i_form_string_add (reqdata->form_out, "error", "Error", "Specified entity not found");
      i_form_deliver (self, reqdata);
      return 0;
    }
  }
  else
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "No entity specified");
    i_form_deliver (self, reqdata);
    return 0;
  }
  if (met->ent_type != ENT_METRIC)
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "Specified entity is not a metric");
    i_form_deliver (self, reqdata);
    return 0;
  }

  /* Create Table */
  table = i_form_table_create (reqdata->form_out, "rules_table", "Rules (in order of application)", 7);
  if (!table)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create table for trigger info"); return 1; }
  i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */
  i_form_table_add_row (table, labels);

  /* Add table rows for rules */
  for (i_list_move_head(list); (rule=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    int row;
    char *str;
    
    /* Site config */
    if (rule->site_name && rule->site_desc) { labels[0] = rule->site_desc; }
    else if (rule->site_name) { labels[0] = rule->site_name; }
    else { labels[0] = "*"; }

    /* Device config */
    if (rule->dev_name && rule->dev_desc) { labels[1] = rule->dev_desc; }
    else if (rule->dev_name) { labels[1] = rule->dev_name; }
    else { labels[1] = "*"; }

    /* Object config */
    if (rule->obj_name && rule->obj_desc) { labels[2] = rule->obj_desc; }
    else if (rule->obj_name) { labels[2] = rule->obj_name; }
    else { labels[2] = "*"; }

    /* Metric */
    if (rule->met_name && rule->met_desc) { labels[3] = rule->met_desc; }
    else if (rule->met_name) { labels[3] = rule->met_name; }
    else { labels[3] = "*"; }

    /* Application flag */
    if (rule->recordflag == 0) labels[4] = "Do Not Record";
    else labels[4] = "Record";

    /* Add row */
    row = i_form_table_add_row (table, labels);

    asprintf (&str, "%li", rule->id);
    i_form_table_add_link (table, 5, row, NULL, ENT_ADDR(met), "record_recrule_edit", 0, str, strlen(str)+!1);
    i_form_table_add_link (table, 6, row, NULL, ENT_ADDR(met), "record_recrule_remove", 0, str, strlen(str)+1);
    free (str);
  }

  /* Default Rule */
  labels[0] = "-Default-";
  labels[1] = "-Default-";
  labels[2] = "-Default-";
  labels[3] = met->desc_str;
  if (met->record_defaultflag == 0) labels[4] = "Do Not Record";
  else labels[4] = "Record";
  labels[5] = NULL;
  labels[6] = NULL;
  i_form_table_add_row (table, labels);
  
  /* Add new link */
  item = i_form_string_add (reqdata->form_out, "add_new", "Add New Rule", NULL);
  i_form_item_add_link (item, 0, 0, 0, NULL, ENT_ADDR(met), "record_recrule_edit", 0, NULL, 0);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, self->hierarchy->dev->name_str);

  /* Deliver form */
  i_form_deliver (self, reqdata);

  return -1;    /* Dont keep the list */
}

