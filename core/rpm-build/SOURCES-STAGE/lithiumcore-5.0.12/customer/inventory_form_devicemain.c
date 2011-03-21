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

int form_inventory_devicemain (i_resource *self, i_form_reqdata *reqdata)
{
  char *frame_title;
  char *addr_str;
  char *labels[6];
  i_list *list;
  i_form_item *hw_table = NULL;
  i_form_item *sw_table = NULL;
  i_form_item *other_table = NULL;
  i_form_item *fitem;
  i_inventory_item *invitem;
  
  if (!self || !reqdata) return -1;

  /* Form setup */
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_inventory_devicemain failed to create form"); return -1; }

  if (!reqdata->form_passdata || reqdata->form_passdata_size < 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No device specified"); return 1; }
  addr_str = (char *) reqdata->form_passdata;

  /* Get list of all matching devices */

  list = l_inventory_get_device_str (self, addr_str);
  if (!list || list->size < 1)
  { 
    i_form_frame_start (reqdata->form_out, "devicemain", "Inventory - Device Information");
    
    l_inventory_form_links (self, reqdata);
    i_form_spacer_add (reqdata->form_out);

    i_form_string_add (reqdata->form_out, "msg", "No Items", "No inventory items are registered to this device"); 

    /* Manual add link */
    if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
    {
      char *pass_str;
      
      i_form_spacer_add (reqdata->form_out);

      /* Add string for 'Add static item' link */
      fitem = i_form_string_add (reqdata->form_out, "add_static", "Add static item", NULL);

      /* Add link */
      asprintf (&pass_str, "%s:%p", (char *)reqdata->form_passdata, NULL);
      i_form_item_add_link (fitem, 0, 0, 0, RES_ADDR(self), NULL, "inventory_static_edit", 0, pass_str, strlen(pass_str)+1);
      free (pass_str);
    }

    i_form_frame_end (reqdata->form_out, "devicemain");

    return 1; 
  }

  /* Stats */

  i_list_move_head (list);
  invitem = i_list_restore (list);
  if (!invitem)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to restore first invitem in list"); return 1; }

  asprintf (&frame_title, "Inventory - Device '%s' at '%s' Information", invitem->dev->desc_str, invitem->dev->site->desc_str);
  i_form_frame_start (reqdata->form_out, "devicemain", frame_title);
  free (frame_title);
  
  l_inventory_form_links (self, reqdata);
  i_form_spacer_add (reqdata->form_out);

  fitem = i_form_string_add (reqdata->form_out, "device_desc", "Device Name", invitem->dev->desc_str);
  i_form_item_add_link (fitem, 0, 0, 0, invitem->dev->resaddr, NULL, "main", 0, NULL, 0);

  fitem = i_form_string_add (reqdata->form_out, "site_name", "Site Name", invitem->dev->site->desc_str);
  i_form_item_add_link (fitem, 0, 0, 0, NULL, ENT_ADDR(invitem->dev->site), "device_list", 0, invitem->dev->site->name_str, strlen(invitem->dev->site->name_str)+1);

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
        if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
        { hw_table = i_form_table_create (reqdata->form_out, "hardware_table", "Hardware Items", 6); }
        else
        { hw_table = i_form_table_create (reqdata->form_out, "hardware_table", "Hardware Items", 5); }
        if (!hw_table) { i_printf (1, "form_inventory_devicemain failed to create address_table"); return -1; }
        i_form_item_add_option (hw_table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
        i_form_item_add_option (hw_table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

        labels[0] = "Vendor";
        labels[1] = "Product";
        labels[2] = "Serial Number";
        labels[3] = "Item Type";
        labels[4] = "Item History";
        labels[5] = "Edit/Remove";
        i_form_table_add_row (hw_table, labels);
      }

      cur_table = hw_table;
      labels[0] = invitem->vendor_str;
      labels[1] = invitem->product_str;
      labels[2] = invitem->serial_str;
      labels[3] = i_inventory_type_str (invitem->type);
      labels[4] = "History";
      if (invitem->flags & INV_FLAG_STATIC)
      { labels[5] = "Edit/Remove"; }
      else
      { labels[5] = ""; }

      row = i_form_table_add_row (hw_table, labels);
    }
    else if (invitem->type & INV_SOFTWARE)
    {
      /* Software Item */
      if (!sw_table)
      {
        i_form_string_add (reqdata->form_out, "space", "", NULL);
        if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
        { sw_table = i_form_table_create (reqdata->form_out, "software_table", "Software Items", 6); }
        else
        { sw_table = i_form_table_create (reqdata->form_out, "software_table", "Software Items", 5); }
        if (!sw_table) { i_printf (1, "form_inventory_devicemain failed to create address_table"); return -1; }
        i_form_item_add_option (sw_table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
        i_form_item_add_option (sw_table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

        labels[0] = "Vendor";
        labels[1] = "Product";
        labels[2] = "Version";
        labels[3] = "Item Type";
        labels[4] = "Item History";
        labels[5] = "Edit/Remove";
        i_form_table_add_row (sw_table, labels);
      }

      cur_table = sw_table;
      labels[0] = invitem->vendor_str;
      labels[1] = invitem->product_str;
      labels[2] = invitem->version_str;
      labels[3] = i_inventory_type_str (invitem->type);
      labels[4] = "History";
      if (invitem->flags & INV_FLAG_STATIC)
      { labels[5] = "Edit/Remove"; }
      else
      { labels[5] = ""; }

      row = i_form_table_add_row (sw_table, labels);
    }
    else
    {
      /* Other Item */
      if (!other_table)
      {
        i_form_string_add (reqdata->form_out, "space", "", NULL);
        if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
        { other_table = i_form_table_create (reqdata->form_out, "other_table", "Other Items", 6); }
        else
        { other_table = i_form_table_create (reqdata->form_out, "other_table", "Other Items", 5); }
        if (!other_table) { i_printf (1, "form_inventory_devicemain failed to create address_table"); return -1; }
        i_form_item_add_option (other_table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
        i_form_item_add_option (other_table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

        labels[0] = "Vendor";
        labels[1] = "Product";
        labels[2] = "";
        labels[3] = "Item Type";
        labels[4] = "Item History";
        labels[5] = "Edit/Remove";
        i_form_table_add_row (other_table, labels);
      }

      cur_table = other_table;
      labels[0] = invitem->vendor_str;
      labels[1] = invitem->product_str;
      labels[2] = "";
      labels[3] = i_inventory_type_str (invitem->type);
      labels[4] = "History";
      if (invitem->flags & INV_FLAG_STATIC)
      { labels[5] = "Edit/Remove"; }
      else
      { labels[5] = ""; }

      row = i_form_table_add_row (other_table, labels);
    }

    if (cur_table)
    {
      char *dev_addr_str;
      char *pass_str;

      i_form_table_add_link (cur_table, 0, row, RES_ADDR(self), NULL, "inventory_vendormain", 0, invitem->vendor_str, strlen(invitem->vendor_str)+1);
      asprintf (&pass_str, "%s:%s", invitem->vendor_str, invitem->product_str);
      i_form_table_add_link (cur_table, 1, row, RES_ADDR(self), NULL, "inventory_productmain", 0, pass_str, strlen(pass_str)+1);
      free (pass_str);

      dev_addr_str = i_resource_address_struct_to_string (invitem->dev->resaddr);
      asprintf (&pass_str, "%s:%p", dev_addr_str, invitem);
      free (dev_addr_str);
      i_form_table_add_link (cur_table, 4, row, RES_ADDR(self), NULL, "inventory_historymain", 0, pass_str, strlen(pass_str)+1);
      if (invitem->flags & INV_FLAG_STATIC && reqdata->auth->level >= AUTH_LEVEL_STAFF)
      { i_form_table_add_link (cur_table, 5, row, RES_ADDR(self), NULL, "inventory_static_edit", 0, pass_str, strlen(pass_str)+1); }
      free (pass_str);

    }
  }

  /* Manual add link */
  if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
  {
    char *pass_str;
    
    i_form_spacer_add (reqdata->form_out);

    /* Add string for 'Add static item' link */
    fitem = i_form_string_add (reqdata->form_out, "add_static", "Add static item", NULL);

    /* Add link */
    asprintf (&pass_str, "%s:%p", (char *)reqdata->form_passdata, NULL);
    i_form_item_add_link (fitem, 0, 0, 0, RES_ADDR(self), NULL, "inventory_static_edit", 0, pass_str, strlen(pass_str)+1);
    free (pass_str);
  }
  
  /* Finished */

  i_form_frame_end (reqdata->form_out, "devicemain");

  return 1;
}

