#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/ip.h>
#include <induction/message.h>
#include <induction/entity.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/socket.h>
#include <induction/msgproc.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/ipregistry.h>

#include "ipregistry.h"

int form_ipregistry_ipmain (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  char *frame_title;
  char *ip_str;
  char *str;
  struct in_addr addr;
  i_list *list;
  
  /* Form setup */
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_ipregistry_ipmain failed to create form"); return -1; }

  if (!reqdata->form_passdata || reqdata->form_passdata_size < 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No IP address specified"); return 1; }

  ip_str = (char *) reqdata->form_passdata;
  asprintf (&frame_title, "IP Registry - IP Address %s", ip_str);
  i_form_frame_start (reqdata->form_out, "ipmain", frame_title);
  free (frame_title);

  l_ipregistry_form_links (self, reqdata);
  i_form_spacer_add (reqdata->form_out);

  /* Get list of all matching IP addresses */
  list = l_ipregistry_get_ip_str (self, ip_str);
  if (!list || list->size < 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "IP Address not found in registry"); return 1; }

  /* Stats */

  i_form_string_add (reqdata->form_out, "ip", "IP Address", ip_str);

  num = inet_aton (ip_str, &addr);
  if (num == 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to convert ip_str to addr struct"); return 1; }

  asprintf (&str, "%i", list->size);
  i_form_string_add (reqdata->form_out, "list_size", "Matching Entries", str);
  free (str);

  i_form_spacer_add (reqdata->form_out);

  /* Add the list table */

  num = l_ipregistry_entrylist_table (self, list, "iplist", "IP Address List", reqdata);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add IP address list table"); }
    
  /* Finished */

  i_form_frame_end (reqdata->form_out, "ipmain");

  return 1;
}

