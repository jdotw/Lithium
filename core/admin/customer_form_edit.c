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

int form_customer_edit (i_resource *self, i_form_reqdata *reqdata)
{
  i_customer *cust;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN) 
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_customer_edit failed to create form"); return -1; }

  /* Check for customer entity */
  if (reqdata->entaddr)
  {
    /* Customer specified, edit existing */
    cust = l_customer_get (self, reqdata->entaddr->cust_name);
    if (!cust)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified customer not found"); return 1; }

    /* Frame start */
    i_form_frame_start (reqdata->form_out, "customer_edit", "Edit Existing Customer");
  }
  else
  {
    /* No customer specified, add new */
    cust = NULL;
    
    /* Frame start */
    i_form_frame_start (reqdata->form_out, "customer_edit", "Add New Customer");
  }

  /* Name */
  if (cust) i_form_string_add (reqdata->form_out, "name", "Customer Name", cust->name_str);
  else i_form_entry_add (reqdata->form_out, "name", "Customer Name", NULL);

  /* Desc */
  if (cust) i_form_entry_add (reqdata->form_out, "desc", "Customer Description", cust->desc_str);
  else i_form_entry_add (reqdata->form_out, "desc", "Customer Description", NULL);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, "customer_edit");

  return 1;
}

int form_customer_edit_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  i_customer *cust;
  i_form_item_option *name_opt;
  i_form_item_option *desc_opt;

  /* Auth checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }
  
  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_customer_edit_submit unable to create form"); return -1; }

  /* Name */
  name_opt = i_form_get_value_for_item (reqdata->form_in, "name");
  if (!name_opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Name not found in form"); return 1; }

  /* Description */
  desc_opt = i_form_get_value_for_item (reqdata->form_in, "desc");
  if (!desc_opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Description not found in form"); return 1; }

  /* Check for specified customer */
  if (reqdata->entaddr)
  {
    /* Customer specified, edit existing customer */
    cust = l_customer_get (self, reqdata->entaddr->cust_name);
    if (!cust)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified customer not found"); return 1; }

    /* Start frame */
    i_form_frame_start (reqdata->form_out, "customer_edit", "Edit Existing Customer");

    /* Set description */
    if (cust->desc_str) free (cust->desc_str);
    if (desc_opt->data) cust->desc_str = strdup ((char *)desc_opt->data);
    else cust->desc_str = NULL;

    /* Call update */
    num = l_customer_update (self, cust);
    if (num == 0)
    { i_form_string_add (reqdata->form_out, "msg", "Success", "Successfully updated customer"); }
    else
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to update customer"); }
  }
  else
  {
    /* Start frame */
    i_form_frame_start (reqdata->form_out, "customer_edit", "Add New Customer");

    /* Create customer */
    cust = l_customer_add (self, (char *) name_opt->data, (char *) desc_opt->data);
    if (cust)
    { i_form_string_add (reqdata->form_out, "msg", "Success", "Successfully added customer"); }
    else
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add customer"); }
  }

  i_form_frame_end (reqdata->form_out, "customer_edit");
  
  return 1;
}

