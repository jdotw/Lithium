#include <stdlib.h>

#include <induction.h>

#include "feature.h"
#include "featuredb.h"

/* Feature list form */

int form_feature_list (i_resource *self, i_form_reqdata *reqdata)
{
  char *res_addr_str;
  char *labels[5] = { "Description", "ID", "Control", "Configure", "Remove" };
  i_form_item *item;
  l_feature *feature;
  i_list *feature_list;

  if (!self || !reqdata) return -1;

  /* Auth checking */

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF) 
  { return i_form_deliver_denied (self, reqdata); }

  /* Form creation */
  
  reqdata->form_out = i_form_create (0);
  if (!reqdata->form_out) 
  { i_printf (1, "form_feature_list failed to create form"); return -1; }
  i_form_set_title (reqdata->form_out, "Device feature configuration");

  /* 'Add' Link */
  
  item = i_form_string_add (reqdata->form_out, "add", "Add Feature", NULL);
  if (!item) { i_form_string_add (reqdata->form_out, "error", "Error", "form_main unable to create feature-add link item"); return 1; }
  res_addr_str = i_resource_address_struct_to_string (RES_ADDR(self));          
  i_form_item_add_link (item, 0, 0, 0, res_addr_str, "feature_add", NULL, 0);
  free (res_addr_str);
  
  /* Feature list */

  feature_list = l_feature_list ();
  if (!feature_list)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to retrieve feature list"); return 1; }
  
  item = i_form_table_create (reqdata->form_out, "feature_list", "Configured Features", 4);
  if (!item) 
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create table"); return 1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  i_form_table_add_row (item, labels);
  
  for (i_list_move_head(feature_list); (feature=i_list_restore(feature_list))!=NULL; i_list_move_next(feature_list))
  {
    int row;

    if (feature->desc_str) labels[0] = feature->desc_str;
    else  labels[0] = "No description configured";

    if (feature->id_str) labels[1] = feature->id_str;
    else labels[1] = "No ID string configured";

    if (feature->enabled == 1) labels[2] = "Disable";
    else labels[2] = "Enable";

    row = i_form_table_add_row (item, labels);

    res_addr_str = i_resource_address_struct_to_string (RES_ADDR(self));
    i_form_table_add_link (item, 2, row, res_addr_str, "feature_control", feature->id_str, strlen(feature->id_str)+1);
    i_form_table_add_link (item, 3, row, res_addr_str, "feature_config", feature->id_str, strlen(feature->id_str)+1);
    i_form_table_add_link (item, 4, row, res_addr_str, "feature_remove", feature->id_str, strlen(feature->id_str)+1); 
    free (res_addr_str);
  }

  /* Finished */

  return 1;
}

int form_feature_add (i_resource *self, i_form_reqdata *reqdata)
{
  int i;
  int count;
  i_form *form;
  i_form_item *item;
  char *form_title = "Add new feature";

  if (!self || !reqdata) return -1;

  /* auth checking */

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }

  /* form creation */

  reqdata->form_out = i_form_create (1);
  if (!reqdata->form_out)
  { i_printf (1, "form_feature_add failed to create form"); return -1; }
  i_form_set_title (reqdata->form_out, "Add new feature");

  item = i_form_dropdown_create ("module_name", "Select Feature");
  if (!item) 
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add dropdown");  return 1; }
  i_form_add_item (reqdata->form_out, item);

  /* Add all the features configured in features.conf */

  count = i_configfile_instance_count (self, FEATURE_CONF_FILE, "features", "module");
  for (i=0; i < count; i++)
  {
    char *module_str;

    module_str = i_configfile_get (self, FEATURE_CONF_FILE, "features", "module", i);
    if (module_str)
    {
      i_resource_info *info;

      info = i_resource_info_module (self, module_str);
      if (info)
      {
        i_form_dropdown_add_option (item, info->module_name, info->type_str, 0);
        i_resource_info_free (info);
      }

      free (module_str);
    }
  }

  /* ID string */

  i_form_entry_add (reqdata->form_out, "id", "ID String", NULL);

  /* finished */

  return 1;
}

int form_feature_add_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  i_form_item_option *opt;
  l_feature *feature;
  i_resource_info *info;
  i_form_item *item;
  i_list *feature_list;

  if (!self || !reqdata) return -1;
  
  /* auth checking */

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF) 
  { return i_form_deliver_denied (self, reqdata); }

  /* form creation */

  reqdata->form_out = i_form_create (0);
  if (!reqdata->form_out)
  { i_printf (1, "form_feature_add_submit failed to create form"); return -1; }
  i_form_set_title (reqdata->form_out, "Adding feature");

  /* get module info */

  item = i_form_find_item (reqdata->form_in, "module_name");
  if (!item)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Module name item not found in submitted form"); return 1; }
  opt = i_form_dropdown_get_selected_value (item);
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Module name not found in submitted form"); return 1; }

  info = i_resource_info_module (self, (char *) opt->data);
  if (!info)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to get resource information from specified module"); return 1; }
  
  opt = i_form_get_value_for_item (reqdata->form_in, "id");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "ID String not found in submitted form"); return 1; }

  if (strlen((char *)opt->data) < 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "A device-unique ID string must be specified"); return 1; }

  /* Duplicate checking */

  feature = l_feature_get_by_id (self, (char *) opt->data);
  if (feature)
  { i_form_string_add (reqdata->form_out, "error", "Error", "A feature with the specified ID already exists"); return 1; }

  /* Create/Store/List feature */
  
  feature = l_feature_create ();
  if (!feature)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create feature data structure"); return 1; }

  feature->type = info->type;
  if (opt->data) feature->id_str = strdup ((char *)opt->data);
  if (info->type_str) feature->desc_str = strdup (info->type_str);
  if (info->module_name) feature->module_name = strdup (info->module_name);
  i_resource_info_free (info);

  num = l_featuredb_put (self, feature);
  if (num != 0)
  { 
    i_form_string_add (reqdata->form_out, "error", "Error", "Failed to put feature record into database"); 
    l_feature_free (feature); 
    return 1; 
  }

  feature_list = l_feature_list ();
  if (!feature_list)
  { 
    i_form_string_add (reqdata->form_out, "error", "Error", "Failed to retrieve feature list"); 
    l_featuredb_del (self, feature->id_str);
    l_feature_free (feature);
    return 1; 
  }
      
  i_list_enqueue (feature_list, feature);    /* DO NOT FREE feature, as it is now in the feature list */
  
  /* Finished */

  i_form_string_add (reqdata->form_out, "message", "Success", "Feature successfully added");

  return 1;
}

int form_feature_remove (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  char *str;

  if (!self || !reqdata) return -1;
  
  /* auth checking */

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF) 
  { return i_form_deliver_denied (self, reqdata); }

  /* form creation */
  
  reqdata->form_out = i_form_create (0);
  if (!reqdata->form_out)
  { i_printf (1, "form_feature_remove failed to create form"); return -1; }
  i_form_set_title (reqdata->form_out, "Removing feature");

  if (!reqdata->form_passdata || reqdata->form_passdata_size < 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No feature specified"); return 1; }

  /* feature removal */

  num = l_feature_remove_by_id (self, (char *) reqdata->form_passdata);
  if (num != 0)
  { 
    asprintf (&str, "Failed to remove feature \"%s\"", (char *) reqdata->form_passdata);
    i_form_string_add (reqdata->form_out, "error", "Error", str); 
    free (str);
    return 1; 
  }

  asprintf (&str, "Feature \"%s\" was successfully removed", (char *) reqdata->form_passdata);
  i_form_string_add (reqdata->form_out, "message", "Success", str);
  free (str);

  return 1;
}

