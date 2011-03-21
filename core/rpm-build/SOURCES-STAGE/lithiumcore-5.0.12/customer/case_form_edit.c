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
#include <induction/list.h>
#include <induction/user.h>
#include <induction/userdb.h>
#include <induction/postgresql.h>
#include <induction/callback.h>

#include "case.h"

int form_case_edit (i_resource *self, i_form_reqdata *reqdata)
{
  l_case *cas;
  i_callback *cb;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF) 
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_case_edit failed to create form"); return -1; }

  /* Check for case */
  if (reqdata->form_passdata)
  {
    /* Case specified, edit existing */
    cb = l_case_sql_list (self, reqdata->form_passdata, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, l_case_form_edit_casecb, reqdata);
    if (!cb)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to query SQL database"); return 1; }
  }
  else
  {
    /* No case specified, add new */
    cas = NULL;
    
    /* Frame start */
    i_form_frame_start (reqdata->form_out, "case_edit", "Open New Case");

    /* Fields */
    i_form_entry_add (reqdata->form_out, "hline", "Headline", NULL);
    i_form_entry_add (reqdata->form_out, "requester", "Requester", NULL);
    i_form_textarea_add (reqdata->form_out, "logentry", "Log Entry", NULL);
    
    /* End Frame */
    i_form_frame_end (reqdata->form_out, "case_edit");

    return 1; /* Form is ready */
  }

  return 0;   /* Not ready */
}

int l_case_form_edit_casecb (i_resource *self, i_list *list, void *passdata)
{
  char *str;
  l_case *cas;
  i_user *user;
  i_form_item *fitem;
  i_list *userlist;
  i_form_reqdata *reqdata = passdata;

  /* Check for not found */
  if (!list || list->size < 1)
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "Specified case not found");
    i_form_deliver (self, reqdata);
    return -1; 
  }

  /* 
   * Case fields 
   */
  
  i_list_move_head (list);
  cas = i_list_restore (list);
  
  asprintf (&str, "%li", cas->id);
  i_form_hidden_add (reqdata->form_out, "id", str);
  free (str);

  i_form_entry_add (reqdata->form_out, "hline", "Headline", cas->hline_str);
  i_form_entry_add (reqdata->form_out, "requester", "Requester", cas->requester_str);

  fitem = i_form_dropdown_create ("owner", "Owner");
  i_form_dropdown_add_option (fitem, "admin", "Administrator", 0);
  userlist = i_userdb_get_all (self);
  for (i_list_move_head(userlist); (user=i_list_restore(userlist))!=NULL; i_list_move_next(userlist))
  {
    i_form_dropdown_add_option (fitem, user->auth->username, user->fullname, 0);
  }
  i_form_dropdown_set_selected (fitem, cas->owner_str);
  i_form_add_item (reqdata->form_out, fitem);

  i_form_deliver (self, reqdata);

  return -1;    /* Dont keep the case list */
}

int form_case_edit_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  i_callback *cb;
  i_form_item_option *id_opt;
  i_form_item_option *hline_opt;
  i_form_item_option *owner_opt;
  i_form_item_option *requester_opt;
  i_form_item_option *logentry_opt;

  /* Auth checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }
  
  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_case_add_submit unable to create form"); return -1; }

  /* ID */
  id_opt = i_form_get_value_for_item (reqdata->form_in, "id");
  
  /* Other fields */
  hline_opt = i_form_get_value_for_item (reqdata->form_in, "hline");
  owner_opt = i_form_get_value_for_item (reqdata->form_in, "owner");
  requester_opt = i_form_get_value_for_item (reqdata->form_in, "requester");
  logentry_opt = i_form_get_value_for_item (reqdata->form_in, "logentry");

  /* Check for specified case */
  if (id_opt)
  {
    /* Case specified, edit existing case */
    cb = l_case_sql_list (self, (char *) id_opt->data, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, l_case_form_edit_submit_casecb, reqdata);
    if (!cb)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to query SQL database"); return 1; }
  }
  else
  {
    /* No case specifie, open new case */
    long caseid;
    l_case *cas;
    char *logentry_str = NULL;
    
    /* Create case */
    cas = l_case_create ();

    /* Fields */
    if (hline_opt) cas->hline_str = strdup ((char *) hline_opt->data);
    if (requester_opt) cas->requester_str = strdup ((char *) requester_opt->data);
    if (logentry_opt) logentry_str = (char *) logentry_opt->data;
    cas->owner_str = strdup (reqdata->auth->username);
    cas->state = CASE_STATE_OPEN;
    gettimeofday (&cas->start, NULL);

    /* Insert Case */
    num = l_case_sql_insert (self, cas);
    caseid = cas->id;
    l_case_free (cas);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add case to SQL database"); return 1; }

    /* Log entry */
    if (logentry_str)
    {
      l_case_logentry *log;
      log = l_case_logentry_create ();
      gettimeofday (&log->tstamp, NULL);
      log->author_str = strdup (reqdata->auth->username);
      log->entry_str = strdup (logentry_str);

      /* Insert log entry */
      num = l_case_logentry_sql_insert (self, caseid, log);
      l_case_logentry_free (log);
      if (num != 0)
      { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to insert initial log comment"); return 1; }
    }

    /* Call case view form */
    asprintf ((char **) &reqdata->form_passdata, "%li", caseid);
    return form_case_view (self, reqdata);
  }

  return 0;   /* Not finished, waiting for case */
}

int l_case_form_edit_submit_casecb (i_resource *self, i_list *list, void *passdata)
{
  int num;
  l_case *cas;
  i_form_reqdata *reqdata = passdata;
  i_form_item_option *hline_opt;
  i_form_item_option *owner_opt;
  i_form_item_option *requester_opt;

  /* Check for not found */
  if (!list || list->size < 1)
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "Specified case not found");
    i_form_deliver (self, reqdata);
    return -1;
  }

  /* Get case */
  i_list_move_head (list);
  cas = i_list_restore (list);

  /* Fields */
  hline_opt = i_form_get_value_for_item (reqdata->form_in, "hline");
  if (hline_opt && hline_opt->data)
  {
    if (cas->hline_str) { free (cas->hline_str); cas->hline_str = NULL; }
    cas->hline_str = strdup ((char *) hline_opt->data);
  }
  owner_opt = i_form_get_value_for_item (reqdata->form_in, "owner");
  if (owner_opt && owner_opt->data)
  {
    if (cas->owner_str) { free (cas->owner_str); cas->owner_str = NULL; }
    cas->owner_str = strdup ((char *) owner_opt->data);
  }
  requester_opt = i_form_get_value_for_item (reqdata->form_in, "requester");
  if (requester_opt && requester_opt->data)
  {
    if (cas->requester_str) { free (cas->requester_str); cas->requester_str = NULL; }
    cas->requester_str = strdup ((char *) requester_opt->data);
  }

  /* Update */
  num = l_case_sql_update (self, cas);
  if (num != 0)
  { 
    i_form_string_add (reqdata->form_out, "error", "Error", "Failed to update SQL database"); 
    i_form_deliver (self, reqdata);
    return -1;
  }

  /* Call case view form */
  asprintf ((char **) &reqdata->form_passdata, "%li", cas->id);
  num = form_case_view (self, reqdata);
  if (num == 1) i_form_deliver (self, reqdata);

  return -1;  /* Dont keep the case list */
}
