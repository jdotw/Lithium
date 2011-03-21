#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/form.h>
#include <induction/construct.h>
#include <induction/module.h>
#include <induction/socket.h>

/* The form_node_config functions */

extern i_hashtable *global_res_table;

void l_form_resource_info_links (i_resource *self, i_resource *res, i_form *form, char *pass_data, int pass_datasize)
{
  int row;
  i_form_item *links;
  char *action_labels[2] = { "Configure", "Remove" };
  i_resource_info *info;

  /* Create the table */
  
  links = i_form_table_create (form, "action", NULL, 2);
  if (!links) { i_form_string_add (form, "error", "unable to create action table", NULL); return; }

  /* Get resource type info */

  info = i_resource_info_local (res->type);
  if (!info)
  { info = i_resource_info_module (self, res->module_name); }
  
  if (!info) 
  { i_form_string_add (form, "error", "unable to get resource type info", NULL); return; }

  /* Set up row labels */

  if (info->user_spawn == 1 && res->no_remove == 0) action_labels[1] = "Remove";
  else action_labels[1] = "";

  /* Add row */
  
  row = i_form_table_add_row (links, action_labels);

  /* Add links */

  i_form_table_add_link (links, 0, row, RES_ADDR(res), NULL, "resource_config", 0, NULL, 0);

  if (info->user_spawn == 1 && res->no_remove == 0)
  {
    /* Res is user spawnable/(removable) and doesnt have the no_remove flag */
    i_form_table_add_link (links, 1, row, RES_ADDR(self), NULL, "resource_remove", 0, pass_data, pass_datasize);
  }

  /* Clean up */
  
  i_resource_info_free (info);
}

int form_resource_info (i_resource *self, i_form_reqdata *reqdata)
{
  i_resource_address *res_addr;
  i_resource *res;
  i_resource_info *info;
  char *temp;
  char *submit_label = "Submit Changes";

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out) { i_printf (1, "form_resource_info unable to create form"); return -1; }
  i_form_set_title (reqdata->form_out, "Resource Information");
  i_form_option_add (reqdata->form_out, FORM_OPTION_SUBMIT_LABEL, submit_label, strlen(submit_label)+1);

  if (!reqdata->form_passdata) 
  { i_form_string_add (reqdata->form_out, "error", "Error", "No resource specified"); return 1; }
  
  res_addr = i_resource_address_string_to_struct (reqdata->form_passdata);
  if (!res_addr) 
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to convert supplied resource address string to an address struct"); return 1; }
  
  res = i_resource_local_get (global_res_table, res_addr);
  i_resource_free_address (res_addr);
  if (!res) 
  { i_form_string_add (reqdata->form_out, "Error", "Error", "Failed to find specified resource"); return 1; }

  /* Add the links table at the top */

  l_form_resource_info_links (self, res, reqdata->form_out, reqdata->form_passdata, reqdata->form_passdata_size);
  i_form_spacer_add (reqdata->form_out);
   
  /* Add the resource type string */

  info = i_resource_info_local (res->type);
  if (!info)
  { info = i_resource_info_module (self, res->module_name); }

  if (!info) 
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to resource information for specified resource"); return 1; }

  i_form_string_add (reqdata->form_out, "type", "Resource Type", info->type_str);

  i_resource_info_free (info);

  /* Add the ident_int entry */

  asprintf (&temp, "%i", res->ident_int);
  if (!temp) 
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create ident_int string"); return 1; }
  i_form_string_add (reqdata->form_out, "ident_int", "ID Number", temp);
  free (temp);

  /* Add the ident_str entry */

  i_form_string_add (reqdata->form_out, "ident_str", "ID String", res->ident_str);

  /* Add a whole bunch of other stuff from the res/construct struct */

  i_form_string_add (reqdata->form_out, "plexus", "Plexus", res->plexus);
  i_form_string_add (reqdata->form_out, "node", "Node", res->node);
  
  switch (res->state)
  {
    case RES_STATE_RUN: i_form_string_add (reqdata->form_out, "state", "Resource State", "Running");
                        break;
    case RES_STATE_SLEEP: i_form_string_add (reqdata->form_out, "state", "Resource State", "Sleeping");
                          break;
    default: i_form_string_add (reqdata->form_out, "state", "Resource State", "Unknown");
  }

  if (res->core_socket)
  {
    asprintf (&temp, "%i", res->core_socket->sockfd);
    if (!temp) { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create sockfd string"); return 1; }
    i_form_string_add (reqdata->form_out, "sockfd", "Socket sockfd", temp);
    free (temp);
  }
  else i_form_string_add (reqdata->form_out, "sockfd", "Socket sockfd", "Not Connected");

  if (res->construct)
  {
    i_form_string_add (reqdata->form_out, "module_path", "Module path", res->construct->module_path);
    i_form_string_add (reqdata->form_out, "config_path", "Config path", res->construct->config_path);   
  
    asprintf (&temp, "%i", res->construct->pid);
    if (!temp) { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create pid string"); return 1; }
    i_form_string_add (reqdata->form_out, "pid", "Process ID", temp);
    free (temp);
  
    asprintf (&temp, "%i", res->construct->ppid);
    if (!temp) { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create ppid string"); return 1; }
    i_form_string_add (reqdata->form_out, "ppid", "Parent Process ID", temp);
    free (temp);
  
    asprintf (&temp, "%i", res->construct->id);
    if (!temp) { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create id string"); return 1; }
    i_form_string_add (reqdata->form_out, "session_id", "Resource Session ID", temp);
    free (temp);
  }
  
  i_form_string_add (reqdata->form_out, "spawn_time", "Spawn time", ctime ((time_t *)&res->spawn_time.tv_sec));
  i_form_string_add (reqdata->form_out, "life_time", "Last life sign", ctime ((time_t *)&res->life_sign.tv_sec));
  i_form_string_add (reqdata->form_out, "wake_time", "Wakeup time", ctime ((time_t *)&res->wake_time.tv_sec));
  i_form_string_add (reqdata->form_out, "module_name", "Module name", res->module_name);
  i_form_string_add (reqdata->form_out, "root", "File System Root", res->root);
    
  asprintf (&temp, "%i", res->restart_count);
  if (!temp) { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create restart_count string"); return 1; }
  i_form_string_add (reqdata->form_out, "restart_count", "Restart count", temp);
  free (temp);

  asprintf (&temp, "%i", res->fast_restart_count);
  if (!temp) { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create fast_restart_count string"); return 1; }
  i_form_string_add (reqdata->form_out, "fast_restart_count", "Fast restart count since last sleep", temp);
  free (temp);

  if (res->owner)
  {
    temp = i_resource_address_struct_to_string (res->owner);
    i_form_string_add (reqdata->form_out, "owner", "Resource Owner", temp);
    free (temp);
  }
  else i_form_string_add (reqdata->form_out, "owner", "Resource Owner", "Unknown");

  /* Add the links again at the bottom */

  i_form_spacer_add (reqdata->form_out);
  l_form_resource_info_links (self, res, reqdata->form_out, reqdata->form_passdata, reqdata->form_passdata_size);

  /* All done, return the form */

  return 1;
}

