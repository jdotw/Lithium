#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/ip.h>
#include <induction/message.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/socket.h>
#include <induction/msgproc.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/entity.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/ipregistry.h>

#include "ipregistry.h"

int form_ipregistry_networklist (i_resource *self, i_form_reqdata *reqdata)
{
  int row;
  char *labels[3] = { "Network", "Mask", "Usage" };
  i_list *network_list;
  i_list *entry_list;
  i_hashtable *network_table;
  i_form_item *item;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_ipregistry_networklist failed to create form"); return -1; }

  i_form_frame_start (reqdata->form_out, "networklist", "IP Registry Network List");

  l_ipregistry_form_links (self, reqdata);
  i_form_spacer_add (reqdata->form_out);

  network_list = i_list_create ();
  if (!network_list)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create network_list struct"); return 1; }

  network_table = l_ipregistry_table_network ();
  if (!network_table)
  { i_form_string_add (reqdata->form_out, "error", "Error", "IP Registry Network Table not present"); return 1; }

  /* Create a list of the networks (i.e a list of list of entries */
  
  i_hashtable_iterate (self, network_table, l_ipregistry_networklist_iterate, network_list);

  /* Sort the list */

  i_list_sort (network_list, l_ipregistry_networklist_sort_func);

  /* Add the list table */

  item = i_form_table_create (reqdata->form_out, "entry_list", "IP Network List", 3);
  if (!item) { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create list table"); return -1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  i_form_table_add_row (item, labels);

  for (i_list_move_head(network_list); (entry_list=i_list_restore(network_list))!=NULL; i_list_move_next(network_list))
  {
    char *pass_str;
    struct in_addr network_addr;
    i_ipregistry_entry *entry;            

    i_list_move_head (entry_list);
    entry = i_list_restore (entry_list);
    if (!entry)
    { i_printf (1, "form_ipregistry_networklist failed to restore first entry for an entry list"); continue; }

    network_addr.s_addr = entry->maskaddr.s_addr & entry->ipaddr.s_addr;
    labels[0] = inet_ntoa (network_addr);
    labels[1] = entry->mask_str;
    if (ntohl(entry->maskaddr.s_addr) == 0)
    { labels[2] = strdup("N/A"); }
    else
    { asprintf (&labels[2], "%.2f %%", ((float) entry_list->size / ((ntohl(~entry->maskaddr.s_addr))+1)) * 100); }

    row = i_form_table_add_row (item, labels);

    asprintf (&pass_str, "%s:%s", labels[0], entry->mask_str);
    i_form_table_add_link (item, 0, row, NULL, NULL, "ipregistry_networkmain", 0, pass_str, strlen(pass_str)+1);
    free (pass_str);
    
    free (labels[2]);
  }

  /* Finished */

  i_list_free (network_list);

  i_form_frame_end (reqdata->form_out, "networklist");
  
  return 1;
}

/* Network table iteration function */

void l_ipregistry_networklist_iterate (i_resource *self, i_hashtable *ip_table, void *data, void *passdata)
{
  int num;
  i_list *network_list = passdata;

  num = i_list_enqueue (network_list, data);
  if (num != 0)
  { i_printf (1, "l_ipregistry_networklist_iterate failed to enqueue an entry"); }

  return;
}

/* Sorting */

int l_ipregistry_networklist_sort_func (void *curptr, void *nextptr)
{
  i_list *cur_list;
  i_list *next_list;
  i_ipregistry_entry *cur_entry;
  i_ipregistry_entry *next_entry;
  unsigned long int cur_addr_h;
  unsigned long int next_addr_h;

  /* This function sorts out the 'networklist' which is
   * created by iterating the hashtable and enqueuing each
   * list of entries
   */

  cur_list = curptr;
  next_list = nextptr;
  i_list_move_head (cur_list);
  i_list_move_head (next_list);
  cur_entry = i_list_restore (cur_list);
  next_entry = i_list_restore (next_list);

  if (!cur_entry || !next_entry)
  { return 0; }

  cur_addr_h = ntohl (cur_entry->ipaddr.s_addr);
  next_addr_h = ntohl (next_entry->ipaddr.s_addr);

  if (cur_addr_h > next_addr_h) return 1;

  return 0;
}
