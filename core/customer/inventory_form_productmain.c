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

int form_inventory_productmain (i_resource *self, i_form_reqdata *reqdata)
{
  char *product_str;
  char *vendor_str;
  char *frame_title;
  char *labels[5];
  i_list *list;
  i_form_item *hw_table = NULL;
  i_form_item *sw_table = NULL;
  i_form_item *other_table = NULL;
  i_inventory_item *invitem;
  i_hashtable *product_table;
  
  if (!self || !reqdata) return -1;

  /* Form setup */
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_inventory_productmain failed to create form"); return -1; }

  if (!reqdata->form_passdata || reqdata->form_passdata_size < 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No device specified"); return 1; }

  vendor_str = (char *) reqdata->form_passdata;
  product_str = strchr (vendor_str, ':');
  if (!product_str)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Passdata is corrupt"); return 1; }
  *product_str = '\0';
  product_str++;

  /* Get product table for specified vendor */

  product_table = l_inventory_get_vendor (self, vendor_str);
  if (!product_table)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified vendor not found"); return 1; }

  /* Get list of entries for product */

  list = l_inventory_get_product (self, product_table, product_str);
  if (!list || list->size < 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Product not found in registry"); return 1; }

  /* Stats */

  i_list_move_head (list);
  invitem = i_list_restore (list);
  if (!invitem)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to restore first invitem in list"); return 1; }

  asprintf (&frame_title, "Inventory - '%s' '%s' Information", invitem->vendor_str, invitem->product_str);
  i_form_frame_start (reqdata->form_out, "productmain", frame_title);
  free (frame_title);
  
  l_inventory_form_links (self, reqdata);
  i_form_spacer_add (reqdata->form_out);

  i_form_string_add (reqdata->form_out, "vendor_str", "Vendor", invitem->vendor_str);
  i_form_string_add (reqdata->form_out, "product_str", "Product", invitem->product_str);

  i_form_spacer_add (reqdata->form_out);

  /* Items */

  for (i_list_move_head(list); (invitem=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    int row;
    i_form_item *cur_table = NULL;

    if (invitem->type & INV_HARDWARE)
    {
      /* Hardware item */
      if (!hw_table)
      {
        i_form_string_add (reqdata->form_out, "space", "", NULL);
        hw_table = i_form_table_create (reqdata->form_out, "hardware_table", "Hardware Items", 5);
        if (!hw_table) 
        { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add hardware table"); return 1; }
        i_form_item_add_option (hw_table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);   /* Use proportional spacing */
        i_form_item_add_option (hw_table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);        /* 100% Width for main table */
        labels[0] = "Serial Number";
        labels[1] = "Device";
        labels[2] = "Site";
        labels[3] = "Item Type";
        labels[4] = "Item History";
        i_form_table_add_row (hw_table, labels);
      }

      cur_table = hw_table;

      labels[0] = invitem->serial_str;
      labels[1] = invitem->dev->desc_str;
      labels[2] = invitem->dev->site->desc_str;
      labels[3] = i_inventory_type_str (invitem->type);
      labels[4] = "History";

      row = i_form_table_add_row (hw_table, labels);

    }
    else if (invitem->type & INV_SOFTWARE)
    {
      /* Software Item */
      if (!sw_table)
      {
        i_form_string_add (reqdata->form_out, "space", "", NULL);
        sw_table = i_form_table_create (reqdata->form_out, "software_table", "Software Items", 5);
        if (!sw_table)
        { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add software table"); return 1; }
        i_form_item_add_option (sw_table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);   /* Use proportional spacing */
        i_form_item_add_option (sw_table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);        /* 100% Width for main table */
        labels[0] = "Version Number";
        labels[1] = "Device";
        labels[2] = "Site";
        labels[3] = "Item Type";
        labels[4] = "Item History";
        i_form_table_add_row (sw_table, labels);
      }

      cur_table = sw_table;

      labels[0] = invitem->version_str;
      labels[1] = invitem->dev->desc_str;
      labels[2] = invitem->dev->site->desc_str;
      labels[3] = i_inventory_type_str (invitem->type);
      labels[4] = "History";

      row = i_form_table_add_row (sw_table, labels);
    }
    else
    {
      /* Other Item */
      if (!other_table)
      {
        i_form_string_add (reqdata->form_out, "space", "", NULL);
        other_table = i_form_table_create (reqdata->form_out, "other_table", "Other Items", 5);
        if (!other_table)
        { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add other items table"); return 1; }
        i_form_item_add_option (other_table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);   /* Use proportional spacing */
        i_form_item_add_option (other_table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);        /* 100% Width for main table */
        labels[0] = "";
        labels[1] = "Device";
        labels[2] = "Site";
        labels[3] = "Item Type";
        labels[4] = "Item History";
        i_form_table_add_row (other_table, labels);
      }
      
      cur_table = other_table;

      labels[0] = "";
      labels[1] = invitem->dev->desc_str;
      labels[2] = invitem->dev->site->desc_str;
      labels[3] = i_inventory_type_str (invitem->type);
      labels[4] = "History";
      
      row = i_form_table_add_row (sw_table, labels);
    }

    if (cur_table)
    {
      /* Generic links */
      char *dev_addr_str;
      char *pass_str;
      
      i_form_table_add_link (cur_table, 2, row, NULL, ENT_ADDR(invitem->dev->site), "inventory_sitemain", 0, invitem->dev->site->name_str, strlen(invitem->dev->site->name_str)+1);
      
      dev_addr_str = i_resource_address_struct_to_string (invitem->dev->resaddr);
      i_form_table_add_link (cur_table, 1, row, NULL, ENT_ADDR(invitem->dev), "inventory_devicemain", 0, dev_addr_str, strlen(dev_addr_str)+1);
      asprintf (&pass_str, "%s:%p", dev_addr_str, invitem);
      free (dev_addr_str);
      i_form_table_add_link (cur_table, 4, row, RES_ADDR(self), NULL, "inventory_historymain", 0, pass_str, strlen(pass_str)+1);
      free (pass_str);                    
    }
  }
    
  /* Finished */

  i_form_frame_end (reqdata->form_out, "productmain");

  return 1;
}

