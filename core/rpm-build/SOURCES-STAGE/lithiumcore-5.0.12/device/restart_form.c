#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/module.h>
#include <induction/auth.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/construct.h>
#include <induction/list.h>

#include "restart.h"

int form_restart (i_resource *self, i_form_reqdata *reqdata)
{
  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF) 
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_restart failed to create form"); return -1; }

  /* Frame */
  i_form_frame_start (reqdata->form_out, "restart", "Confirm Device Process Restart");
  i_form_string_add (reqdata->form_out, "note", "Note", "Click below to restart the customer process");
  i_form_string_add (reqdata->form_out, "note", "Note", "This will also cause all device processes to be restarted");
  i_form_frame_end (reqdata->form_out, "restart");

  return 1;
}

int form_restart_submit (i_resource *self, i_form_reqdata *reqdata)
{
  /* Auth checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_lic_remove_submit unable to create form"); return -1; }
  
  /* Frame */
  i_form_frame_start (reqdata->form_out, "restart", "Restarting Customer");
  i_form_string_add (reqdata->form_out, "note", "Note", "The customer process is being reset");
  i_form_string_add (reqdata->form_out, "note", "Note", "Please wait a few minutes before proceeding");
  i_form_frame_end (reqdata->form_out, "restart");

  /* Install timer to kill process */
  i_timer_add (self, 1, 0, l_restart_timercb, NULL);

  return 1;
} 

