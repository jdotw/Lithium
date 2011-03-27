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

int form_case_logupdate (i_resource *self, i_form_reqdata *reqdata)
{
  i_callback *cb;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF) 
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_case_view failed to create form"); return -1; }

  /* Load owned cases */
  cb = l_case_sql_list (self, reqdata->form_passdata, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, l_case_logupdate_casecb, reqdata);
  if (!cb)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to query SQL database"); return 1; }

  return 0;   /* Not ready */
}

int l_case_logupdate_casecb (i_resource *self, i_list *list, void *passdata)
{
  char *str;
  char *pass_str;
  l_case *cas;
  i_form_item *item;
  i_form_reqdata *reqdata = passdata;

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

  /* Update log */
  i_form_frame_start (reqdata->form_out, "case_logupdate", "Log Update");
  i_form_textarea_add (reqdata->form_out, "comment", "Comment", NULL);
  i_form_entry_add (reqdata->form_out, "timespent", "Time Spent (minutes)", NULL);
  item = i_form_dropdown_create ("type", "Comment Type");
  i_form_dropdown_add_option (item, "worklog", "Work Log", 1);
  i_form_dropdown_add_option (item, "custcontact", "Customer Contact", 0);
  i_form_add_item (reqdata->form_out, item);
  asprintf (&pass_str, "%li", cas->id);
  i_form_hidden_add (reqdata->form_out, "caseid", pass_str);
  free (pass_str);
  i_form_frame_end (reqdata->form_out, "case_logupdate");

  /* Deliver form */
  i_form_deliver (self, reqdata);

  return -1;    /* Dont keep the case list */
}

int form_case_logupdate_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  long caseid;
  l_case_logentry *log;
  i_form_item_option *comment_opt;
  i_form_item_option *type_opt;
  i_form_item_option *timespent_opt;
  i_form_item_option *caseid_opt;

  /* Auth checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_case_logupdate_submit unable to create form"); return -1; }

  /* Fields */
  caseid_opt = i_form_get_value_for_item (reqdata->form_in, "caseid");
  if (!caseid_opt) { i_form_string_add (reqdata->form_out, "error", "Error", "Case ID not found"); return 1; }
  type_opt = i_form_get_value_for_item (reqdata->form_in, "type");
  if (!type_opt) { i_form_string_add (reqdata->form_out, "error", "Error", "Type not found"); return 1; }
  comment_opt = i_form_get_value_for_item (reqdata->form_in, "comment");
  if (!comment_opt) { i_form_string_add (reqdata->form_out, "error", "Error", "Comment not found"); return 1; }
  timespent_opt = i_form_get_value_for_item (reqdata->form_in, "timespent");
  if (!timespent_opt) { i_form_string_add (reqdata->form_out, "error", "Error", "Time spent not found"); return 1; }

  /* Create log entry */
  log = l_case_logentry_create ();
  log->type = atoi ((char *) type_opt->data);
  gettimeofday (&log->tstamp, NULL);
  log->timespent_sec = atol ((char *) timespent_opt->data) * 60;
  log->author_str = strdup (reqdata->auth->username);
  log->entry_str = strdup ((char *) comment_opt->data);
  caseid = atol ((char *) caseid_opt->data);

  /* Insert */
  num = l_case_logentry_sql_insert (self, caseid, log);
  l_case_logentry_free (log);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add comment to SQL database"); return 1; }
  
  /* Call case view form */
  asprintf ((char **) &reqdata->form_passdata, "%li", caseid);
  return form_case_view (self, reqdata);
}

