#include <stdio.h>
#include <stdlib.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/user.h>
#include <induction/userdb.h>

int form_user_remove (i_resource *self, i_form_reqdata *reqdata)
{
  char *str;
  i_user *user;

  if (!self || !reqdata) return -1;
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN) 
  { return i_form_deliver_denied (self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_user_remove failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "userremove", "Confirm User Removal");
  i_form_option_add (reqdata->form_out, FORM_OPTION_SUBMIT_LABEL, "Confirm", 8);

  if (!reqdata->form_passdata || reqdata->form_passdata_size < 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Passdata not present"); return 1; }
  
  user = i_userdb_get (self, (char *) reqdata->form_passdata);
  if (!user)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified user not found"); return 1; }

  i_form_hidden_add (reqdata->form_out, "username", user->auth->username);

  asprintf (&str, "Please confirm removal of user '%s' (%s)", user->fullname, user->auth->username);
  i_form_string_add (reqdata->form_out, "msg", "Confirm", str);
  free (str);
  i_user_free (user);

  i_form_set_submit (reqdata->form_out, 1);

  i_form_frame_end (reqdata->form_out, "userremove");

  return 1;
}

int form_user_remove_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  i_form_item_option *opt;

  if (!self || !reqdata) return -1;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN)
  { return i_form_deliver_denied (self, reqdata); }
  
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out) 
  { i_printf (1, "form_user_remove_submit failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "userremove", "User Removal");

  opt = i_form_get_value_for_item (reqdata->form_in, "username");
  if (opt)
  {
    num = i_userdb_del (self, (char *)opt->data);
    if (num == 0)
    { i_form_string_add (reqdata->form_out, "msg", "Success", "User successfully removed"); }
    else
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to remove user"); }
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No username specified"); }

  i_form_frame_end (reqdata->form_out, "userremove");

  return 1;
}
