#include <stdlib.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/inventory.h>

#include "inventory.h"

int l_inventory_form_links (i_resource *self, i_form_reqdata *reqdata)
{
  int row;
  char *labels[3] = { "Inventory Vendor List", "Inventory Device List", "Inventory Site List" };
  i_form_item *item;
  
  item = i_form_table_create (reqdata->form_out, "inventory_links", NULL, 3);
  if (!item) { i_printf (1, "l_inventory_form_links failed to create table"); return -1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  row = i_form_table_add_row (item, labels);  

  i_form_table_add_link (item, 0, row, RES_ADDR(self), NULL, "inventory_vendorlist", 0, NULL, 0);
  i_form_table_add_link (item, 1, row, RES_ADDR(self), NULL, "inventory_devicelist", 0, NULL, 0);
  i_form_table_add_link (item, 2, row, RES_ADDR(self), NULL, "inventory_sitelist", 0, NULL, 0);

  return 0;
}
