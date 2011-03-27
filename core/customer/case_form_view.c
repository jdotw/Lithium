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
#include <induction/postgresql.h>
#include <induction/timeutil.h>
#include <induction/callback.h>

#include "case.h"

int form_case_view (i_resource *self, i_form_reqdata *reqdata)
{
  i_callback *cb;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF) 
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_case_view failed to create form"); return -1; }

  /* Load owned cases */
  cb = l_case_sql_list (self, reqdata->form_passdata, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, l_case_form_view_casecb, reqdata);
  if (!cb)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to query SQL database"); return 1; }

  return 0;   /* Not ready */
}

int l_case_form_view_casecb (i_resource *self, i_list *list, void *passdata)
{
  int row;
  char *str;
  char *pass_str;
  l_case *cas;
  i_form_item *item;
  i_form_reqdata *reqdata = passdata;
  char *control_labels[4];
  i_callback *cb;

  /* Get case */
  i_list_move_head (list);
  cas = i_list_restore (list);
  if (!cas)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified case not found"); i_form_deliver (self, reqdata); return -1; }

  /* Start frame */
  asprintf (&str, "%s Case %li", self->hierarchy->cust->desc_str, cas->id);
  i_form_frame_start (reqdata->form_out, "case_view", str);
  free (str);

  /* Case info */
  i_form_string_add (reqdata->form_out, "hline", "Headline", cas->hline_str);
  asprintf (&str, "%li", cas->id);
  i_form_string_add (reqdata->form_out, "id", "Case #", str);
  free (str);
  i_form_string_add (reqdata->form_out, "owner", "Owner", cas->owner_str);
  i_form_string_add (reqdata->form_out, "requester", "Requester", cas->requester_str);
  i_form_string_add (reqdata->form_out, "state", "State", l_case_statestr (cas->state));
  str = i_time_ctime (cas->start.tv_sec);
  i_form_string_add (reqdata->form_out, "start", "Opened", str);
  free (str);
  if (cas->end.tv_sec > 0)
  {
    str = i_time_ctime (cas->end.tv_sec);
    i_form_string_add (reqdata->form_out, "end", "Closed", str);
    free (str);
  }

  /* End frame */
  i_form_frame_end (reqdata->form_out, "case_view"); 

  /* Administration */
  i_form_frame_start (reqdata->form_out, "case_control", "Case Control");
  item = i_form_table_create (reqdata->form_out, "control_table", NULL, 4);
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);
  i_form_item_add_option (item, ITEM_OPTION_NOHEADERS, 0, 0, 0, NULL, 0);
  control_labels[0] = "Refresh";
  control_labels[1] = "Update Log";
  control_labels[2] = "Case Administration";
  control_labels[3] = "Close Case";
  row = i_form_table_add_row (item, control_labels);
  asprintf (&pass_str, "%li", cas->id);
  i_form_table_add_link (item, 0, row, NULL, NULL, "case_view", 0, pass_str, strlen(pass_str)+1);
  i_form_table_add_link (item, 1, row, NULL, NULL, "case_logupdate", 0, pass_str, strlen(pass_str)+1);
  i_form_table_add_link (item, 2, row, NULL, NULL, "case_edit", 0, pass_str, strlen(pass_str)+1);
  i_form_table_add_link (item, 3, row, NULL, NULL, "case_close", 0, pass_str, strlen(pass_str)+1);
  free (pass_str);
  i_form_frame_end (reqdata->form_out, "case_control");

  /* Load log entries */
  cb = l_case_logentry_sql_list (self, cas->id, l_case_form_view_logcb, reqdata);
  if (!cb)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to query log entries"); i_form_deliver (self, reqdata); return -1; }

  return -1;    /* Dont keep the case list */
}

int l_case_form_view_logcb (i_resource *self, i_list *list, void *passdata)
{
  int index=0;
  l_case_logentry *log;
  i_form_reqdata *reqdata = passdata;

  /* Start frame */
  i_form_frame_start (reqdata->form_out, "case_log", "Case Log");

  /* Log entries */
  for (i_list_move_head(list); (log=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    char *time_str;

    if (index > 0) i_form_hrline_add (reqdata->form_out);

    time_str = i_time_ctime (log->tstamp.tv_sec);
    i_form_string_add (reqdata->form_out, "entry", time_str, log->entry_str);
    free (time_str);
    i_form_string_add (reqdata->form_out, "author", "Author", log->author_str);

    index++;
  }

  /* End frame */
  i_form_frame_end (reqdata->form_out, "case_log");

  /* Deliver */
  i_form_deliver (self, reqdata);

  return -1; /* Dont keep list */
}

