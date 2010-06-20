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

int form_inventory_sitemain (i_resource *self, i_form_reqdata *reqdata)
{
  char *frame_title;
  char *name_str;
  char *labels[7];
  i_list *list;
  i_form_item *fitem;
  i_form_item *hw_table = NULL;
  i_form_item *sw_table = NULL;
  i_form_item *other_table = NULL;
  i_inventory_item *invitem;
  
  if (!self || !reqdata) return -1;

  /* Form setup */
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_inventory_sitemain failed to create form"); return -1; }

  if (!reqdata->form_passdata || reqdata->form_passdata_size < 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No site specified"); return 1; }
  name_str = (char *) reqdata->form_passdata;

  /* Get list of all matching sites */

  list = l_inventory_get_site (self, name_str);
  if (!list || list->size < 1)
  { 
    i_form_frame_start (reqdata->form_out, "sitemain", "Inventory - Site Information");
    i_form_string_add (reqdata->form_out, "msg", "No Items", "No inventory items are registered at this site"); 
    i_form_frame_end (reqdata->form_out, "sitemain");
    return 1; 
  }

  /* Stats */

  i_list_move_head (list);
  invitem = i_list_restore (list);
  if (!invitem)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to restore first invitem in list"); return 1; }

  asprintf (&frame_title, "Inventory - Site '%s' Information", invitem->dev->site->desc_str);
  i_form_frame_start (reqdata->form_out, "sitemain", frame_title);
  free (frame_title);
  
  l_inventory_form_links (self, reqdata);
  i_form_spacer_add (reqdata->form_out);

  fitem = i_form_string_add (reqdata->form_out, "site_name", "Site Name", invitem->dev->site->desc_str);
  i_form_item_add_link (fitem, 0, 0, 0, NULL, ENT_ADDR(invitem->dev->site), "device_list", 0, NULL, 0);

  i_form_spacer_add (reqdata->form_out);

  /* Items */
  for (i_list_move_head(list); (invitem=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    int row;
    i_form_item *cur_table;

    if (invitem->type & INV_HARDWARE)
    {
      /* Hardware item */
      if (!hw_table)
      {
        i_form_string_add (reqdata->form_out, "space", "", NULL);
        if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
        { hw_table = i_form_table_create (reqdata->form_out, "hardware_table", "Hardware Items", 7); }
        else
        { hw_table = i_form_table_create (reqdata->form_out, "hardware_table", "Hardware Items", 6); }
        if (!hw_table) { i_printf (1, "form_inventory_sitemain failed to create hw_table"); return -1; }
        i_form_item_add_option (hw_table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);
        i_form_item_add_option (hw_table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);

        labels[0] = "Device";
        labels[1] = "Vendor";
        labels[2] = "Product";
        labels[3] = "Serial Number";
        labels[4] = "Item Type";
        labels[5] = "Item History";
        labels[6] = "Edit/Remove";
        i_form_table_add_row (hw_table, labels);
      }

      cur_table = hw_table;

      labels[0] = invitem->dev->desc_str;
      labels[1] = invitem->vendor_str;
      labels[2] = invitem->product_str;
      labels[3] = invitem->serial_str;
      labels[4] = i_inventory_type_str (invitem->type);
      labels[5] = "History";
      if (invitem->flags & INV_FLAG_STATIC)
      { labels[6] = "Edit/Remove"; }
      else
      { labels[6] = ""; }

      row = i_form_table_add_row (hw_table, labels);
    }
    else if (invitem->type & INV_SOFTWARE)
    {
      /* Software item */
      if (!sw_table)
      {
        i_form_string_add (reqdata->form_out, "space", "", NULL);
        if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
        { sw_table = i_form_table_create (reqdata->form_out, "software_table", "Software Items", 7); }
        else
        { sw_table = i_form_table_create (reqdata->form_out, "software_table", "Software Items", 6); }
        if (!sw_table) { i_printf (1, "form_inventory_sitemain failed to create sw_table"); return -1; }
        i_form_item_add_option (sw_table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);
        i_form_item_add_option (sw_table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);

        labels[0] = "Device";
        labels[1] = "Vendor";
        labels[2] = "Product";
        labels[3] = "Version";
        labels[4] = "Item Type";
        labels[5] = "Item History";
        labels[6] = "Edit/Remove";
        i_form_table_add_row (sw_table, labels);
      }

      cur_table = sw_table;

      labels[0] = invitem->dev->desc_str;
      labels[1] = invitem->vendor_str;
      labels[2] = invitem->product_str;
      labels[3] = invitem->version_str;
      labels[4] = i_inventory_type_str (invitem->type);
      labels[5] = "History";
      if (invitem->flags & INV_FLAG_STATIC)
      { labels[6] = "Edit/Remove"; }
      else
      { labels[6] = ""; }

      row = i_form_table_add_row (sw_table, labels);
    }
    else 
    {
      /* Other item */
      if (!other_table)
      {
        i_form_string_add (reqdata->form_out, "space", "", NULL);
        if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
        { other_table = i_form_table_create (reqdata->form_out, "other_table", "Other Items", 7); }
        else
        { other_table = i_form_table_create (reqdata->form_out, "other_table", "Other Items", 6); }
        if (!other_table) { i_printf (1, "form_inventory_sitemain failed to create address_table"); return -1; }
        i_form_item_add_option (other_table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);
        i_form_item_add_option (other_table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);

        labels[0] = "Device";
        labels[1] = "Vendor";
        labels[2] = "Product";
        labels[3] = "";
        labels[4] = "Item Type";
        labels[5] = "Item History";
        labels[6] = "Edit/Remove";
        i_form_table_add_row (other_table, labels);
      }

      cur_table = other_table;

      labels[0] = invitem->dev->desc_str;
      labels[1] = invitem->vendor_str;
      labels[2] = invitem->product_str;
      labels[3] = "";
      labels[4] = i_inventory_type_str (invitem->type);
      labels[5] = "History";
      if (invitem->flags & INV_FLAG_STATIC)
      { labels[6] = "Edit/Remove"; }
      else
      { labels[6] = ""; }

      row = i_form_table_add_row (other_table, labels);
    }

    if (cur_table)
    {
      char *device_addr_str;
      char *pass_str;
      
      device_addr_str = i_resource_address_struct_to_string (invitem->dev->resaddr);
      i_form_table_add_link (cur_table, 0, row, NULL, ENT_ADDR(invitem->dev), "inventory_devicemain", 0, device_addr_str, strlen(device_addr_str)+1);
      asprintf (&pass_str, "%s:%p", device_addr_str, invitem);
      i_form_table_add_link (cur_table, 5, row, NULL, NULL, "inventory_historymain", 0, pass_str, strlen(pass_str)+1);

      if (invitem->flags & INV_FLAG_STATIC && reqdata->auth->level >= AUTH_LEVEL_STAFF)
      { i_form_table_add_link (cur_table, 6, row, NULL, NULL, "inventory_static_edit", 0, pass_str, strlen(pass_str)+1); }

      free (pass_str);                        
      free (device_addr_str);
    
      i_form_table_add_link (cur_table, 1, row, NULL, NULL, "inventory_vendormain", 0, invitem->vendor_str, strlen(invitem->vendor_str)+1);
    
      asprintf (&pass_str, "%s:%s", invitem->vendor_str, invitem->product_str);
      i_form_table_add_link (cur_table, 2, row, NULL, NULL, "inventory_productmain", 0, pass_str, strlen(pass_str)+1);
      free (pass_str);
    }
  }
  
  /* Finished */

  i_form_frame_end (reqdata->form_out, "sitemain");

  return 1;
}

