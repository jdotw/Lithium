#include <stdlib.h>

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

int form_customer_list (i_resource *self, i_form_reqdata *reqdata)
{
  char *labels[4];
  i_form_item *item;
  i_customer *cust;
  i_list *customer_list;

  if (!self || !reqdata) return -1;
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }
  
  /* Create / Setup Form */
  
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out) 
  { i_printf (1, "form_customer_list failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "custlist", "Customer List");

  i_form_spacer_add (reqdata->form_out);

  /* Create customer table */

  if (reqdata->auth->level >= AUTH_LEVEL_ADMIN)
  { item = i_form_table_create (reqdata->form_out, "customer_list", NULL, 4); }
  else
  { item = i_form_table_create (reqdata->form_out, "customer_list", NULL, 2); }
  if (!item) 
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create customer table"); return 1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width */

  labels[0] = "Description";
  labels[1] = "Name";
  labels[2] = "Edit";
  labels[3] = "Remove";
  i_form_table_add_row (item, labels);

  /* Populate table */
  
  customer_list = l_customer_list ();
  for (i_list_move_head(customer_list); (cust=i_list_restore(customer_list)) != NULL; i_list_move_next(customer_list))
  {
    int row;

    /* Add row */
    labels[0] = cust->desc_str;
    labels[1] = cust->name_str;
    row = i_form_table_add_row (item, labels);

    /* Add Links */
    i_form_table_add_link (item, 0, row, cust->resaddr, NULL, "main", 0, NULL, 0);
    i_form_table_add_link (item, 1, row, cust->resaddr, NULL, "main", 0, NULL, 0);
    i_form_table_add_link (item, 2, row, RES_ADDR(self), ENT_ADDR(cust), "customer_edit", 0, NULL, 0);
    i_form_table_add_link (item, 3, row, RES_ADDR(self), ENT_ADDR(cust), "customer_remove", 0, NULL, 0);
  }

  if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
  {
    item = i_form_string_add (reqdata->form_out, "add", "Add New Customer", NULL);
    if (!item) 
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add 'Add New Customer' string to form"); return 1; }
    i_form_item_add_link (item, 0, 0, 0, RES_ADDR(self), NULL, "customer_edit", 0, NULL, 0);
  }

  i_form_frame_end (reqdata->form_out, "custlist");

  return 1;
}

int form_customer_list_addr (i_resource *self, i_form_reqdata *reqdata)
{
  char *res_addr_str;
  i_list *customer_list;
  i_customer *cust;

  if (!self || !reqdata) return -1;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_INFSTAT)
  { return i_form_deliver_denied (self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_customer_list_addr failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "custlist", "Customer Resource Address List");

  customer_list = l_customer_list ();
  for (i_list_move_head(customer_list); (cust=i_list_restore(customer_list)) != NULL; i_list_move_next(customer_list))
  {
    if (cust->resaddr)
    {
      res_addr_str = i_resource_address_struct_to_string (cust->resaddr);
      if (!res_addr_str) continue;

      i_form_string_add (reqdata->form_out, cust->resaddr->ident_str, cust->resaddr->ident_str, res_addr_str);
      free (res_addr_str);
    }
  }

  i_form_frame_end (reqdata->form_out, "custlist");

  return 1;
}

