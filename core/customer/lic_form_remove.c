#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/module.h>
#include <induction/auth.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/construct.h>
#include <induction/list.h>

#include "lic.h"

int form_lic_remove (i_resource *self, i_form_reqdata *reqdata)
{
  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN) 
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_device_remove failed to create form"); return -1; }

  /* Check data */
  if (!reqdata->form_passdata)

  /* Check ID */
  if (strcmp("0", (char *)reqdata->form_passdata) == 0)
  {
    /* Can not remove default */
    i_form_string_add (reqdata->form_out, "error" , "Error", "Default license can not be removed"); 
    return 1; 
  }

  /* Frame */
  i_form_frame_start (reqdata->form_out, "lic_remove", "Confirm License Key Remove");
  i_form_string_add (reqdata->form_out, "note", "Note", "Click below to confirm license key removal");
  i_form_string_add (reqdata->form_out, "note", "Note", "License/Entitlement changes may not take effect until the customer process is restarted");
  i_form_frame_end (reqdata->form_out, "lic_remove");

  /* ID */
  i_form_hidden_add (reqdata->form_out, "id", reqdata->form_passdata);

  return 1;
}

int form_lic_remove_submit (i_resource *self, i_form_reqdata *reqdata)
{
  i_form_item_option *id_opt;

  /* Auth checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN)
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_lic_remove_submit unable to create form"); return -1; }
  
  /* Fields */
  id_opt = i_form_get_value_for_item (reqdata->form_in, "id");  
  if (!id_opt || !id_opt->data) 
  { i_form_string_add (reqdata->form_out, "error", "Error", "Key ID not found"); return 1; }
  
  /* Add license key */
  l_lic_key_remove (self, atol((char *)id_opt->data));
  
  /* Return license main */
  return form_lic_main (self, reqdata);
}


