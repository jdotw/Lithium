#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/auth.h>

/* The form_node_config functions */

extern i_hashtable* global_res_table;
static i_list* res_list;

void form_resource_list_iterate (i_resource *self, i_hashtable *res_table, void *data)
{
  i_list_enqueue (res_list, data);
}

int form_resource_list (i_resource *self, i_form_reqdata *reqdata)
{
  int count=0;
  i_resource *res;
  i_form_item *item;
  char *labels[8];
  char *form_title;

  if (!self || !reqdata) return -1;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out) 
  { i_printf (1, "form_resource_list failed to create form"); return -1; }

  asprintf (&form_title, "Resource list on node %s:%s", self->plexus, self->node);
  i_form_set_title (reqdata->form_out, form_title);
  free (form_title);

  res_list = i_list_create ();
  if (!res_list)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create res_list"); return 1; }
  i_hashtable_iterate (NULL, global_res_table, form_resource_list_iterate);             /* Iterate through the hash table and run the func */

  item = i_form_table_create (reqdata->form_out, "res_list", "Resource List", 8);
  if (!item) 
  { i_form_string_add (reqdata->form_out, "erorr", "Error", "Failed to create form table"); i_list_free (res_list); return 1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);      
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);        

  labels[0] = "Identification";
  labels[1] = "Customer";
  labels[2] = "Type";
  labels[3] = "Restarts";
  labels[4] = "Config";
  labels[5] = "Info";
  labels[6] = "Restart";
  labels[7] = "State";
  i_form_table_add_row (item, labels);
  
  for (i_list_move_head(res_list); (res=i_list_restore(res_list)) != NULL; i_list_move_next(res_list))
  {
    int row;
    char *res_addr_str;
    char *ident_str;
    i_resource_info *info;

    if (res->ident_str) ident_str = res->ident_str;
    else ident_str = " ";
    asprintf (&labels[0], "%s:%i", ident_str, res->ident_int);

    if (res->customer_id) labels[1] = strdup (res->customer_id);
    else labels[1] = strdup ("N/A");
    
    info = i_resource_info_local (res->type);
    if (!info)  /* Try from the module */
    { info = i_resource_info_module (self, res->module_name); }

    if (info) 
    {
      if (info->type_str) labels[2] = strdup (info->type_str);
      else labels[2] = strdup ("Not present");
      i_resource_info_free (info);
    }        
    else
    { labels[2] = strdup("Unknown"); }

    asprintf (&labels[3], "%i", res->restart_count);

    labels[7] = i_resource_state_str (res);

    row = i_form_table_add_row (item, labels);
    count++;

    res_addr_str = i_resource_address_struct_to_string (RES_ADDR(res));
    i_form_table_add_link (item, 4, row, RES_ADDR(res), NULL, "resource_config", 0, NULL, 0);                       
    i_form_table_add_link (item, 5, row, RES_ADDR(self), NULL, "resource_info", 0, res_addr_str, strlen(res_addr_str)+1); 
    i_form_table_add_link (item, 6, row, RES_ADDR(self), NULL, "resource_restart", 0, res_addr_str, strlen(res_addr_str)+1);
    if (res_addr_str) free (res_addr_str);

    free (labels[0]);
    free (labels[1]);
    free (labels[2]);
    free (labels[3]);
  }

  i_list_free (res_list);
  
  return 1;
}

int form_resource_list_raw (i_resource *self, i_form_reqdata *reqdata)
{
  i_resource *res;
  char *form_title;

  if (!self || !reqdata) return -1;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_resource_list_raw unable to create form"); return -1; }

  asprintf (&form_title, "Resource list on node %s:%s", self->plexus, self->node);
  i_form_set_title (reqdata->form_out, form_title);
  free (form_title);

  res_list = i_list_create ();
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create res_list"); return 1; }
  i_hashtable_iterate (NULL, global_res_table, form_resource_list_iterate);             /* Iterate through the hash table and run the func */

  for (i_list_move_head(res_list); (res=i_list_restore(res_list)) != NULL; i_list_move_next(res_list))
  {
    char *type_str;
    char *res_addr_str;
    i_resource_info *info;

    info = i_resource_info_local (res->type);
    if (!info)
    { info = i_resource_info_module (self, res->module_name); }

    if (info)
    {
      type_str = info->type_str;
      i_resource_info_free (info);
    }
    else
    { type_str = "Unknown"; }        

    res_addr_str = i_resource_address_struct_to_string (RES_ADDR(res));
    i_form_string_add (reqdata->form_out, res_addr_str, res_addr_str, type_str);
    free (res_addr_str);
  }

  i_list_free (res_list);

  return 1;
}
