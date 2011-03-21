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

/* form_infstat_issue_close - Close an issue */

int form_infstat_issue_close (i_resource *self, i_form_reqdata *reqdata)
{
  char *name_str;
  char *id_str;
  l_infstat_cat *cat;
  l_infstat_issue *issue;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied(self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_infstat_issue_close failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "issueclose", "Infrastructure Status - Close Issue");

  /* State check */
  if (l_infstat_cat_load_inprogress() == 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Issues can not be closed whilst the category list is being loaded. Please try again when the category list load has complete"); return 1; }

  /* Find category and ID */

  if (reqdata->form_passdata && reqdata->form_passdata_size > 0)
  {
    name_str = (char *) reqdata->form_passdata;
    id_str = strchr (name_str, ':');
    if (!id_str)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Issue ID not specified"); return 1; }
    *id_str = '\0';
    id_str++;

    cat = l_infstat_cat_get (self, name_str);
    if (!cat)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified category not found"); return 1; }
    i_form_hidden_add (reqdata->form_out, "cat", cat->name_str);

    issue = l_infstat_issue_get (self, cat, atol(id_str));
    if (!issue)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified issue not found"); return 1; }
    i_form_hidden_add (reqdata->form_out, "id", id_str);
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No category name or issue ID specified"); return 1; }

  /* Closing Note */
  i_form_textarea_add (reqdata->form_out, "closenote", "Closing Note", NULL);

  i_form_frame_end (reqdata->form_out, "issueclose");

  /* Finished */
  return 1;
}

int form_infstat_issue_close_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  i_form_item_option *opt;
  l_infstat_cat *cat;
  l_infstat_issue *issue;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }

  /* Create the form */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_infstat_issue_close_submit failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "closeissue", "Infrastructure Status - Close Issue");

  /* Find the category */
  opt = i_form_get_value_for_item (reqdata->form_in, "cat");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Category name not found in form"); return 1; }
  cat = l_infstat_cat_get (self, (char *) opt->data);
  if (!cat)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified category not found"); return 1; }

  /* Find the issue */
  opt = i_form_get_value_for_item (reqdata->form_in, "id");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Issue ID not found in form"); return 1; }
  issue = l_infstat_issue_get (self, cat, atol((char *)opt->data));

  /* Issue info */
  opt = i_form_get_value_for_item (reqdata->form_in, "closenote");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Closing note not found in form"); return 1; }
  issue->closenote_str = strdup (opt->data);

  /* Close the issue */
  num = l_infstat_issue_close (self, cat, issue);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to close issue"); return 1; }

  /* Success */
  i_form_string_add (reqdata->form_out, "msg", "Success", "Issue closed successfully");

  i_form_frame_end (reqdata->form_out, "closeissue");

  return 1;
}
