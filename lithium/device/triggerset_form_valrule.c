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
#include <induction/triggerset.h>

#include "triggerset.h"

int form_triggerset_valrule (i_resource *self, i_form_reqdata *reqdata)
{ 
  char *str;
  char *trglabels[6] = { "Container", "Object", "Metric", "Trigger Set", "Trigger", "Effect" };
  char *infolabels[5] = { "Site", "Device ID", "Device Description", "Management IP", "Vendor Module" };
  char *rule_labels[10] = { "Site", "Device", "Object", "Trigger Type", "X Value", "Y Value", "Duration", "Administrative State", "Edit", "Remove" };
  i_form_item *table;
  i_form_item *item;
  i_object *obj = NULL;
  i_metric *met = NULL;
  i_hashtable_key *key;
  i_vendor *vendor;
  i_trigger *trg = NULL;
  i_triggerset *tset;
  i_list *list;
  i_triggerset_valrule *rule;

  /* 
   * Form Setup 
   */
  
  /* Check auth */
  if (!reqdata || !reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }

  /* Create form */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_triggerset_valrule failed to create form"); return -1; }

  /* Get local entity */
  if (reqdata->entaddr)
  {
    obj = (i_object *) i_entity_local_get (self, reqdata->entaddr);
    if (!obj)
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
  if (obj->ent_type != ENT_OBJECT)
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "Specified entity is not an object"); 
    return 1;
  }

  /* Get trigger and trigger */
  if (reqdata->form_passdata)
  {
    char *tset_name;
    char *trg_name;

    /* Get string */
    tset_name = strdup (reqdata->form_passdata);
    trg_name = strchr (tset_name, ':');
    if (!trg_name)
    { i_printf (1, "form_triggerset_valrule no trg name specified"); free (tset_name); return -1; }
    *trg_name = '\0';
    trg_name++;

    /* Get tset */    
    key = i_hashtable_create_key_string (tset_name, obj->cnt->tset_ht->size);
    tset = i_hashtable_get (obj->cnt->tset_ht, key);
    i_hashtable_key_free (key);
    if (!tset)
    { i_printf (1, "form_triggerset_valrule specified tset not found"); return -1; }

    /* Get trigger */
    for (i_list_move_head(tset->trg_list); (trg=i_list_restore(tset->trg_list))!=NULL; i_list_move_next(tset->trg_list))
    {
      if (!strcmp(trg->name_str, trg_name)) break;
    }
    free (tset_name);
    if (!trg) 
    { i_printf (1, "form_triggerset_valrule specified trigger not found"); return -1; }

  }
  else
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "No triggerset specified"); 
    return 1;
  }

  /* Get metric */
  met = (i_metric *) i_entity_child_get (ENTITY(obj), tset->metname_str);
  if (!met)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Trigger set metric not found!"); return 1; }

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
   * Trigger Info
   */

  /* Start Frame */
  asprintf (&str,  "%s %s Trigger Set %s Trigger %s (Metric %s)", obj->cnt->desc_str, obj->desc_str, tset->desc_str, trg->name_str, met->desc_str);
  i_form_frame_start (reqdata->form_out, self->hierarchy->dev->name_str, str);
  free (str);

  /* Create Table */
  table = i_form_table_create (reqdata->form_out, "tset_table", "Trigger Info", 6);
  if (!table)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create table for trigger info"); return 1; }
  i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  i_form_table_add_row (table, trglabels);

  /* Add trigger info */
  trglabels[0] = obj->cnt->desc_str;
  trglabels[1] = obj->desc_str;
  trglabels[2] = met->desc_str;
  trglabels[3] = tset->desc_str;
  trglabels[4] = trg->desc_str;
  trglabels[5] = i_entity_opstatestr (trg->effect);
  i_form_table_add_row (table, trglabels);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, self->hierarchy->dev->name_str);

  /* 
   * Trigger application rules 
   */

  /* Start Frame */
  asprintf (&str,  "%s %s Trigger Set %s Trigger %s - Value Rules", obj->cnt->desc_str, obj->desc_str, tset->desc_str, trg->desc_str);
  i_form_frame_start (reqdata->form_out, self->hierarchy->dev->name_str, str);
  free (str);

  /* Load rules */
  list = i_triggerset_valrule_sql_load_sync (self, tset, obj, trg);
  if (!list)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to load triggerset rules"); return 1; }

  /* Create Table */
  table = i_form_table_create (reqdata->form_out, "rules_table", "Rules (in order of application)", 10);
  if (!table)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create table for trigger info"); return 1; }
  i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */
  i_form_table_add_row (table, rule_labels);

  /* Add table rows for rules */
  for (i_list_move_head(list); (rule=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    int row;
    char *str;
    
    /* Site config */
    if (rule->site_name && rule->site_desc) { rule_labels[0] = rule->site_desc; }
    else if (rule->site_name) { rule_labels[0] = rule->site_name; }
    else { rule_labels[0] = "*"; }

    /* Device config */
    if (rule->dev_name && rule->dev_desc) { rule_labels[1] = rule->dev_desc; }
    else if (rule->dev_name) { rule_labels[1] = rule->dev_name; }
    else { rule_labels[1] = "*"; }

    /* Object config */
    if (rule->obj_name && rule->obj_desc) { rule_labels[2] = rule->obj_desc; }
    else if (rule->obj_name) { rule_labels[2] = rule->obj_name; }
    else { rule_labels[2] = "*"; }

    /* Trigger type */
    rule_labels[3] = i_trigger_typestr (rule->trg_type);

    /* X Value */
    rule_labels[4] = rule->xval_str;

    /* Y Value */
    rule_labels[5] = rule->yval_str;

    /* Duration */
    asprintf (&rule_labels[6], "%li", rule->duration_sec);

    /* Admin State */
    rule_labels[7] = i_entity_adminstatestr (rule->adminstate);
    
    /* Add row */
    row = i_form_table_add_row (table, rule_labels);

    /* Add links */
    asprintf (&str, "%s:%s:%li", tset->metname_str, trg->name_str, rule->id);
    i_form_table_add_link (table, 8, row, NULL, ENT_ADDR(obj), "triggerset_valrule_edit", 0, str, strlen(str)+1);
    i_form_table_add_link (table, 9, row, NULL, ENT_ADDR(obj), "triggerset_valrule_remove", 0, str, strlen(str)+1);
    free (str);

    /* Clean up */
    free (rule_labels[6]);
  }

  /* Default Rule */
  rule_labels[0] = "-Default-";
  rule_labels[1] = "-Default-";
  rule_labels[2] = "-Default-";
  rule_labels[3] = i_trigger_typestr (trg->trg_type);
  if (trg->val)
  { rule_labels[4] = i_value_valstr (trg->val_type, trg->val, met->unit_str, met->enumstr_list); }
  else
  { rule_labels[4] = strdup ("N/A"); }
  if (trg->yval)
  { rule_labels[5] = i_value_valstr (trg->val_type, trg->yval, met->unit_str, met->enumstr_list); }
  else
  { rule_labels[5] = strdup ("N/A"); }
  asprintf (&rule_labels[6], "%li", trg->duration_sec);
  rule_labels[7] = "Enabled";
  rule_labels[8] = NULL;
  rule_labels[9] = NULL;
  i_form_table_add_row (table, rule_labels);
  if (rule_labels[4]) free (rule_labels[4]);
  if (rule_labels[5]) free (rule_labels[5]);
  free (rule_labels[6]);
  
  /* Add new link */
  asprintf (&str, "%s:%s", tset->metname_str, trg->name_str);
  item = i_form_string_add (reqdata->form_out, "add_new", "Add New Rule", NULL);
  i_form_item_add_link (item, 0, 0, 0, NULL, ENT_ADDR(obj), "triggerset_valrule_edit", 0, str, strlen(str)+1);
  free (str);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, self->hierarchy->dev->name_str);

  return 1;
}

