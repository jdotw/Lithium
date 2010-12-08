#include <stdlib.h>
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

int form_dhcp_subnet_remove (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  l_dhcp_subnet *subnet = NULL;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied(self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_dhcp_subnet_remove failed to create form"); return -1; }

  /* Check for specified subnet */
  if (reqdata->form_passdata && reqdata->form_passdata_size > 0)
  {
    char *id_str;
    id_str = (char *) reqdata->form_passdata;
    subnet = l_dhcp_subnet_get (self, atol(id_str));
    if (!subnet)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified subnet not found"); return 1; }
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No subnet specified"); return 1; }

  /* Start frame */
  i_form_frame_start (reqdata->form_out, "subnet_remove", "Remove DHCP Subnet"); 

  /* Remove subnet */
  num = l_dhcp_subnet_remove (self, subnet);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to remove subnet"); }
  else
  { i_form_string_add (reqdata->form_out, "msg", "Success", "DHCP subnet successfully removed"); }

  /* End frame */
  i_form_frame_end (reqdata->form_out, "subnet_remove");
  
  return 1;
}

