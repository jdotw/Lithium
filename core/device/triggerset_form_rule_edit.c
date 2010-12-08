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

int form_triggerset_rule_edit (i_resource *self, i_form_reqdata *reqdata)
{ 
  int num;
  long ruleid = 0;
  char *str;
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
    char *tset_name;
    sscanf ((char *)reqdata->form_passdata, "%s:%li", &tset_name, &ruleid);
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
   * Rule
   */

  if (ruleid == 0)
  {
    /* New rule being added */
    
    return 1;
  }
  else
  {
    /* Rule specified */
    num = i_triggerset_rule_sql_get (self, ruleid, l_triggerset_form_rule_edit_rulecb, reqdata);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to get rule from SQL"); return 1; }
  }

  return 0;    /* Waiting for SQL */
}

