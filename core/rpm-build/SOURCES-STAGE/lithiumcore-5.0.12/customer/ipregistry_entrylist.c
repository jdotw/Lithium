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
#include <induction/ipregistry.h>

#include "ipregistry.h"

int l_ipregistry_entrylist_table (i_resource *self, i_list *list, char *name, char *title, i_form_reqdata *reqdata)
{
  int row;
  char *labels[5] = { "Address", "Mask", "Network", "Site", "Device" };
  i_form_item *item;
  i_ipregistry_entry *entry;
  
  if (!self || !list || !name || !reqdata) return -1;

  /* Add the list table */

  item = i_form_table_create (reqdata->form_out, name, title, 5);
  if (!item) { i_printf (1, "l_ipregistry_entrylist_table failed to create form_table"); return -1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  i_form_table_add_row (item, labels);

  for (i_list_move_head(list); (entry=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    char *devaddr_str;
    struct in_addr network_addr;

    labels[0] = entry->ipent->desc_str;
    if (entry->mask_str)
    {
      labels[1] = entry->mask_str;
      network_addr.s_addr = entry->maskaddr.s_addr & entry->ipaddr.s_addr;
      labels[2] = inet_ntoa (network_addr);
    }
    else
    {
      labels[1] = NULL;
      labels[2] = NULL;
    }
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

    devaddr_str = i_resource_address_struct_to_string (entry->devaddr);
    i_form_table_add_link (item, 4, row, NULL, NULL, "ipregistry_devicemain", 0, devaddr_str, strlen(devaddr_str)+1); 
    free (devaddr_str);
  }

  /* Finished */

  return 0;
}

