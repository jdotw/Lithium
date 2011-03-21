#include <stdio.h>
#include <string.h>
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
#include <induction/interface.h>
#include <induction/ipregistry.h>

#include "ipregistry.h"

int form_ipregistry_sitemain (i_resource *self, i_form_reqdata *reqdata)
{
  char *frame_title;
  char *str;
  char *labels[5] = { "Address", "Mask", "Network", "Device", "Interface" };
  i_list *list;
  i_form_item *table;
  i_form_item *fitem;
  i_ipregistry_entry *entry;
  
  /* Form setup */
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_ipregistry_sitemain failed to create form"); return -1; }

  if (!reqdata->form_passdata || reqdata->form_passdata_size < 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No site specified"); return 1; }

  /* Get list of all matching IP addresses */

  list = l_ipregistry_get_site (self, (char *) reqdata->form_passdata);
  if (!list || list->size < 1)
  { 
    i_form_frame_start (reqdata->form_out, "sitemain", "IP Registry Site Information");
    i_form_string_add (reqdata->form_out, "error", "No Registry Entries", "No IP Registry Entries were found for the specified site"); 
    i_form_frame_end (reqdata->form_out, "sitemain");
    return 1; 
  }

  /* Stats */

  i_list_move_head (list);
  entry = i_list_restore (list);
  if (!entry)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to restore first entry in list"); return 1; }

  asprintf (&frame_title, "IP Registry - %s", entry->ipent->site_desc);
  i_form_frame_start (reqdata->form_out, "sitemain", frame_title);
  free (frame_title);

  l_ipregistry_form_links (self, reqdata);
  i_form_spacer_add (reqdata->form_out);

  fitem = i_form_string_add (reqdata->form_out, "site_name", "Site Name", entry->ipent->site_desc);

  asprintf (&str, "%i", list->size);
  i_form_string_add (reqdata->form_out, "list_size", "Registered Addresses", str);
  free (str);

  i_form_spacer_add (reqdata->form_out);

  /* Address table */

  table = i_form_table_create (reqdata->form_out, "address_table", "IP Addresses registered at site", 5);
  if (!table) { i_printf (1, "form_ipregistry_sitemain failed to create address_table"); return -1; }
  i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  i_form_table_add_row (table, labels);

  for (i_list_move_head(list); (entry=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    int row;
    char *device_addr_str;
    struct in_addr network_addr;
    labels[0] = entry->ipent->desc_str;
    labels[1] = entry->mask_str;
    if (entry->mask_str)
    {
      network_addr.s_addr = entry->maskaddr.s_addr & entry->ipaddr.s_addr;
      labels[2] = inet_ntoa (network_addr);
    }
    else { labels[2] = NULL; }
    labels[3] = entry->ipent->dev_desc;
    if (entry->ifent && entry->ifent->desc_str)
    { labels[4] = entry->ifent->desc_str; }
    else if (entry->ifent && entry->ifent->name_str)
    { labels[4] = entry->ifent->name_str; }
    else
    { labels[4] = NULL; }

    row = i_form_table_add_row (table, labels);

    if (entry->ipent->name_str)
    { i_form_table_add_link (table, 0, row, NULL, NULL, "ipregistry_ipmain", 0, entry->ipent->desc_str, strlen(entry->ipent->desc_str)+1); }

    if (labels[2])
    {
      char *pass_str;
      asprintf (&pass_str, "%s:%s", labels[2], entry->mask_str);
      i_form_table_add_link (table, 2, row, NULL, NULL, "ipregistry_networkmain", 0, pass_str, strlen(pass_str)+1);
      free (pass_str);
    }

    device_addr_str = i_resource_address_struct_to_string (entry->devaddr);
    if (device_addr_str)
    { i_form_table_add_link (table, 3, row, NULL, NULL, "ipregistry_devicemain", 0, device_addr_str, strlen(device_addr_str)+1); free (device_addr_str); }
  }
    
  /* Finished */

  i_form_frame_end (reqdata->form_out, "sitemain");

  return 1;
}

