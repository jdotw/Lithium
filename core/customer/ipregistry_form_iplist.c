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

int form_ipregistry_iplist (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  i_list *list;
  i_hashtable *ip_table;
  
  if (!self || !reqdata) return -1;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_ipregistry_iplist failed to create form"); return -1; }

  i_form_frame_start (reqdata->form_out, "iplist", "IP Address List");

  l_ipregistry_form_links (self, reqdata);
  i_form_spacer_add (reqdata->form_out);

  list = i_list_create ();
  if (!list)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create list struct"); return 1; }

  ip_table = l_ipregistry_table_ip ();
  if (!ip_table)
  { i_form_string_add (reqdata->form_out, "error", "Error", "IP Registry IP Table not present"); i_list_free (list); return 1; }

  /* Create a list of the IP addresses */
  
  i_hashtable_iterate (self, ip_table, l_ipregistry_iplist_iterate, list);

  /* Sort the list */

  i_list_sort (list, l_ipregistry_listsort_ip_func);

  /* Add the list table */

  num = l_ipregistry_entrylist_table (self, list, "iplist", "IP Address List", reqdata);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add IP Address List table"); }
    
  /* Finished */

  i_list_free (list);

  i_form_frame_end (reqdata->form_out, "iplist");

  return 1;
}

void l_ipregistry_iplist_iterate (i_resource *self, i_hashtable *ip_table, void *data, void *passdata)
{
  int num;
  i_list *ip_list = passdata;
  i_list *entry_list = data;
  i_ipregistry_entry *entry;

  if (!ip_list || !entry_list) return;

  for (i_list_move_head(entry_list); (entry=i_list_restore(entry_list))!=NULL; i_list_move_next(entry_list))
  {
    /* Loop through each entry and enqueue it into ip_list */
    num = i_list_enqueue (ip_list, entry);
    if (num != 0)
    { i_printf (1, "l_ipregistry_iplist_iterate failed to enqueue entry for %s/%s", entry->ipent->name_str, entry->mask_str); }
  }

  return;
}
