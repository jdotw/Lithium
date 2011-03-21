#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/module.h>
#include <induction/auth.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/construct.h>
#include <induction/customer.h>
#include <induction/hierarchy.h>
#include <induction/list.h>
#include <induction/user.h>
#include <induction/userdb.h>
#include <induction/postgresql.h>
#include <induction/callback.h>

#include "case.h"

int form_case_main (i_resource *self, i_form_reqdata *reqdata)
{
  i_callback *cb;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF) 
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_case_main failed to create form"); return -1; }

  /* Load owned cases */
  cb = l_case_sql_list (self, NULL, CASE_STATE_OPEN_NUMSTR, NULL, NULL, NULL, NULL, NULL, reqdata->auth->username, NULL, l_case_form_main_casecb, reqdata);
  if (!cb)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to query SQL database"); return 1; }

  return 0;   /* Not ready */
}

int l_case_form_main_casecb (i_resource *self, i_list *list, void *passdata)
{
  char *str;
  l_case *cas;
  i_form_item *item;
  i_form_reqdata *reqdata = passdata;
  char *labels[5];

  /* Start frame */
  asprintf (&str, "%s Cases Owned by %s", self->hierarchy->cust->desc_str, reqdata->auth->username);
  i_form_frame_start (reqdata->form_out, "case_main", str);
  free (str);

  /* Case table */
  item = i_form_table_create (reqdata->form_out, "case_list", NULL, 5);
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);
  labels[0] = "Case #";
  labels[1] = "Headline";
  labels[2] = "State";
  labels[3] = "Opened";
  labels[4] = "Update";
  i_form_table_add_row (item, labels);

  /* Iterate cases */
  for (i_list_move_head(list); (cas=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    int row;
    char *pass_str;
    
    asprintf (&labels[0], "%li", cas->id);
    labels[1] = cas->hline_str;
    labels[2] = l_case_statestr (cas->state);
    labels[3] = ctime ((time_t *) &cas->start.tv_sec);

    row = i_form_table_add_row (item, labels);

    asprintf (&pass_str, "%li", cas->id);
    i_form_table_add_link (item, 0, row, NULL, NULL, "case_view", 0, pass_str, strlen(pass_str)+1);
    i_form_table_add_link (item, 1, row, NULL, NULL, "case_view", 0, pass_str, strlen(pass_str)+1);
    i_form_table_add_link (item, 4, row, NULL, NULL, "case_logupdate", 0, pass_str, strlen(pass_str)+1);
    free (pass_str);
  }

  i_form_spacer_add (reqdata->form_out);
  item = i_form_string_add (reqdata->form_out, "open", "Open New Case", NULL);
  i_form_item_add_link (item, 0, 0, 0, NULL, NULL, "case_edit", 0, NULL, 0);

  /* End frame */
  i_form_frame_end (reqdata->form_out, "case_main"); 

  /* Search frame */
  i_form_frame_start (reqdata->form_out, "case_search", "Case Search");
  i_form_entry_add (reqdata->form_out, "caseid", "Case #", NULL);
  i_form_entry_add (reqdata->form_out, "hline", "Headline", NULL);
  i_form_entry_add (reqdata->form_out, "owner", "Owner", NULL);
  i_form_frame_end (reqdata->form_out, "case_search");
  
  /* Deliver form */
  i_form_deliver (self, reqdata);

  return -1;    /* Dont keep the case list */
}

int form_case_main_submit (i_resource *self, i_form_reqdata *reqdata)
{
  char *id_str = NULL;
  char *hline_str = NULL;
  char *owner_str = NULL;
  i_form_item_option *id_opt;
  i_form_item_option *hline_opt;
  i_form_item_option *owner_opt;
  i_callback *cb;

  /* Auth checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }
  
  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_case_add_submit unable to create form"); return -1; }
  
  /* Fields */
  id_opt = i_form_get_value_for_item (reqdata->form_in, "caseid");
  if (id_opt && strlen((char *)id_opt->data) > 0) id_str = (char *) id_opt->data;
  hline_opt = i_form_get_value_for_item (reqdata->form_in, "hline");
  if (hline_opt && strlen((char *)hline_opt->data) > 0) hline_str = (char *) hline_opt->data;
  owner_opt = i_form_get_value_for_item (reqdata->form_in, "owner");
  if (owner_opt && strlen((char *)owner_opt->data) > 0) owner_str = (char *) owner_opt->data;

  /* Perform search */
  cb = l_case_sql_list (self, id_str, NULL, NULL, NULL, NULL, NULL, hline_str, owner_str, NULL, l_case_form_main_submit_casecb, reqdata);
  if (!cb)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to query SQL database"); return 1; }

  return 0;
}

int l_case_form_main_submit_casecb (i_resource *self, i_list *list, void *passdata)
{
  char *str;
  l_case *cas;
  i_form_item *item;
  i_form_reqdata *reqdata = passdata;
  char *labels[5];

  /* Start frame */
  asprintf (&str, "%s Case Search Results", self->hierarchy->cust->desc_str);
  i_form_frame_start (reqdata->form_out, "case_search", str);
  free (str);

  /* Case table */
  item = i_form_table_create (reqdata->form_out, "case_list", NULL, 5);
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);
  labels[0] = "Case #";
  labels[1] = "Headline";
  labels[2] = "Owner";
  labels[3] = "State";
  labels[4] = "Opened";
  i_form_table_add_row (item, labels);

  /* Iterate cases */
  for (i_list_move_head(list); (cas=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    int row;
    char *pass_str;

    asprintf (&labels[0], "%li", cas->id);
    labels[1] = cas->hline_str;
    labels[2] = cas->owner_str;
    labels[3] = l_case_statestr (cas->state);
    labels[4] = ctime ((time_t *) &cas->start.tv_sec);

    row = i_form_table_add_row (item, labels);

    asprintf (&pass_str, "%li", cas->id);
    i_form_table_add_link (item, 0, row, NULL, NULL, "case_view", 0, pass_str, strlen(pass_str)+1);
    i_form_table_add_link (item, 1, row, NULL, NULL, "case_view", 0, pass_str, strlen(pass_str)+1);
    free (pass_str);
  }

  /* End frame */
  i_form_frame_end (reqdata->form_out, "case_search");

  i_form_deliver (self, reqdata);

  return -1;    /* Dont keep case list */
}
