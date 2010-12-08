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

int form_triggerset_valrule_edit (i_resource *self, i_form_reqdata *reqdata)
{ 
  int num;
  long ruleid = 0;
  char *str;
  char *trglabels[8] = { "Container", "Object", "Metric", "Trigger Set", "Trigger", "Effect" };
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
    char *trg_name;
    char *ruleid_str;

    tset_metname = strdup (reqdata->form_passdata);
    trg_name = strchr (tset_metname, ':');
    if (!trg_name)
    { i_form_string_add (reqdata->form_out, "error", "Error", "No trigger specified"); return 1; }
    *trg_name = '\0';
    trg_name++;
    ruleid_str = strchr (trg_name, ':');
    if (ruleid_str)
    {
      *ruleid_str = '\0';
      ruleid_str++;
      ruleid = atol (ruleid_str);
    }

    /* Get tset */
    key = i_hashtable_create_key_string (tset_metname, obj->cnt->tset_ht->size);
    tset = i_hashtable_get (obj->cnt->tset_ht, key);
    i_hashtable_key_free (key);
    if (!tset)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified triggerset not found"); free (tset_metname); return 1; }

    /* Get trigger */
    for (i_list_move_head(tset->trg_list); (trg=i_list_restore(tset->trg_list))!=NULL; i_list_move_next(tset->trg_list))
    {
      if (!strcmp(trg->name_str, trg_name)) break;
    }
    if (!trg)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified trigger not found"); free (tset_metname); return 1; }
    
    /* Put triggerset, trg_name  and rule_id in form */
    i_form_hidden_add (reqdata->form_out, "tset_metname", tset_metname);
    i_form_hidden_add (reqdata->form_out, "trg_name", trg->name_str);
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

  /* Get metric */
  met = (i_metric *) i_entity_child_get (ENTITY(obj), tset->metname_str);
  if (!met)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Trigger set metric not found!"); }

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
  asprintf (&str,  "%s %s Trigger Set %s Trigger %s (Metric %s)", obj->cnt->desc_str, obj->desc_str, tset->desc_str, trg->desc_str, met->desc_str);
  i_form_frame_start (reqdata->form_out, self->hierarchy->dev->name_str, str);
  free (str);

  /* Create Table */
  table = i_form_table_create (reqdata->form_out, "tset_table", "Triggers", 8);
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
  trglabels[5] = i_entity_opstatestr (trg->effect);;
  i_form_table_add_row (table, trglabels);

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
    asprintf (&str,  "%s %s Trigger Set %s Trigger %s - Add new value rule", obj->cnt->desc_str, obj->desc_str, tset->desc_str, trg->desc_str);
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
    item = i_form_dropdown_create ("trg_type", "Trigger Type");
    i_form_dropdown_add_option (item, "1", "Metric Value Equals", 0);
    i_form_dropdown_add_option (item, "2", "Metric Value Is Less Than X", 0);
    i_form_dropdown_add_option (item, "3", "Metric Value Is Greater Than X", 0);
    i_form_dropdown_add_option (item, "4", "Metric Value Is Not X", 0);
    i_form_dropdown_add_option (item, "5", "Metric Value Is Between X and Y", 0);
    i_form_add_item (reqdata->form_out, item);

    /* X Value */
    i_form_entry_add (reqdata->form_out, "xval", "X Value", NULL);

    /* Y Value */
    i_form_entry_add (reqdata->form_out, "yval", "Y Value", NULL);

    /* Duration */
    asprintf (&str, "%li", trg->duration_sec);
    i_form_entry_add (reqdata->form_out, "duration_sec", "Duration (sec)", str);
    free (str);

    /* Admin State drop-down */
    item = i_form_dropdown_create ("adminstate", "Administrative State");
    i_form_dropdown_add_option (item, "0", "Enabled", 1);
    i_form_dropdown_add_option (item, "1", "Disabled", 0);
    i_form_add_item (reqdata->form_out, item);

    i_form_frame_end (reqdata->form_out, self->hierarchy->dev->name_str);
    
    return 1;
  }
  else
  {
    /* Rule specified */
    num = i_triggerset_valrule_sql_get (self, ruleid, l_triggerset_form_valrule_edit_rulecb, reqdata);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to get rule from SQL"); return 1; }
  }

  return 0;    /* Waiting for SQL */
}

int l_triggerset_form_valrule_edit_rulecb (i_resource *self, i_triggerset_valrule *rule, void *passdata)
{
  /* Existing rule being edited */
  char *str;
  long ruleid = 0;
  i_form_item *item;
  i_object *obj;
  i_trigger *trg;
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
    i_hashtable_key *key;
    char *tset_metname;
    char *trg_name;
    char *ruleid_str; 
  
    tset_metname = strdup (reqdata->form_passdata); 
    trg_name = strchr (tset_metname, ':');
    if (!trg_name)
    { i_form_string_add (reqdata->form_out, "error", "Error", "No trigger specified"); return 1; }
    *trg_name = '\0';
    trg_name++;
    ruleid_str = strchr (trg_name, ':');
    if (ruleid_str)
    {
      *ruleid_str = '\0';
      ruleid_str++;
      ruleid = atol (ruleid_str);
    }

    /* Get tset */
    key = i_hashtable_create_key_string (tset_metname, obj->cnt->tset_ht->size);
    tset = i_hashtable_get (obj->cnt->tset_ht, key);
    i_hashtable_key_free (key);
    if (!tset)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified triggerset not found"); free (tset_metname); return 1; }

    /* Get trigger */
    for (i_list_move_head(tset->trg_list); (trg=i_list_restore(tset->trg_list))!=NULL; i_list_move_next(tset->trg_list))
    {
      if (!strcmp(trg->name_str, trg_name)) break;
    }
    if (!trg)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified trigger not found"); free (tset_metname); return 1; }
  }
  else
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "No triggerset specified");
    return 1;
  }
  
  /* Start frame */
  asprintf (&str,  "%s %s Trigger Set %s Trigger %s - Edit existing application rule", obj->cnt->desc_str, obj->desc_str, tset->desc_str, trg->desc_str);
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
  switch (rule->trg_type)
  {
    case 1: i_form_string_add (reqdata->form_out, "current_trg_type", "Trigger Type", "Metric Value Equals");
            break;
    case 2: i_form_string_add (reqdata->form_out, "current_trg_type", "Trigger Type", "Metric Value Is Less Than X");
            break;
    case 3: i_form_string_add (reqdata->form_out, "current_trg_type", "Trigger Type", "Metric Value Is Greater Than X");
            break;
    case 4: i_form_string_add (reqdata->form_out, "current_trg_type", "Trigger Type", "Metric Value Is Not X");
            break;
    case 5: i_form_string_add (reqdata->form_out, "current_trg_type", "Trigger Type", "Metric Value Is Between X and Y");
            break;
  }
  item = i_form_dropdown_create ("trg_type", "Change Trigger Type to");
  i_form_dropdown_add_option (item, "1", "Metric Value Equals", 0);
  i_form_dropdown_add_option (item, "2", "Metric Value Is Less Than X", 0);
  i_form_dropdown_add_option (item, "3", "Metric Value Is Greater Than X", 0);
  i_form_dropdown_add_option (item, "4", "Metric Value Is Not X", 0);
  i_form_dropdown_add_option (item, "5", "Metric Value Is Between X and Y", 0);
  asprintf (&str, "%i", rule->trg_type);
  i_form_dropdown_set_selected (item, str);
  free (str);
  i_form_add_item (reqdata->form_out, item);

  /* X Value */
  i_form_string_add (reqdata->form_out, "current_xval", "X Value", rule->xval_str);
  i_form_entry_add (reqdata->form_out, "xval", "Change X Value To", rule->xval_str);
  
  /* Y Value */
  i_form_string_add (reqdata->form_out, "current_yval", "Y Value", rule->yval_str);
  i_form_entry_add (reqdata->form_out, "yval", "Change Y Value To", rule->yval_str);

  /* Duration */
  asprintf (&str, "%li", rule->duration_sec);
  i_form_string_add (reqdata->form_out, "current_duration", "Duration (sec)", str);
  i_form_entry_add (reqdata->form_out, "duration", "Change Duration (sec) To", str);
  free (str);

  /* Admin State */
  if (rule->adminstate == 0) i_form_string_add (reqdata->form_out, "current_adminstate", "Adminstate", "Enabled");
  else if (rule->adminstate == 1) i_form_string_add (reqdata->form_out, "current_adminstate", "Adminstate", "Disabled");
  else i_form_string_add (reqdata->form_out, "current_adminstate", "Adminstate", "Unknown");
  item = i_form_dropdown_create ("adminstate", "Change Administrative State to");
  i_form_dropdown_add_option (item, "0", "Enabled", 0);
  i_form_dropdown_add_option (item, "1", "Disabled", 0);
  asprintf (&str, "%i", rule->adminstate);
  i_form_dropdown_set_selected (item, str);
  free (str);
  i_form_add_item (reqdata->form_out, item);
  
  i_form_frame_end (reqdata->form_out, self->hierarchy->dev->name_str);

  i_form_deliver (self, reqdata);

  return 0;
}

int form_triggerset_valrule_edit_submit (i_resource *self, i_form_reqdata *reqdata)
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
  char *xval_str = NULL;
  char *yval_str = NULL;
  char *duration_str = NULL;
  int trg_type = 0;
  int adminstate = 0;
  i_form_item_option *opt;
  i_triggerset_valrule *rule;
  i_object *obj;
  i_trigger *trg;
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

  /* Get trigger set */
  opt = i_form_get_value_for_item (reqdata->form_in, "tset_metname");
  if (opt && opt->data)
  {
    i_hashtable_key *key;
    key = i_hashtable_create_key_string ((char *) opt->data, obj->cnt->tset_ht->size);
    tset = i_hashtable_get (obj->cnt->tset_ht, key);
    i_hashtable_key_free (key);
    if (!tset)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified triggerset not found"); return 1; }
  }
  else
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "No triggerset specified");
    return 1;
  }

  /* Get trigger */
  opt = i_form_get_value_for_item (reqdata->form_in, "trg_name");
  if (opt && opt->data)
  {
    for (i_list_move_head(tset->trg_list); (trg=i_list_restore(tset->trg_list))!=NULL; i_list_move_next(tset->trg_list))
    {
      if (!strcmp(trg->name_str, (char *)opt->data)) break;
    }
    if (!trg)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified trigger not found"); return 1; }
  } 
  else
  { 
    i_form_string_add (reqdata->form_out, "error", "Error", "No trigger specified");
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

  /* Retrieve type value */
  opt = i_form_get_value_for_item (reqdata->form_in, "trg_type"); 
  if (opt)
  { trg_type = atoi ((char *) opt->data); }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "Trigger type not specified"); return 1; }

  /* Retrieve xval */
  opt = i_form_get_value_for_item (reqdata->form_in, "xval");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Xvalue not specified"); return 1; }
  xval_str = (char *) opt->data;

  /* Retrieve yval */
  opt = i_form_get_value_for_item (reqdata->form_in, "yval");
  if (opt)
  { yval_str = (char *) opt->data; }
  else
  { yval_str = NULL; }

  /* Retrieve duration */
  opt = i_form_get_value_for_item (reqdata->form_in, "duration_sec");
  if (opt)
  { duration_str = (char *) opt->data; }
  else
  { duration_str = NULL; }

  /* Retrieve adminstate */
  opt = i_form_get_value_for_item (reqdata->form_in, "adminstate"); 
  if (opt)
  { adminstate = atoi ((char *) opt->data); }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "Admin state not specified"); return 1; }

  /* Create rule */
  rule = i_triggerset_valrule_create ();
  if (site_name) rule->site_name = strdup (site_name);
  if (site_desc) rule->site_desc = strdup (site_desc);
  if (dev_name) rule->dev_name = strdup (dev_name);
  if (dev_desc) rule->dev_desc = strdup (dev_desc);
  if (obj_name) rule->obj_name = strdup (obj_name);
  if (obj_desc) rule->obj_desc = strdup (obj_desc);
  rule->trg_name = strdup (trg->name_str);
  rule->trg_desc = strdup (trg->desc_str);
  if (xval_str) rule->xval_str = strdup (xval_str);
  if (yval_str) rule->yval_str = strdup (yval_str);
  if (duration_str) rule->duration_sec = atol (duration_str);
  rule->adminstate = adminstate;
  rule->trg_type = trg_type;
  
  /* Check if a rule id is specified */
  opt = i_form_get_value_for_item (reqdata->form_in, "rule_id");
  if (opt)
  { rule_id = atol ((char *) opt->data); }
  if (rule_id > 0)
  {
    /* Editing existing rule */
    rule->id = rule_id;
    num = i_triggerset_valrule_update (self, obj, tset, rule);
    i_triggerset_valrule_free (rule);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to update existing rule"); return 1; }
  }
  else
  {
    /* Add new rule */
    num = i_triggerset_valrule_add (self, obj, tset, rule);
    i_triggerset_valrule_free (rule);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add new rule"); return 1; }
  }

  /* Free temp form */
  i_form_free (reqdata->form_out);
  reqdata->form_out = NULL;

  /* Return valrule list */
  asprintf (&str, "%s:%s", tset->metname_str, trg->name_str);
  reqdata->form_passdata = str;
  reqdata->form_passdata_size = strlen (str)+1;
  return form_triggerset_valrule (self, reqdata);
}

