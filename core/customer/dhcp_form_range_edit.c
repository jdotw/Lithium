#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/site.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>

#include "dhcp.h"

/* 
 * ISC DHCP Management Sub-System 
 */

int form_dhcp_range_edit (i_resource *self, i_form_reqdata *reqdata)
{
  char *str;
  l_dhcp_subnet *subnet;
  l_dhcp_range *range = NULL;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied(self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_dhcp_range_edit failed to create form"); return -1; }

  /* Check for specified range */
  if (reqdata->form_passdata && reqdata->form_passdata_size > 0)
  {
    char *subnet_str;
    char *range_str;
    
    subnet_str = (char *) reqdata->form_passdata;
    range_str = strchr (subnet_str, ':');
    if (range_str) 
    { *range_str = '\0'; range_str++; }

    subnet = l_dhcp_subnet_get (self, atol (subnet_str));
    if (!subnet)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified subnet not found"); return 1; }

    if (range_str)
    {
      range = l_dhcp_range_get (self, subnet, atol(range_str));
      if (!range)
      { i_form_string_add (reqdata->form_out, "error", "Error", "Specified range not found"); return 1; }
    
      i_form_frame_start (reqdata->form_out, "range_edit", "Edit DHCP Range");
    }
    else
    { i_form_frame_start (reqdata->form_out, "range_edit", "Add DHCP Range"); }
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No subnet/range specified"); return 1; }

  /* Add Subnet ID */
  asprintf (&str, "%li", subnet->id);
  i_form_hidden_add (reqdata->form_out, "subnet_id", str);
  free (str);
  
  /* Add ID (if present) */
  if (range)
  { 
    asprintf (&str, "%li", range->id); 
    i_form_hidden_add (reqdata->form_out, "range_id", str); 
    free (str); 
  }

  /* Subnet Info */
  i_form_string_add (reqdata->form_out, "subnet", "Subnet", subnet->network_str);
  i_form_string_add (reqdata->form_out, "mask", "Mask", subnet->mask_str); 

  /* Start Address */
  if (range) i_form_entry_add (reqdata->form_out, "start", "Start Address", range->start_str);
  else i_form_entry_add (reqdata->form_out, "start", "Start Address", NULL); 

  /* End Address */
  if (range) i_form_entry_add (reqdata->form_out, "end", "End Address", range->end_str);
  else i_form_entry_add (reqdata->form_out, "end", "End Address", NULL); 

  /* End frame */
  i_form_frame_end (reqdata->form_out, "range_edit");
  
  return 1;
}

int form_dhcp_range_edit_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  l_dhcp_subnet *subnet;
  l_dhcp_range *range = NULL;
  i_form_item_option *subnet_id_opt;
  i_form_item_option *range_id_opt;
  i_form_item_option *start_opt;
  i_form_item_option *end_opt;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied(self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_dhcp_range_edit_submit failed to create form"); return -1; }

  /* Find Subnet */
  subnet_id_opt = i_form_get_value_for_item (reqdata->form_in, "subnet_id");
  if (subnet_id_opt)
  { 
    subnet = l_dhcp_subnet_get (self, atol((char *) subnet_id_opt->data)); 
    if (!subnet)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified subnet not found"); return 1; }
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No subnet specified"); return 1; }

  /* Retrieve Values */
  start_opt = i_form_get_value_for_item (reqdata->form_in, "start");
  if (!start_opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Start address not found in form"); return 1; }
  end_opt = i_form_get_value_for_item (reqdata->form_in, "end");
  if (!end_opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "End address not found in form"); return 1; }

  /* Find range */
  range_id_opt = i_form_get_value_for_item (reqdata->form_in, "range_id");
  if (range_id_opt)
  { range = l_dhcp_range_get (self, subnet, atol((char *) range_id_opt->data)); }

  /* Edit/Add Handling */
  if (range)
  {
    /* Existing Range Edit */
    i_form_frame_start (reqdata->form_out, "range_edit", "Edit DHCP Range");

    /* Start */
    if (range->start_str) { free (range->start_str); range->start_str = NULL; }
    range->start_str = strdup ((char *)start_opt->data);
    
    /* End */
    if (range->end_str) { free (range->end_str); range->end_str = NULL; }
    range->end_str = strdup ((char *)end_opt->data);
    
    /* Update range */
    num = l_dhcp_range_update (self, range);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to update DHCP range"); }
    else
    { i_form_string_add (reqdata->form_out, "msg", "Success", "DHCP range successfully updated"); }
  }
  else
  {
    
    /* Add New Range */
    i_form_frame_start (reqdata->form_out, "range_edit", "Add DHCP Range");

    /* Add range */
    range = l_dhcp_range_add (self, subnet, (char *) start_opt->data, (char *) end_opt->data);
    if (!range)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add DHCP range"); }
    else
    { i_form_string_add (reqdata->form_out, "msg", "Success", "DHCP range successfully added"); }
  }

  /* End Frame */
  i_form_frame_end (reqdata->form_out, "range_edit");

  return 1;
}

