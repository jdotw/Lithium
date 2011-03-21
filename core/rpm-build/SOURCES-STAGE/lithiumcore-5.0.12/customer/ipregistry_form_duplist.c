#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/ip.h>
#include <induction/message.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/socket.h>
#include <induction/msgproc.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/ipregistry.h>

#include "ipregistry.h"

int form_ipregistry_duplist (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  i_list *list;
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_dupregistry_iplist failed to create form"); return -1; }

  i_form_frame_start (reqdata->form_out, "duplist", "Duplicate IP Address List");

  l_ipregistry_form_links (self, reqdata);
  i_form_spacer_add (reqdata->form_out);
  
  /* Get duplicate list */
  list = l_ipregistry_get_dup (self);

  /* Add the list table */
  if (list && list->size > 0)
  {
    num = l_ipregistry_entrylist_table (self, list, "dupiplist", "Duplicate IP Address List", reqdata);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add IP Address List table"); }
  }
  else
  { i_form_string_add (reqdata->form_out, "msg", "No Duplicates", "No duplicate IP addresses present in IP registry"); }

  /* Finished */

  i_list_free (list);

  i_form_frame_end (reqdata->form_out, "duplist");

  return 0;
}
