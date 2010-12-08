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
#include <induction/timeutil.h>
#include <induction/callback.h>

#include "case.h"

int form_case_close (i_resource *self, i_form_reqdata *reqdata)
{
  i_callback *cb;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF) 
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_case_close failed to create form"); return -1; }

  /* Check passdata */
  if (!reqdata->form_passdata)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No case specified"); return 1; }

  /* Load specified cases */
  cb = l_case_sql_list (self, (char *) reqdata->form_passdata, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, l_case_form_close_casecb, reqdata);
  if (!cb)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to query SQL database"); return 1; }

  return 0;   /* Not ready */
}

int l_case_form_close_casecb (i_resource *self, i_list *list, void *passdata)
{
  char *str;
  l_case *cas;
  i_form_reqdata *reqdata = passdata;

  /* Get case */
  i_list_move_head (list);
  cas = i_list_restore (list);
  if (!cas)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified case not found"); i_form_deliver (self, reqdata); return -1; }

  /* Start frame */
  asprintf (&str, "%s Case %li", self->hierarchy->cust->desc_str, cas->id);
  i_form_frame_start (reqdata->form_out, "case_close", str);
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
  i_form_frame_end (reqdata->form_out, "case_close"); 

  /* Log entry */
  i_form_frame_start (reqdata->form_out, "case_log", "Final Log Entry");
  i_form_textarea_add (reqdata->form_out, "logentry", "Log Entry", NULL);
  i_form_frame_end (reqdata->form_out, "case_log");
  
  /* Confirm frame */
  i_form_frame_start (reqdata->form_out, "case_close", "Confirm");
  i_form_string_add (reqdata->form_out, "confirmstr", "Confirm", "Click submit to close case"); 
  i_form_hidden_add (reqdata->form_out, "caseid", (char *) reqdata->form_passdata);
  i_form_frame_end (reqdata->form_out, "case_close");

  /* Deliver form */
  i_form_deliver (self, reqdata);

  return -1;    /* Dont keep the case list */
}

int form_case_close_submit (i_resource *self, i_form_reqdata *reqdata)
{
  i_callback *cb;
  i_form_item_option *id_opt;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_case_close failed to create form"); return -1; }

  /* Get ID */
  id_opt = i_form_get_value_for_item (reqdata->form_in, "caseid");
  if (!id_opt || !id_opt->data || strlen ((char *)id_opt->data) < 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Case ID not found"); return 1; }

  /* Load case */
  cb = l_case_sql_list (self, (char *) id_opt->data, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, l_case_form_close_submit_casecb, reqdata);
  if (!cb)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to query SQL database"); return 1; }

  return 0;
}

int l_case_form_close_submit_casecb (i_resource *self, i_list *list, void *passdata)
{
  int num;
  char *str;
  l_case *cas;
  i_form_item_option *logentry_opt;
  i_form_reqdata *reqdata = passdata;

  /* Get case */
  i_list_move_head (list);
  cas = i_list_restore (list);
  if (!cas)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified case not found"); i_form_deliver (self, reqdata); return -1; }

  /* Start frame */
  asprintf (&str, "%s Case %li", self->hierarchy->cust->desc_str, cas->id);
  i_form_frame_start (reqdata->form_out, "case_close", str);
  free (str);

  /* Set Update case */
  gettimeofday (&cas->end, NULL);
  cas->state = CASE_STATE_CLOSED;

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
  i_form_frame_end (reqdata->form_out, "case_close");

  /* Log entry */
  logentry_opt = i_form_get_value_for_item (reqdata->form_in, "logentry");
  if (logentry_opt->data)
  {
    /* Create entry */
    l_case_logentry *log;
    log = l_case_logentry_create ();
    gettimeofday (&log->tstamp, NULL);
    log->author_str = strdup (reqdata->auth->username);
    log->entry_str = strdup ((char *) logentry_opt->data);

    /* Insert log entry */
    num = l_case_logentry_sql_insert (self, cas->id, log);
    l_case_logentry_free (log);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to insert initial log comment"); }
  }

  /* SQL Update */
  i_form_frame_start (reqdata->form_out, "case_close", "Closing case");
  num = l_case_sql_update (self, cas);
  if (num == 0)
  { i_form_string_add (reqdata->form_out, "result", "Result", "Case Closed"); }
  else
  { i_form_string_add (reqdata->form_out, "result", "Result", "Failed to update case"); }
  i_form_frame_end (reqdata->form_out, "case_close");

  /* Deliver form */
  i_form_deliver (self, reqdata);

  return -1;    /* Dont keep the case list */
}
