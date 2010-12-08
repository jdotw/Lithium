#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/hashtable.h>

/* The form_node_config functions */

extern i_hashtable *global_res_table;

int form_resource_restart (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  i_resource_address *res_addr;
  i_resource *res;
  char *form_title;

  if (!self || !reqdata) return -1;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }
  
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out) 
  { i_printf (1, "form_resource_info unable to create form"); return -1; }

  if (!reqdata->form_passdata) 
  { i_form_string_add (reqdata->form_out, "error", "Error", "No resource specified"); return 1; }
  
  asprintf (&form_title, "Restart Resource %s", reqdata->form_passdata);
  i_form_set_title (reqdata->form_out, form_title);
  free (form_title);

  res_addr = i_resource_address_string_to_struct (reqdata->form_passdata);
  if (!res_addr) 
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to convert specified resource address string to an address struct"); return 1; }
  
  res = i_resource_local_get (global_res_table, res_addr);
  i_resource_free_address (res_addr);
  if (!res) 
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to find specificed resource"); return 1; }

  num = i_resource_local_restart (self, global_res_table, res);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Restart operation failed"); return 1; }

  i_form_string_add (reqdata->form_out, "success", "Success", "Restart completed successfully");
  
  /* All done */

  return 1;
}

