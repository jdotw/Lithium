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
#include <induction/callback.h>

#include "inventory.h"

int broken_form_inventory_static_edit (i_resource *self, i_form_reqdata *reqdata)
{
  /* In the reqdata->form_passdata will be the 
   * address for the device and a pointee to an existing
   * item (or 0x0 for new entry). First, we need to get
   * the hierarchy of the device to know which site it
   * belongs to and the address of the site etc.
   */

  char *dev_addr_str;
  char *strptr;
  i_resource_address *dev_addr;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied(self, reqdata); }

  /* Create form */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_inventory_static_edit failed to create form"); return -1; }

  /* Check passdata */
  if (!reqdata->form_passdata || reqdata->form_passdata_size < 1)
  {
    /* No passdata (device resaddrstr) */
    i_form_string_add (reqdata->form_out, "error", "Error", "No device specified"); 
    return 1;
  }

  /* Extract device addr from passdata
   * WITHOUT altering the passdata (yet)
   */
  dev_addr_str = strdup ((char *) reqdata->form_passdata);
  strptr = strrchr (dev_addr_str, ':');
  if (!strptr)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Passdata corrupt"); free (dev_addr_str); return 1; }
  *strptr = '\0';

  /* Convert passdata to device addr struct */
  dev_addr = i_resource_address_string_to_struct (dev_addr_str);
  free (dev_addr_str);
  if (!dev_addr)
  {
    i_form_string_add (reqdata->form_out, "error", "Error", "Failed to convert specified device address string to an address struct");
    return 1;
  }

  /* FIX Defunct */
//  /* Retrieve hierarchy for device */
//  hier_callback = i_hierarchy_get (self, dev_addr, l_inventory_static_form_add_hiercb, reqdata);
//  i_resource_address_free (dev_addr);
//  if (!hier_callback)
//  { 
//    i_form_string_add (reqdata->form_out, "error", "Error", "Failed to convert specified device address string to an address struct"); 
//    return 1;
//  }

  return 0;     /* async form op, return 0 -- form not yet ready */
}

int l_inventory_static_form_add_hiercb (i_resource *self, i_hierarchy *hierarchy, void *passdata)
{
  /* Called when the hierarchy has been retrieved */

  char *str;
  char *res_addr_str;
  i_form_item *fitem;
  i_form_reqdata *reqdata = passdata;
  
  if (hierarchy)
  {
    /* Hierarchy found */
    char *dev_addr_str;
    char *invitemptr_str;
    void *invitemptr;
    i_inventory_item *invitem;

    /* Interpret passdata 
     *
     * This does mangle passdata in the process. From here on in
     * Passdata is a null terminated string of the dev_addr with
     * trailing bits and peices of the ptr addr after the \0
     */
    dev_addr_str = (char *) reqdata->form_passdata;
    invitemptr_str = strrchr (dev_addr_str, ':');
    if (!invitemptr_str)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Passdata is corrupt"); i_form_deliver (self, reqdata); return 0; }
    *invitemptr_str = '\0';
    invitemptr_str++;
    invitemptr = (void *) strtoul (invitemptr_str, NULL, 16);

    if (invitemptr)
    {
      i_list *devitem_list;

      i_form_frame_start (reqdata->form_out, "staticedit", "Inventory - Edit Static Item");
      i_form_option_add (reqdata->form_out, FORM_OPTION_SUBMIT_LABEL, "Update Item", 12);
        
      l_inventory_form_links (self, reqdata);
      i_form_spacer_add (reqdata->form_out);

      /* Find existing invitem */
      devitem_list = l_inventory_get_device_str (self, dev_addr_str);
      for (i_list_move_head(devitem_list); (invitem=i_list_restore(devitem_list))!=NULL; i_list_move_next(devitem_list))
      { if (invitem == invitemptr) break; }

      if (!invitem)
      { i_form_string_add (reqdata->form_out, "error", "Error", "Specified inventory item not found"); i_form_deliver (self, reqdata); return 0; }
      
      if (!(invitem->flags & INV_FLAG_STATIC))
      { i_form_string_add (reqdata->form_out, "error", "Error", "Specified inventory item is not a static item"); i_form_deliver (self, reqdata); return 0; }
    }
    else
    { 
      i_form_frame_start (reqdata->form_out, "staticedit", "Inventory - Add Static Item");
      i_form_option_add (reqdata->form_out, FORM_OPTION_SUBMIT_LABEL, "Add Item", 9);
      invitem = NULL; 
      l_inventory_form_links (self, reqdata);
      i_form_spacer_add (reqdata->form_out);
    }
  
    /* Make form submitable */
    i_form_set_submit (reqdata->form_out, 1);

    /* Device/Site info */
    i_form_string_add (reqdata->form_out, "site_desc", "Site", hierarchy->site_desc);
    i_form_string_add (reqdata->form_out, "device_desc", "Device", hierarchy->device_desc);
    i_form_spacer_add (reqdata->form_out);

    /* Remove link */
    if (invitem)
    {
      char *pass_str;

      fitem = i_form_string_add (reqdata->form_out, "remove_link", "Remove", "Remove Item");

      asprintf (&pass_str, "%s:%p", dev_addr_str, invitem);
      i_form_item_add_link (fitem, 0, 0, 0, RES_ADDR(self), NULL, "inventory_static_remove", 0, pass_str, strlen(pass_str));
      free (pass_str);

      i_form_spacer_add (reqdata->form_out);
    }

    /* Hidden info */
    res_addr_str = i_resource_address_struct_to_string (hierarchy->device_addr);
    i_form_hidden_add (reqdata->form_out, "device_addr", res_addr_str);
    free (res_addr_str);
    if (invitem)
    { i_form_hidden_add (reqdata->form_out, "invitemptr_str", invitemptr_str); }

    /* Item type */
    fitem = i_form_dropdown_create ("type", "Item Type");

    asprintf (&str, "%i", INV_CHASSIS); 
    if (invitem) i_form_dropdown_add_option (fitem, str, i_inventory_type_str(INV_CHASSIS), (invitem->type == INV_CHASSIS));
    else i_form_dropdown_add_option (fitem, str, i_inventory_type_str(INV_CHASSIS), 1);
    free (str);
    
    asprintf (&str, "%i", INV_MAINBOARD);    
    if (invitem) i_form_dropdown_add_option (fitem, str, i_inventory_type_str(INV_MAINBOARD), (invitem->type == INV_MAINBOARD));
    else i_form_dropdown_add_option (fitem, str, i_inventory_type_str(INV_MAINBOARD), 0);
    free (str);
    
    asprintf (&str, "%i", INV_INTCARD);    
    if (invitem) i_form_dropdown_add_option (fitem, str, i_inventory_type_str(INV_INTCARD), (invitem->type == INV_INTCARD));
    else i_form_dropdown_add_option (fitem, str, i_inventory_type_str(INV_INTCARD), 0);
    free (str);
    
    asprintf (&str, "%i", INV_STORAGE);    
    if (invitem) i_form_dropdown_add_option (fitem, str, i_inventory_type_str(INV_STORAGE), (invitem->type == INV_STORAGE));
    else i_form_dropdown_add_option (fitem, str, i_inventory_type_str(INV_STORAGE), 0);
    free (str);
    
    asprintf (&str, "%i", INV_PERIPHERAL);    
    if (invitem) i_form_dropdown_add_option (fitem, str, i_inventory_type_str(INV_PERIPHERAL), (invitem->type == INV_PERIPHERAL));
    else i_form_dropdown_add_option (fitem, str, i_inventory_type_str(INV_PERIPHERAL), 0);
    free (str);
    
    asprintf (&str, "%i", INV_OS);    
    if (invitem) i_form_dropdown_add_option (fitem, str, i_inventory_type_str(INV_OS), (invitem->type == INV_OS));
    else i_form_dropdown_add_option (fitem, str, i_inventory_type_str(INV_OS), 0);
    free (str);
    
    asprintf (&str, "%i", INV_APP);    
    if (invitem) i_form_dropdown_add_option (fitem, str, i_inventory_type_str(INV_APP), (invitem->type == INV_APP));
    else i_form_dropdown_add_option (fitem, str, i_inventory_type_str(INV_APP), 0);
    free (str);
    
    asprintf (&str, "%i", INV_DRIVER);    
    if (invitem) i_form_dropdown_add_option (fitem, str, i_inventory_type_str(INV_DRIVER), (invitem->type == INV_DRIVER));
    else i_form_dropdown_add_option (fitem, str, i_inventory_type_str(INV_DRIVER), 0);
    free (str);
    
    i_form_add_item (reqdata->form_out, fitem);

    /* Vendor / Product */
    if (invitem) i_form_entry_add (reqdata->form_out, "vendor_str", "Vendor Name", invitem->vendor_str);
    else i_form_entry_add (reqdata->form_out, "vendor_str", "Vendor Name", NULL);
    if (invitem) i_form_entry_add (reqdata->form_out, "product_str", "Product Name", invitem->product_str);
    else i_form_entry_add (reqdata->form_out, "product_str", "Product Name", NULL);

    /* Version / Serial Number */
    if (invitem) i_form_entry_add (reqdata->form_out, "version_str", "Software Version", invitem->version_str);
    else i_form_entry_add (reqdata->form_out, "version_str", "Software Version", NULL);
    i_form_string_add (reqdata->form_out, "version_str_note", "", "(For software items)");
    
    if (invitem) i_form_entry_add (reqdata->form_out, "serial_str", "Hardware Serial Number", invitem->serial_str);
    else i_form_entry_add (reqdata->form_out, "serial_str", "Hardware Serial Number", NULL);
    i_form_string_add (reqdata->form_out, "serial_str_note", "", "(For hardware items)");
  }
  else
  {
    /* No hierarchy */
    i_form_frame_start (reqdata->form_out, "staticedit", "Inventory - Add/Edit Static Item");
    i_form_string_add (reqdata->form_out, "error", "Error", "Failed to retrieve hierarchy for specified device"); 
  }

  i_form_frame_end (reqdata->form_out, "staticedit");

  /* Deliver form */
  i_form_deliver (self, reqdata);

  return 0;
}

int broken_form_inventory_static_edit_submit (i_resource *self, i_form_reqdata *reqdata)
{
  i_form_item_option *opt;
  i_resource_address *device_addr = NULL;

  /* Create form */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_inventory_static_edit_submit failed to create form"); return -1; }

  /* Get device_addr */
  opt = i_form_get_value_for_item (reqdata->form_in, "device_addr");
  if (opt)
  { device_addr = i_resource_address_string_to_struct ((char *) opt->data); }
  if (!device_addr)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to find/interpret device_addr in form"); return 1; }

  /* FIX Defunct */
  /* Retrieve hierarchy */
//  hier_callback = i_hierarchy_get (self, device_addr, l_inventory_static_form_add_submit_hiercb, reqdata);
//  i_resource_address_free (device_addr);
//  if (!hier_callback)
//  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to request hierarchy from device"); return 1; }

  return 0;
}

int l_inventory_static_form_add_submit_hiercb (i_resource *self, i_hierarchy *hierarchy, void *passdata)
{
  i_form_reqdata *reqdata = passdata;
  
  if (hierarchy)
  {
    int num;
    char *success_str;
    i_form_item_option *opt;
    i_inventory_item *invitem;

    /* Create item struct */
    invitem = i_inventory_item_create ();
    if (!invitem)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create item struct"); return 1; }
//    invitem->hierarchy = i_hierarchy_duplicate (hierarchy);
    /* Set flag */
    invitem->flags += INV_FLAG_STATIC;

    /* Fill item with form values */
  
    opt = i_form_get_value_for_item (reqdata->form_in, "type");
    if (opt)
    { invitem->type = atoi ((char *) opt->data); }

    opt = i_form_get_value_for_item (reqdata->form_in, "vendor_str");
    if (opt)
    { invitem->vendor_str = strdup ((char *) opt->data); }
  
    opt = i_form_get_value_for_item (reqdata->form_in, "product_str");
    if (opt)
    { invitem->product_str = strdup ((char *) opt->data); }

    if (invitem->type & INV_HARDWARE)
    {
      opt = i_form_get_value_for_item (reqdata->form_in, "serial_str");
      if (opt)
      { invitem->serial_str = strdup ((char *) opt->data); }
    }
    else if (invitem->type & INV_SOFTWARE)
    {
      opt = i_form_get_value_for_item (reqdata->form_in, "version_str"); 
      if (opt)
      { invitem->version_str = strdup ((char *) opt->data); }
    }

//    opt = i_form_get_value_for_item (reqdata->form_in, "device_addr");
//    if (opt)
//    { invitem->device_addr = i_resource_address_string_to_struct ((char *) opt->data); }

    opt = i_form_get_value_for_item (reqdata->form_in, "invitemptr_str");
    if (opt)
    {
      /* Editing an existing item
       *
       * Remove the old item from static inventory
       */
      i_inventory_item *existing_item = NULL;
      void *invitemptr;

      /* Find item */
      invitemptr = (void *) strtoul ((char *)opt->data, NULL, 16);
//      devitem_list = l_inventory_get_device (self, invitem->device_addr);
//      for (i_list_move_head(devitem_list); (existing_item=i_list_restore(devitem_list))!=NULL; i_list_move_next(devitem_list))
//      { if (existing_item == invitemptr) break; }

      /* Remove */
      if (existing_item)
      { 
        /* Remove from static inventory */
        l_inventory_static_remove_item (self, existing_item); 
      }

      /* Start Frame and success string */
      i_form_frame_start (reqdata->form_out, "staticedit", "Inventory - Edit Static Item");
      success_str = "Static item successfully updated";
    }
    else
    { 
      i_form_frame_start (reqdata->form_out, "staticedit", "Inventory - Add Static Item");
      success_str = "Static item successfully added"; 
    }
    
    /* Add invitem */
    num = l_inventory_static_add (self, invitem);
    if (num != 0)
    { 
      i_printf (1, "l_inventory_static_form_add_submit_hiercb failed to add static item %s %s at %s in %s",
        invitem->vendor_str, invitem->product_str, invitem->dev->site->desc_str, invitem->dev->desc_str);
      i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add static item"); 
      i_inventory_item_free (invitem);
    }

    /* Success */
    i_form_string_add (reqdata->form_out, "msg", "Success", success_str); 
  }
  else
  {
    /* Failed to get hierarchy */
    i_form_string_add (reqdata->form_out, "error", "Error", "Failed to retrieve hierarchy from device"); 
  }

  i_form_frame_end (reqdata->form_out, "staticedit");

  /* Deliver form */
  i_form_deliver (self, reqdata);

  return 0;
}
