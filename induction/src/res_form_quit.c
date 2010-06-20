#include <stdlib.h>

#include "induction.h"
#include "form.h"
#include "timer.h"
#include "auth.h"

int form_resource_quit (i_resource *self, i_form_reqdata *reqdata)
{
  i_timer *timer;
  if (!self || !reqdata) return -1;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_resource_quit failed to create form"); return -1; }
  i_form_set_title (reqdata->form_out, "Restarting resource");

  timer = i_timer_add (self, 2, 0, i_res_form_quit_timer_callback, NULL);
  if (!timer)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to install restart timer"); return 1; }

  i_form_string_add (reqdata->form_out, "msg", "Success", "Resource will restart in 2 seconds");

  return 1;
}

int i_res_form_quit_timer_callback (i_resource *self, i_timer *timer, void *passdata)
{
  exit (0);
  return -1;
}
