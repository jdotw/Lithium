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

int form_triggerset_apprule (i_resource *self, i_form_reqdata *reqdata)
{ 
  int num;
  char *str;
  char *trglabels[10] = { "Name", "Description", "Trigger Type", "Value Type", "X Value", "Y Value", "Duration", "Effect", "Admin State", "Edit" };
  char *infolabels[5] = { "Site", "Device ID", "Device Description", "Management IP", "Vendor Module" };
  i_form_item *table;
  i_object *obj = NULL;
  i_metric *met = NULL;
  i_hashtable_key *key;
  i_vendor *vendor;
  i_trigger *trg;
  i_triggerset *tset;

  /* 
   * Form Setup 
   */
  
  /* Check auth */
  if (!reqdata || !reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }

  /* Create form */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_triggerset_apprule failed to create form"); return -1; }

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

  /* Get trigger set */
  if (reqdata->form_passdata)
  {
    key = i_hashtable_create_key_string ((char *) reqdata->form_passdata, obj->cnt->tset_ht->size);
    tset = i_hashtable_get (obj->cnt->tset_ht, key);
    i_hashtable_key_free (key);
  }
  else
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "No triggerset specified"); 
    return 1;
  }
  if (!tset)
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "Specified triggerset not found"); 
    return 1;
  }

  /* Get metric */
  met = (i_metric *) i_entity_child_get (ENTITY(obj), tset->metname_str);
  if (!met)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Trigger set metric not found!"); return 1; }

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
   * Trigger list
   */

  /* Start Frame */
  asprintf (&str,  "%s %s Trigger Set %s Triggers (Metric %s)", obj->cnt->desc_str, obj->desc_str, tset->desc_str, met->desc_str);
  i_form_frame_start (reqdata->form_out, self->hierarchy->dev->name_str, str);
  free (str);

  /* Create Table */
  table = i_form_table_create (reqdata->form_out, "tset_table", "Triggers", 10);
  if (!table)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create table for trigger info"); return 1; }
  i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  i_form_table_add_row (table, trglabels);

  /* Loop through each trigger */
  for (i_list_move_head(tset->trg_list); (trg=i_list_restore(tset->trg_list))!=NULL; i_list_move_next(tset->trg_list))
  {
    int row;
    i_list *valrules;
    i_triggerset_valrule *valrule;

    /* Get value rules */
    valrules = i_triggerset_valrule_sql_load_sync (self, tset, obj, trg);
    i_list_move_head (valrules);
    valrule = i_list_restore (valrules);
    
    trglabels[0] = trg->name_str;
    trglabels[1] = trg->desc_str;
    if (valrule)
    { trglabels[2] = i_trigger_typestr (valrule->trg_type); }
    else
    { trglabels[2] = i_trigger_typestr (trg->trg_type); }
    trglabels[3] = i_value_typestr (trg->val_type);
    if (valrule)
    { asprintf (&trglabels[4], "%s%s", valrule->xval_str, met->unit_str); }
    else
    { trglabels[4] = i_value_valstr (trg->val_type, trg->val, met->unit_str, met->enumstr_list); }
    if (valrule)
    { 
      if (valrule->yval_str) asprintf (&trglabels[5], "%s%s", valrule->yval_str, met->unit_str);
      else trglabels[5] = strdup("N/A");
    }
    else
    {
      if (trg->yval) trglabels[5] = i_value_valstr (trg->val_type, trg->yval, met->unit_str, met->enumstr_list);
      else trglabels[5] = strdup("N/A");
    }
    if (valrule)
    { asprintf (&trglabels[6], "%li", valrule->duration_sec); }
    else
    { asprintf (&trglabels[6], "%li", trg->duration_sec); }
    trglabels[7] = i_entity_opstatestr (trg->effect);
    if (valrule)
    { trglabels[8] = i_entity_adminstatestr (valrule->adminstate); }
    else
    { trglabels[8] = "Enabled"; }

    /* Add data label row */
    row = i_form_table_add_row (table, trglabels);
    free (trglabels[4]);
    free (trglabels[5]);
    free (trglabels[6]);

    asprintf (&str, "%s:%s", tset->name_str, trg->name_str);
    i_form_table_add_link (table, 9, row, NULL, ENT_ADDR(obj), "triggerset_valrule", 0, str, strlen(str)+1);
    free (str);
  }

  /* End Frame */
  i_form_frame_end (reqdata->form_out, self->hierarchy->dev->name_str);

  /* 
   * Trigger application rules 
   */

  /* Start Frame */
  asprintf (&str,  "%s %s Trigger Set %s - Application Rules", obj->cnt->desc_str, obj->desc_str, tset->desc_str);
  i_form_frame_start (reqdata->form_out, self->hierarchy->dev->name_str, str);
  free (str);

  /* Load rules */
  num = i_triggerset_apprule_sql_load (self, tset, obj, l_triggerset_form_apprule_rulecb, reqdata);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to load triggerset rules"); return 1; }

  /* Return, waiting for rule list */
  return 0;
}

int l_triggerset_form_apprule_rulecb (i_resource *self, i_list *list, void *passdata)
{
  i_object *obj;
  i_triggerset *tset;
  i_triggerset_apprule *rule;
  i_form_item *item;
  i_form_item *table;
  i_form_reqdata *reqdata = passdata;
  char *labels[6] = { "Site", "Device", "Object", "Action", "Edit", "Remove" };

  /* Get local entity */
  if (reqdata->entaddr)
  {
    obj = (i_object *) i_entity_local_get (self, reqdata->entaddr);
    if (!obj)
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
  if (obj->ent_type != ENT_OBJECT)
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "Specified entity is not an object");
    i_form_deliver (self, reqdata);
    return 0;
  }

  /* Get trigger set */
  if (reqdata->form_passdata)
  {
    i_hashtable_key *key;
    key = i_hashtable_create_key_string ((char *) reqdata->form_passdata, obj->cnt->tset_ht->size);
    tset = i_hashtable_get (obj->cnt->tset_ht, key);
    i_hashtable_key_free (key);
  } 
  else
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "No triggerset specified");
    return 1;
  } 
  if (!tset)
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "Specified triggerset not found");
    return 1;
  } 
  
  /* Create Table */
  table = i_form_table_create (reqdata->form_out, "rules_table", "Rules (in order of application)", 6);
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

    /* Application flag */
    if (rule->applyflag == 0) labels[3] = "Do Not Apply";
    else labels[3] = "Apply";

    /* Add row */
    row = i_form_table_add_row (table, labels);

    asprintf (&str, "%s:%li", tset->metname_str, rule->id);
    i_form_table_add_link (table, 4, row, NULL, ENT_ADDR(obj), "triggerset_apprule_edit", 0, str, strlen(str)+!1);
    i_form_table_add_link (table, 5, row, NULL, ENT_ADDR(obj), "triggerset_apprule_remove", 0, str, strlen(str)+1);
    free (str);
  }

  labels[0] = "-Default-";
  labels[1] = "-Default-";
  labels[2] = "-Default-";
  if (tset->default_applyflag == 0)
  { labels[3] = "Do Not Apply"; }
  else
  { labels[3] = "Apply"; }
  labels[4] = NULL;
  labels[5] = NULL;
  i_form_table_add_row (table, labels);
  
  /* Add new link */
  item = i_form_string_add (reqdata->form_out, "add_new", "Add New Rule", NULL);
  i_form_item_add_link (item, 0, 0, 0, NULL, ENT_ADDR(obj), "triggerset_apprule_edit", 0, tset->metname_str, strlen(tset->metname_str)+1);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, self->hierarchy->dev->name_str);

  /* Deliver form */
  i_form_deliver (self, reqdata);

  return -1;    /* Dont keep the list */
}

