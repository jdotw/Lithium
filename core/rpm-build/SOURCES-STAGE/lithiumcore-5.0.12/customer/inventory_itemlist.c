#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/inventory.h>

#include "inventory.h"

int l_inventory_itemlist_table (i_resource *self, i_list *list, char *name, char *title, i_form_reqdata *reqdata)
{
  int row;
  char *labels[4] = { "Vendor", "Product", "Version", "Version History" };
  i_form_item *item;
  i_inventory_item *invitem;
  
  if (!self || !list || !name || !reqdata) return -1;

  /* Add the list table */

  item = i_form_table_create (reqdata->form_out, name, title, 4);
  if (!item) { i_printf (1, "l_inventory_itemlist_table failed to create form_table"); return -1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  i_form_table_add_row (item, labels);

  for (i_list_move_head(list); (invitem=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    char *pass_str;
    char *dev_addr_str;
    labels[0] = invitem->vendor_str;
    labels[1] = invitem->product_str;
    labels[2] = invitem->version_str;

    row = i_form_table_add_row (item, labels);

    if (invitem->vendor_str)
    { 
      i_form_table_add_link (item, 0, row, RES_ADDR(self), NULL, "inventory_vendormain", 0, invitem->vendor_str, strlen(invitem->vendor_str)+1); 
      if (invitem->product_str)
      {
        asprintf (&pass_str, "%s:%s", invitem->vendor_str, invitem->product_str);
        i_form_table_add_link (item, 0, row, RES_ADDR(self), NULL, "inventory_productmain", 0, pass_str, strlen(pass_str)+1); 
        free (pass_str);
      }
    }

    dev_addr_str = i_resource_address_struct_to_string (invitem->dev->resaddr);
    asprintf (&pass_str, "%s:%p", dev_addr_str, item);
    free (dev_addr_str);
    i_form_table_add_link (item, 3, row, RES_ADDR(self), NULL, "inventory_historymain", 0, pass_str, strlen(pass_str)+1);
    free (pass_str);                    
  }

  /* Finished */

  return 0;
}

