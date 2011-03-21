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

/* form_infstat_cat_edit - Add/Edit new Category */

int form_infstat_cat_edit (i_resource *self, i_form_reqdata *reqdata)
{
  char *frame_title;
  l_infstat_cat *cat = NULL;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied(self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_infstat_cat_edit failed to create form"); return -1; }

  if (reqdata->form_passdata && reqdata->form_passdata_size > 0)
  {
    char *name_str;
    name_str = (char *) reqdata->form_passdata;
    cat = l_infstat_cat_get (self, name_str);
    if (cat)
    { asprintf (&frame_title, "Infrastructure Status - Edit Category '%s'", cat->desc_str); }
    else
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified category not found"); return 1; }
    i_form_option_add (reqdata->form_out, FORM_OPTION_SUBMIT_LABEL, "Update", 7);
  }
  else
  { 
    asprintf (&frame_title, "Infrastructure Status - Add New Category"); 
    i_form_option_add (reqdata->form_out, FORM_OPTION_SUBMIT_LABEL, "Add", 4);
  }
  i_form_frame_start (reqdata->form_out, "catedit", frame_title);
  free (frame_title);

  /* Entries */
  if (cat) { i_form_string_add (reqdata->form_out, "name", "Category Name", cat->name_str); }
  else 
  { 
    i_form_entry_add (reqdata->form_out, "name", "Category Name", NULL); 
    i_form_string_add (reqdata->form_out, "name_note", "", "Category name must not begin with a number");
  }
  if (cat) { i_form_entry_add (reqdata->form_out, "desc", "Category Description", cat->desc_str); }
  else { i_form_entry_add (reqdata->form_out, "desc", "Category Description", NULL); }

  i_form_frame_end (reqdata->form_out, "catedit");

  /* Finished */
  return 1;
}

int form_infstat_cat_edit_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  char *name_str;
  char *desc_str;
  i_form_item_option *opt;
  l_infstat_cat *cat;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN)
  { return i_form_deliver_denied (self, reqdata); }

  /* Create the form */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_infstat_cat_add_submit failed to create form"); return -1; }

  /* Get values */
  opt = i_form_get_value_for_item (reqdata->form_in, "name");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Category name not found in form"); return 1; }
  name_str = strdup (opt->data);
  opt = i_form_get_value_for_item (reqdata->form_in, "desc");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Category description not found in form"); return 1; }
  desc_str = strdup (opt->data);

  /* Check for existing cat */
  cat = l_infstat_cat_get (self, name_str);
  if (cat)
  {
    /* Updating an existing cat */
    i_form_frame_start (reqdata->form_out, "catedit", "Infrastructure Status - Update Category");

    /* Update the struct values */
    if (cat->desc_str) { free (cat->desc_str); }
    cat->desc_str = strdup (desc_str);

    /* Update the category in SQL */
    num = l_infstat_cat_update (self, cat);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Warning", "Failed to update SQL database"); }

    /* Success */
    i_form_string_add (reqdata->form_out, "msg", "Success", "Existing category updated successfully");
  }
  else
  { 
    /* Adding a new cat */
    i_form_frame_start (reqdata->form_out, "catedit", "Infrastructure Status - Add Category");
    
    /* Create the struct */
    cat = l_infstat_cat_create ();
    if (!cat)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Faild to create category struct"); return 1; }

    /* Fill the struct */
    cat->name_str = strdup (name_str);
    cat->desc_str = strdup (desc_str);

    /* Add the category */
    num = l_infstat_cat_add (self, cat);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add category"); l_infstat_cat_free (cat); return 1; }

    /* Success */
    i_form_string_add (reqdata->form_out, "msg", "Success", "New category added successfully");
  }

  i_form_frame_end (reqdata->form_out, "catedit");

  return 1;
}
