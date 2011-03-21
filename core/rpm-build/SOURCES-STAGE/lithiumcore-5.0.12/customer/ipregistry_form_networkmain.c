#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
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

int form_ipregistry_networkmain (i_resource *self, i_form_reqdata *reqdata)
{
  unsigned long int i;
  int row;
  int num;
  int exp_flag = 0;
  char *frame_title;
  char *network_str;
  char *mask_str;
  char *str;
  i_form_item *item;
  i_list *list;
  i_ipregistry_entry *entry;
  struct in_addr mask_addr;
  struct in_addr network_addr;
  char *labels[5] = { "Address", "Mask", "Network", "Site", "Device" };
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_ipregistry_networkmain failed to create form"); return -1; }

  if (!reqdata->form_passdata || reqdata->form_passdata_size < 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No network specified"); return 1; }

  network_str = strdup (reqdata->form_passdata);
  mask_str = strchr (network_str, ':');
  if (!mask_str)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Network string missing ':' seperator"); free (network_str); return 1; }
  *mask_str = '\0';
  mask_str++;

  asprintf (&frame_title, "IP Registry - Network %s/%s", network_str, mask_str);
  i_form_frame_start (reqdata->form_out, "networkmain", frame_title);
  free (frame_title);

  l_ipregistry_form_links (self, reqdata);
  i_form_spacer_add (reqdata->form_out);

  list = l_ipregistry_get_network_str (self, network_str, mask_str);
  if (!list || list->size < 1)
  { i_form_string_add (reqdata->form_out, "error", "Network not present", "No IP registry entries found"); free (network_str); return 1; }

  /* Stats */

  i_form_string_add (reqdata->form_out, "network", "Network", network_str);
  i_form_string_add (reqdata->form_out, "network", "Mask", mask_str);
  
  num = inet_aton (mask_str, &mask_addr);
  if (num == 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to convert mask_str to mask_addr struct"); free (network_str); return 1; }

  num = inet_aton (network_str, &network_addr);
  if (num == 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to convert network_str to network_addr struct"); free (network_str); return 1; }

  if (ntohl(~mask_addr.s_addr) == 0)
  { i_form_string_add (reqdata->form_out, "prefix_length", "Prefix Length", "/32"); }
  else if (ntohl(mask_addr.s_addr) == 0)
  { i_form_string_add (reqdata->form_out, "prefix_length", "Prefix Length", "/0"); }
  else
  {
    asprintf (&str, "/%.0f", 32 - (log(ntohl(~mask_addr.s_addr)) / log(2)));
    i_form_string_add (reqdata->form_out, "prefix_length", "Prefix Length", str);
    free (str);
  }

  asprintf (&str, "%i", list->size);
  i_form_string_add (reqdata->form_out, "list_size", "Registered Addresses", str);
  free (str);

  asprintf (&str, "%i", (ntohl(~mask_addr.s_addr)+1));
  i_form_string_add (reqdata->form_out, "network_size", "Addresses In Network", str);
  free (str);

  asprintf (&str, "%.2f %%", ((float) list->size / ((ntohl(~mask_addr.s_addr))+1)) * 100);
  i_form_string_add (reqdata->form_out, "network_util", "Address Utilisation", str);
  free (str);
  
  i_form_spacer_add (reqdata->form_out);

  /* Expanded link */

  item = i_form_string_add (reqdata->form_out, "exp_link", "Link", "Expanded View");
  i_form_item_add_link (item, 0, 0, 0, NULL, NULL, "ipregistry_networkmain_expanded", 0, reqdata->form_passdata, reqdata->form_passdata_size);
  
  i_form_spacer_add (reqdata->form_out);

  /* Add the list table */

  item = i_form_table_create (reqdata->form_out, "ip_list", "IP Address List", 5);
  if (!item) 
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create IP address form table"); free (network_str); return 1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  i_form_table_add_row (item, labels);

  if (strstr(reqdata->form_name, "expanded"))
  { exp_flag = 1; }

  i_list_move_head (list);
  entry = i_list_restore (list);
  
  for (i = ntohl (network_addr.s_addr); i < (ntohl (network_addr.s_addr) + (ntohl(~mask_addr.s_addr)+1)); i++)
  {
    /* Loop through each unique address in network */

    if (entry && i == ntohl(entry->ipaddr.s_addr))
    {
      /* This address has an entry */
      char *device_addr_str;
      
      labels[0] = entry->ipent->desc_str;
      labels[1] = entry->mask_str;
      labels[2] = network_str;
      labels[3] = entry->ipent->site_desc;
      labels[4] = entry->ipent->dev_desc;
  
      row = i_form_table_add_row (item, labels);
   
      if (entry->ipent->desc_str)
      { i_form_table_add_link (item, 0, row, NULL, NULL, "ipregistry_ipmain", 0, entry->ipent->desc_str, strlen(entry->ipent->desc_str)+1); }
  
      if (labels[2])
      {
        char *pass_str;

        asprintf (&pass_str, "%s:%s", labels[2], entry->mask_str);
        i_form_table_add_link (item, 2, row, NULL, NULL, "ipregistry_networkmain", 0, pass_str, strlen(pass_str)+1);
        free (pass_str);
      }
  
      device_addr_str = i_resource_address_struct_to_string (entry->devaddr);
      if (device_addr_str)
      { i_form_table_add_link (item, 4, row, NULL, NULL, "ipregistry_devicemain", 0, device_addr_str, strlen(device_addr_str)+1); free (device_addr_str); }
  
      i_list_move_next (list);
      entry = i_list_restore (list);
    }
    else if (exp_flag == 1)
    {
      /* This address doesnt have an entry, 
       * but because we are in expanded mode, we add
       * a row to the table for it 
       */

      char *pass_str;
      struct in_addr ip_addr;

      ip_addr.s_addr = htonl (i);
      asprintf (&labels[0], "%s", inet_ntoa (ip_addr));
      labels[1] = mask_str;
      labels[2] = network_str;
      labels[3] = "";
      labels[4] = "";
  
      row = i_form_table_add_row (item, labels);
  
      i_form_table_add_link (item, 0, row, NULL, NULL, "ipregistry_address_main", 0, labels[0], strlen(labels[0])+1);

      asprintf (&pass_str, "%s:%s", network_str, mask_str);
      i_form_table_add_link (item, 2, row, NULL, NULL, "ipregistry_networkmain", 0, pass_str, strlen(pass_str)+1);
      free (pass_str);  

      free (labels[0]);
    }
  }

  /* Finished */

  free (network_str);

  i_form_frame_end (reqdata->form_out, "networkmain");

  return 1;
}

/* Form Wrappers */

int form_ipregistry_networkmain_expanded (i_resource *self, i_form_reqdata *reqdata)
{ return form_ipregistry_networkmain (self, reqdata); }

