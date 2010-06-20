#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/hierarchy.h>

#include "infstat.h"

/* form_infstat_cat_remove - Remove a category */

int form_infstat_cat_remove (i_resource *self, i_form_reqdata *reqdata)
{
  char *name_str;
  l_infstat_cat *cat;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied(self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_infstat_cat_remove failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "formremove", "Infrastructure Status - Remove Category");

  /* State check */
  if (l_infstat_cat_load_inprogress() == 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Categories can not be removed whilst the category list is being loaded. Please try again when the category list load has complete"); return 1; }

  /* Find category and ID */

  if (reqdata->form_passdata && reqdata->form_passdata_size > 0)
  {
    name_str = (char *) reqdata->form_passdata;

    cat = l_infstat_cat_get (self, name_str);
    if (!cat)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified category not found"); return 1; }
    i_form_hidden_add (reqdata->form_out, "cat", cat->name_str);
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No category name specified"); return 1; }

  /* Confirm */
  i_form_string_add (reqdata->form_out, "confirm", "Confirm", "Please confirm category removal");

  i_form_frame_end (reqdata->form_out, "catremove");
  
  /* Finished */
  return 1;
}

int form_infstat_cat_remove_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  i_form_item_option *opt;
  l_infstat_cat *cat;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }

  /* Create the form */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_infstat_cat_remove_submit failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "catremove", "Infrastructure Status - Remove Category");

  /* Find the category */
  opt = i_form_get_value_for_item (reqdata->form_in, "cat");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Category name not found in form"); return 1; }
  cat = l_infstat_cat_get (self, (char *) opt->data);
  if (!cat)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified category not found"); return 1; }

  /* Remove the category */
  num = l_infstat_cat_remove (self, cat);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to remove the category"); return 1; }

  /* Success */
  i_form_string_add (reqdata->form_out, "msg", "Success", "Category removed successfully");

  i_form_frame_end (reqdata->form_out, "catremove");

  return 1;
}
