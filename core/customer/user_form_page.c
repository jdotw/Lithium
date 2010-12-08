#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/notification.h>

int form_user_page (i_resource *self, i_form_reqdata *reqdata)
{
  if (!self || !reqdata) return -1;
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN)
  { return i_form_deliver_denied (self, reqdata); }
  
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out) { i_printf (1, "form_user_page unable to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "userpage", "Page User");

  i_form_entry_add (reqdata->form_out, "message_text", "Message (160 Characters Max.)", NULL);

  i_form_frame_end (reqdata->form_out, "userpage");
  
  return -1;
}

int form_user_page_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  i_form_item_option *opt;
  i_notification *note;

  if (!self || !reqdata) return -1;
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN)
  { return i_form_deliver_denied (self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_user_page_submit failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "userpage", "Paging User");

  opt = i_form_get_value_for_item (reqdata->form_in, "message_text");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Message Text not found in form"); return 1; }

  note = i_notification_create ((char *)opt->data, "", RES_ADDR(self));
  if (!note)
  { i_form_string_add (reqdata->form_out, "error", "Notification", "Failed to create notification struct"); return 1; }

  num = i_notification_send (self, note);
  i_notification_free (note);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Notification", "Failed to send notification"); return 1; }

  i_form_string_add (reqdata->form_out, "message", "Success", "Notification successfully sent");

  i_form_frame_end (reqdata->form_out, "userpage");

  return 1;
}
