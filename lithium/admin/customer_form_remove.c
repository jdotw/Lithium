#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/module.h>
#include <induction/auth.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/construct.h>
#include <induction/list.h>

#include "customer.h"

int form_customer_remove (i_resource *self, i_form_reqdata *reqdata)
{
  i_customer *cust;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN) 
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_customer_remove failed to create form"); return -1; }

  /* Check for customer entity */
  if (reqdata->entaddr)
  {
    /* Customer specified, remove confirm */
    cust = l_customer_get (self, reqdata->entaddr->cust_name);
    if (!cust)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified customer not found"); return 1; }

    /* Frame start */
    i_form_frame_start (reqdata->form_out, "customer_remove", "Confirm Customer Remove");
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No customer specified"); return 1; }

  /* Info */
  i_form_string_add (reqdata->form_out, "name", "Customer Name", cust->name_str);
  i_form_string_add (reqdata->form_out, "desc", "Customer Description", cust->desc_str);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, "customer_remove");

  return 1;
}

int form_customer_remove_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  i_customer *cust;

  /* Auth checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }
  
  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_customer_remove_submit unable to create form"); return -1; }

  /* Check for specified customer */
  if (reqdata->entaddr)
  {
    /* Customer specified, remove customer */
    cust = l_customer_get (self, reqdata->entaddr->cust_name);
    if (!cust)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified customer not found"); return 1; }

    /* Start frame */
    i_form_frame_start (reqdata->form_out, "customer_remove", "Remove Customer");

    /* Info */
    i_form_string_add (reqdata->form_out, "name", "Customer Name", cust->name_str);
    i_form_string_add (reqdata->form_out, "desc", "Customer Description", cust->desc_str);
    i_form_spacer_add (reqdata->form_out);

    /* Call Remove */
    num = l_customer_remove (self, cust);
    if (num == 0)
    { i_form_string_add (reqdata->form_out, "msg", "Success", "Successfully removed customer"); }
    else
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to remove customer"); }

    /* End Frame */
    i_form_frame_end (reqdata->form_out, "customer_remove");
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No customer specified"); }
  
  return 1;
}

