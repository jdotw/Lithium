#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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

static char *static_vendor_str = NULL;   /* Vendor string for current calling of this form */

int form_inventory_vendormain (i_resource *self, i_form_reqdata *reqdata)
{
  int row;
  char *prod_labels[1] = { "Product" };
  char *item_labels[6];
  char *frame_title;
  char *str;
  i_list *prod_list;
  i_list *item_list;
  i_form_item *prod_ftable;
  i_form_item *hw_ftable = NULL;
  i_form_item *sw_ftable = NULL;
  i_form_item *other_ftable = NULL;
  i_hashtable *product_table;
  
  if (!self || !reqdata) return -1;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_inventory_vendorlist failed to create form"); return -1; }

  /* Find vendor */
  product_table = l_inventory_get_vendor (self, (char *) reqdata->form_passdata);
  if (!product_table)
  { 
    i_form_frame_start (reqdata->form_out, "vendormain", "Inventory - Vendor Information");
    i_form_string_add (reqdata->form_out, "error", "Error", "Specified vendor not found"); 
    i_form_frame_end (reqdata->form_out, "vendormain");
    return 1;
  }

  /* Create a list of the products */
  prod_list = i_list_create ();
  if (!prod_list)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create prod_list struct"); return 1; }

  /* Iterate hashtable containing all 
   * products for this vendor
   */
  static_vendor_str = NULL;
  i_hashtable_iterate (self, product_table, l_inventory_vendormain_iterate, prod_list);
  if (prod_list->size < 1)
  {
    i_form_frame_start (reqdata->form_out, "vendormain", "Inventory - Vendor Information");
    i_form_string_add (reqdata->form_out, "error", "Product List", "No products are registered for this vendor"); 
    i_form_frame_end (reqdata->form_out, "vendormain");
    return 1;
  }
  /* prod_list is now a list of lists. Each entry in prod_list
   * is infact a list of entries for each product registered
   * under this vendor
   */

  /* Sort the prod_list by product desc */
  i_list_sort (prod_list, l_inventory_vendormain_prodlist_sortfunc);

  /* Check/Set Title */
  if (static_vendor_str)
  {
    /* Set title */
    asprintf (&frame_title, "Inventory - Vendor '%s' Information", static_vendor_str);
    i_form_frame_start (reqdata->form_out, "vendormain", frame_title);
    free (frame_title);
  }
  else
  { i_form_frame_start (reqdata->form_out, "vendormain", "Inventory - Vendor Information"); }

  l_inventory_form_links (self, reqdata);
  i_form_spacer_add (reqdata->form_out);

  /* Add the product list table */
  asprintf (&str, "%s Product List", static_vendor_str);
  prod_ftable = i_form_table_create (reqdata->form_out, "product_list", str, 1);
  free (str);
  if (!prod_ftable) { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create list table"); return 1; }
  i_form_item_add_option (prod_ftable, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (prod_ftable, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  i_form_table_add_row (prod_ftable, prod_labels);

  /* Loop through the prod_list */
  for (i_list_move_head(prod_list); (item_list=i_list_restore(prod_list))!=NULL; i_list_move_next(prod_list))
  {
    i_form_item *cur_ftable;
    i_inventory_item *invitem;

    /* Loop through items in the item_list for this product */
    for (i_list_move_head(item_list); (invitem=i_list_restore(item_list))!=NULL; i_list_move_next(item_list))
    {
      /* Check if item is the first in the list */
      if (item_list->p->prev == item_list->start)
      {
        char *pass_str;
        
        /* Add product entry to prod_ftable */
        prod_labels[0] = invitem->product_str;
        prod_labels[1] = invitem->vendor_str;

        row = i_form_table_add_row (prod_ftable, prod_labels);

        asprintf (&pass_str, "%s:%s", invitem->vendor_str, invitem->product_str);
        i_form_table_add_link (prod_ftable, 0, row, RES_ADDR(self), NULL, "inventory_productmain", 0, pass_str, strlen(pass_str)+1);
        free (pass_str);
      }

      /* Add item to hw/sw/other_ftable */
      if (invitem->type & INV_HARDWARE)
      {
        /* Hardware item */
        if (!hw_ftable)
        {
          i_form_string_add (reqdata->form_out, "space", "", NULL);
          asprintf (&str, "%s Hardware Items", static_vendor_str);
          hw_ftable = i_form_table_create (reqdata->form_out, "hardware_table", str, 6);
          free (str);
          if (!hw_ftable)
          { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add hardware table"); return 1; }
          i_form_item_add_option (hw_ftable, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);   /* Use proportional spacing */
          i_form_item_add_option (hw_ftable, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);        /* 100% Width for main table */
          item_labels[0] = "Product";
          item_labels[1] = "Site";
          item_labels[2] = "Device";
          item_labels[3] = "Serial Number";
          item_labels[4] = "Item Type";
          item_labels[5] = "Item History";
          i_form_table_add_row (hw_ftable, item_labels);
        }

        cur_ftable = hw_ftable;

        item_labels[0] = invitem->product_str;
        item_labels[1] = invitem->dev->site->desc_str;
        item_labels[2] = invitem->dev->desc_str;
        item_labels[3] = invitem->serial_str;
        item_labels[4] = i_inventory_type_str (invitem->type);
        item_labels[5] = "History";
        row = i_form_table_add_row (hw_ftable, item_labels);
      }
      else if (invitem->type & INV_SOFTWARE)
      {
        /* Software item */
        if (!sw_ftable)
        {
          i_form_string_add (reqdata->form_out, "space", "", NULL);
          asprintf (&str, "%s Software Items", static_vendor_str);
          sw_ftable = i_form_table_create (reqdata->form_out, "software_table", str, 6);
          free (str);
          if (!sw_ftable)
          { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add software table"); return 1; }
          i_form_item_add_option (sw_ftable, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);   /* Use proportional spacing */
          i_form_item_add_option (sw_ftable, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);        /* 100% Width for main table */
          item_labels[0] = "Product";
          item_labels[1] = "Site";
          item_labels[2] = "Device";
          item_labels[3] = "Version";
          item_labels[4] = "Item Type";
          item_labels[5] = "Item History";
          i_form_table_add_row (sw_ftable, item_labels);
        }
    
        cur_ftable = sw_ftable;
    
        item_labels[0] = invitem->product_str;
        item_labels[1] = invitem->dev->site->desc_str;
        item_labels[2] = invitem->dev->desc_str;
        item_labels[3] = invitem->version_str;
        item_labels[4] = i_inventory_type_str (invitem->type);
        item_labels[5] = "History";
        row = i_form_table_add_row (sw_ftable, item_labels);
      }
      else
      {
        /* Other item */
        if (!other_ftable)
        {
          i_form_string_add (reqdata->form_out, "space", "", NULL);
          asprintf (&str, "%s Other Items", static_vendor_str);
          other_ftable = i_form_table_create (reqdata->form_out, "other_table", str, 6);
          free (str);
          if (!other_ftable)
          { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add other table"); return 1; }
          i_form_item_add_option (other_ftable, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);   /* Use proportional spacing */
          i_form_item_add_option (other_ftable, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);        /* 100% Width for main table */
          item_labels[0] = "Product";
          item_labels[1] = "Site";
          item_labels[2] = "Device";
          item_labels[3] = "";
          item_labels[4] = "Item Type";
          item_labels[5] = "Item History";
          i_form_table_add_row (other_ftable, item_labels);
        }
    
        cur_ftable = other_ftable;
    
        item_labels[0] = invitem->product_str;
        item_labels[1] = invitem->dev->site->desc_str;
        item_labels[2] = invitem->dev->desc_str;
        item_labels[3] = "";
        item_labels[4] = i_inventory_type_str (invitem->type);
        item_labels[5] = "History";
        row = i_form_table_add_row (other_ftable, item_labels);
      }

      if (cur_ftable)
      {
        /* Generic links */
        char *dev_addr_str;
        char *pass_str;

        asprintf (&pass_str, "%s:%s", invitem->vendor_str, invitem->product_str);
        i_form_table_add_link (cur_ftable, 0, row, NULL, NULL, "inventory_productmain", 0, pass_str, strlen(pass_str)+1);
        free (pass_str);
        
        i_form_table_add_link (cur_ftable, 1, row, NULL, ENT_ADDR(invitem->dev->site), "inventory_sitemain", 0, invitem->dev->site->name_str, strlen(invitem->dev->site->name_str)+1);

        dev_addr_str = i_resource_address_struct_to_string (invitem->dev->resaddr);
        i_form_table_add_link (cur_ftable, 2, row, NULL, ENT_ADDR(invitem->dev), "inventory_devicemain", 0, dev_addr_str, strlen(dev_addr_str)+1);
        asprintf (&pass_str, "%s:%p", dev_addr_str, invitem);
        free (dev_addr_str);
        i_form_table_add_link (cur_ftable, 5, row, NULL, NULL, "inventory_historymain", 0, pass_str, strlen(pass_str)+1);
        free (pass_str);
      }

      /* End of loop per item */
    }
    /* End of loop per product */
  }

  /* Finished */

  i_list_free (prod_list);

  i_form_frame_end (reqdata->form_out, "vendormain");

  return 1;
}

void l_inventory_vendormain_iterate (i_resource *self, i_hashtable *product_table, void *data, void *passdata)
{
  /* Iterate each cell in the product table */

  int num;
  i_list *item_list = data;       /* The list of items for this product from the hashtable */
  i_list *prod_list = passdata;   /* A list of lists of items per product created by the form func */

  /* Check/Set static_vendor_str */
  if (!static_vendor_str)
  {
    i_inventory_item *invitem;
    
    /* Restore first item for this product */
    i_list_move_head (item_list);
    invitem = i_list_restore (item_list);
    if (!invitem)
    { i_printf (1, "l_inventory_vendormain_iterate failed to restore first item for a product"); return; }

    /* Set static_vendor_str */
    static_vendor_str = invitem->vendor_str;
  }

  /* Enqueue the item_list to the prod_list */
  if (item_list)
  {
    num = i_list_enqueue (prod_list, item_list);
    if (num != 0)
    { i_printf (1, "l_inventory_vendormain_iterate failed to enqueue item_list into prod_list"); }
  }
}

int l_inventory_vendormain_prodlist_sortfunc (void *curptr, void *nextptr)
{
  i_list *cur_list = curptr;
  i_list *next_list = nextptr;
  i_inventory_item *cur_item;
  i_inventory_item *next_item;

  i_list_move_head (cur_list);
  cur_item = i_list_restore (cur_list);
  if (!cur_item)
  { i_printf (1, "l_inventory_vendormain_prodlist_sortfunc failed to restore first item from cur_list"); return 1; }

  i_list_move_head (next_list);
  next_item = i_list_restore (next_list);
  if (!next_item)
  { i_printf (1, "l_inventory_vendormain_prodlist_sortfunc failed to restore first item from next_list"); return 1; }

  return l_inventory_listsort_typeproduct_func (cur_item, next_item);
}
