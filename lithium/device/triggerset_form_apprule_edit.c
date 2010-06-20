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
#include <induction/triggerset.h>

#include "triggerset.h"

int form_triggerset_apprule_edit (i_resource *self, i_form_reqdata *reqdata)
{ 
  int num;
  long ruleid = 0;
  char *str;
  char *trglabels[8] = { "Name", "Description", "Trigger Type", "Value Type", "X Value", "Y Value", "Effect", "Edit" };
  char *infolabels[5] = { "Site", "Device ID", "Device Description", "Management IP", "Vendor Module" };
  i_form_item *table;
  i_object *obj = NULL;
  i_metric *met = NULL;
  i_trigger *trg = NULL;
  i_hashtable_key *key;
  i_vendor *vendor;
  i_triggerset *tset;

  /* 
   * Form Setup 
   */
  
  /* Check auth */
  if (!reqdata || !reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }

  /* Create form */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_triggerset_app failed to create form"); return -1; }

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

  /* Get trigger set, and rule id */
  if (reqdata->form_passdata)
  {
    char *tset_metname;
    char *ruleid_str;

    tset_metname = strdup (reqdata->form_passdata);
    ruleid_str = strchr (tset_metname, ':');
    if (ruleid_str)
    {
      *ruleid_str = '\0';
      ruleid_str++;
      ruleid = atol (ruleid_str);
    }
    
    key = i_hashtable_create_key_string (tset_metname, obj->cnt->tset_ht->size);
    tset = i_hashtable_get (obj->cnt->tset_ht, key);
    i_hashtable_key_free (key);
    
    /* Put triggerset and rule_id in form */
    i_form_hidden_add (reqdata->form_out, "tset_metname", tset_metname);
    free (tset_metname);
    asprintf (&str, "%li", ruleid);
    i_form_hidden_add (reqdata->form_out, "rule_id", str);
    free (str);
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
  { i_form_string_add (reqdata->form_out, "error", "Error", "Trigger set metric not found!"); }

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
  table = i_form_table_create (reqdata->form_out, "tset_table", "Triggers", 8);
  if (!table)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create table for trigger info"); return 1; }
  i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  i_form_table_add_row (table, trglabels);

  /* Loop through each trigger */
  for (i_list_move_head(tset->trg_list); (trg=i_list_restore(tset->trg_list))!=NULL; i_list_move_next(tset->trg_list))
  {
    int row;

    trglabels[0] = trg->name_str;
    trglabels[1] = trg->desc_str;
    trglabels[2] = i_trigger_typestr (trg->trg_type);
    trglabels[3] = i_value_typestr (trg->val_type);
    trglabels[4] = i_value_valstr (trg->val_type, trg->val, met->unit_str, met->enumstr_list);
    if (trg->yval) trglabels[5] = i_value_valstr (trg->val_type, trg->yval, met->unit_str, met->enumstr_list);
    else trglabels[5] = "N/A";
    trglabels[6] = i_entity_opstatestr (trg->effect);

    /* Add data label row */
    row = i_form_table_add_row (table, trglabels);

    i_form_table_add_link (table, 7, row, NULL, ENT_ADDR(obj), "triggerset_value_edit", 0, NULL, 0);
  }

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
    asprintf (&str,  "%s %s Trigger Set %s - Add new application rule", obj->cnt->desc_str, obj->desc_str, tset->desc_str);
    i_form_frame_start (reqdata->form_out, self->hierarchy->dev->name_str, str);
    free (str);

    /* Site drop-down */
    item = i_form_dropdown_create ("site_name", "Site");
    i_form_dropdown_add_option (item, "*", "** ALL **", 0);
    i_form_dropdown_add_option (item, obj->cnt->dev->site->name_str, obj->cnt->dev->site->desc_str, 1);
    i_form_add_item (reqdata->form_out, item);
    asprintf (&str, "site_%s_desc", obj->cnt->dev->site->name_str);
    i_form_hidden_add (reqdata->form_out, str, obj->cnt->dev->site->desc_str);
    free (str);

    /* Device drop-down */
    item = i_form_dropdown_create ("dev_name", "Device");
    i_form_dropdown_add_option (item, "*", "** ALL **", 0);
    i_form_dropdown_add_option (item, obj->cnt->dev->name_str, obj->cnt->dev->desc_str, 1);
    i_form_add_item (reqdata->form_out, item);
    asprintf (&str, "dev_%s_desc", obj->cnt->dev->name_str);
    i_form_hidden_add (reqdata->form_out, str, obj->cnt->dev->desc_str);
    free (str);

    /* Container drop-down */
    item = i_form_dropdown_create ("obj_name", "Object");
    i_form_dropdown_add_option (item, "*", "** ALL **", 0);
    i_form_dropdown_add_option (item, obj->name_str, obj->desc_str, 1);
    i_form_add_item (reqdata->form_out, item);
    asprintf (&str, "obj_%s_desc", obj->name_str);
    i_form_hidden_add (reqdata->form_out, str, obj->desc_str);
    free (str);

    /* Action drop-down */
    item = i_form_dropdown_create ("applyflag", "Action");
    i_form_dropdown_add_option (item, "0", "Do Not Apply", 0);
    i_form_dropdown_add_option (item, "1", "Apply", 1);
    i_form_add_item (reqdata->form_out, item);

    i_form_frame_end (reqdata->form_out, self->hierarchy->dev->name_str);
    
    return 1;
  }
  else
  {
    /* Rule specified */
    num = i_triggerset_apprule_sql_get (self, ruleid, l_triggerset_form_apprule_edit_rulecb, reqdata);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to get rule from SQL"); return 1; }
  }

  return 0;    /* Waiting for SQL */
}

int l_triggerset_form_apprule_edit_rulecb (i_resource *self, i_triggerset_apprule *rule, void *passdata)
{
  /* Existing rule being edited */
  char *str;
  long ruleid = 0;
  i_form_item *item;
  i_object *obj;
  i_triggerset *tset;
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

  /* Get trigger set, and rule id */
  if (reqdata->form_passdata)
  {
    char *tset_metname;
    char *ruleid_str;
    i_hashtable_key *key;
    
    tset_metname = strdup (reqdata->form_passdata);
    ruleid_str = strchr (tset_metname, ':');
    if (ruleid_str)
    {
      *ruleid_str = '\0';
      ruleid_str++; 
      ruleid = atol (ruleid_str);
    }
    
    key = i_hashtable_create_key_string (tset_metname, obj->cnt->tset_ht->size);
    free (tset_metname);
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
  
  /* Start frame */
  asprintf (&str,  "%s %s Trigger Set %s - Edit existing application rule", obj->cnt->desc_str, obj->desc_str, tset->desc_str);
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
  i_form_dropdown_add_option (item, obj->cnt->dev->site->name_str, obj->cnt->dev->site->desc_str, 0);
  if (rule->site_name && strcmp(rule->site_name, obj->cnt->dev->site->name_str)!=0)
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
  asprintf (&str, "site_%s_desc", obj->cnt->dev->site->name_str);
  i_form_hidden_add (reqdata->form_out, str, obj->cnt->dev->site->desc_str);
  free (str);

  /* Device drop-down */
  if (rule->dev_name)
  { i_form_string_add (reqdata->form_out, "current_dev", "Device", rule->dev_desc); }
  else
  { i_form_string_add (reqdata->form_out, "current_dev", "Device", "** ALL **"); }
  item = i_form_dropdown_create ("dev_name", "Change device to");
  i_form_dropdown_add_option (item, "*", "** ALL **", 0);
  i_form_dropdown_add_option (item, obj->cnt->dev->name_str, obj->cnt->dev->desc_str, 0);
  if (rule->dev_name && strcmp(rule->dev_name, obj->cnt->dev->name_str)!=0)
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
  asprintf (&str, "dev_%s_desc", obj->cnt->dev->name_str);
  i_form_hidden_add (reqdata->form_out, str, obj->cnt->dev->desc_str);
  free (str);

  /* Object drop-down */
  if (rule->obj_name)
  { i_form_string_add (reqdata->form_out, "current_obj", "Object", rule->obj_desc); }
  else
  { i_form_string_add (reqdata->form_out, "current_obj", "Object", "** ALL **"); }
  item = i_form_dropdown_create ("obj_name", "Change object to");
  i_form_dropdown_add_option (item, "*", "** ALL **", 0);
  i_form_dropdown_add_option (item, obj->name_str, obj->desc_str, 0);
  if (rule->obj_name && strcmp(rule->obj_name, obj->name_str)!=0)
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
  asprintf (&str, "obj_%s_desc", obj->name_str);
  i_form_hidden_add (reqdata->form_out, str, obj->desc_str);
  free (str);

  /* Apply flag drop-down */
  if (rule->applyflag == 1)
  { i_form_string_add (reqdata->form_out, "current_applyflag", "Action", "Apply"); }
  else
  { i_form_string_add (reqdata->form_out, "current_applyflag", "Action", "Do Not Apply"); }
  item = i_form_dropdown_create ("applyflag", "Change action to");
  i_form_dropdown_add_option (item, "0", "Do not apply", 0);
  i_form_dropdown_add_option (item, "1", "Apply", 0);
  if (rule->applyflag == 1)
  { i_form_dropdown_set_selected (item, "1"); }
  else
  { i_form_dropdown_set_selected (item, "0"); }
  i_form_add_item (reqdata->form_out, item);

  i_form_frame_end (reqdata->form_out, self->hierarchy->dev->name_str);

  i_form_deliver (self, reqdata);

  return 0;
}

int form_triggerset_apprule_edit_submit (i_resource *self, i_form_reqdata *reqdata)
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
  int applyflag = 0;
  i_form_item_option *opt;
  i_triggerset_apprule *rule;
  i_object *obj;
  i_triggerset *tset;

  /* Check auth */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }

  /* Create (temp) form */
  reqdata->form_out = i_form_create (reqdata, 0);

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
  
  /* Get trigger set, and rule id */
  opt = i_form_get_value_for_item (reqdata->form_in, "tset_metname");
  if (opt && opt->data)
  { 
    i_hashtable_key *key;
    key = i_hashtable_create_key_string ((char *) opt->data, obj->cnt->tset_ht->size);
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

  /* Retrieve action value */
  opt = i_form_get_value_for_item (reqdata->form_in, "applyflag"); 
  if (opt)
  { applyflag = atoi ((char *) opt->data); }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "Action not specified"); return 1; }

  /* Create rule */
  rule = i_triggerset_apprule_create ();
  rule->applyflag = applyflag;
  if (site_name) rule->site_name = strdup (site_name);
  if (site_desc) rule->site_desc = strdup (site_desc);
  if (dev_name) rule->dev_name = strdup (dev_name);
  if (dev_desc) rule->dev_desc = strdup (dev_desc);
  if (obj_name) rule->obj_name = strdup (obj_name);
  if (obj_desc) rule->obj_desc = strdup (obj_desc);
  
  /* Check if a rule id is specified */
  opt = i_form_get_value_for_item (reqdata->form_in, "rule_id");
  if (opt)
  { rule_id = atol ((char *) opt->data); }
  if (rule_id > 0)
  {
    /* Editing existing rule */
    rule->id = rule_id;
    num = i_triggerset_apprule_update (self, obj, tset, rule);
    i_triggerset_apprule_free (rule);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to update existing rule"); return 1; }
  }
  else
  {
    /* Add new rule */
    num = i_triggerset_apprule_add (self, obj, tset, rule);
    i_triggerset_apprule_free (rule);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add new rule"); return 1; }
  }

  /* Free temp form */
  i_form_free (reqdata->form_out);
  reqdata->form_out = NULL;

  /* Return apprule list */
  reqdata->form_passdata = strdup (tset->metname_str);
  reqdata->form_passdata_size = strlen (tset->metname_str)+1;
  return form_triggerset_apprule (self, reqdata);
}

