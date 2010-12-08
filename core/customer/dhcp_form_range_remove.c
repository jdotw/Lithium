#include <stdlib.h>
#include <string.h>
#include <math.h>

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

int form_dhcp_range_remove (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  l_dhcp_subnet *subnet;
  l_dhcp_range *range = NULL;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied(self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_dhcp_range_remove failed to create form"); return -1; }

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
    }
    else
    { i_form_string_add (reqdata->form_out, "error", "Error", "No range specified"); return 1; }
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No subnet/range specified"); return 1; }

  /* Start frame */
  i_form_frame_start (reqdata->form_out, "range_remove", "Remove DHCP range"); 

  /* Remove range */
  num = l_dhcp_range_remove (self, range);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to remove DHCP range"); }
  else
  { i_form_string_add (reqdata->form_out, "msg", "Success", "DHCP range successfully removed"); }

  /* End frame */
  i_form_frame_end (reqdata->form_out, "range_remove");
  
  return 1;
}

