#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/module.h>
#include <induction/auth.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/construct.h>
#include <induction/list.h>
#include <induction/configfile.h>
#include <induction/files.h>
#include <induction/vendor.h>

#include "lic.h"
#include "device.h"

int form_device_edit (i_resource *self, i_form_reqdata *reqdata)
{
  char *str;
  i_device *device;
  i_form_item *item;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN) 
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_device_edit failed to create form"); return -1; }

  /* Check for device/site entity */
  if (reqdata->entaddr)
  {
    i_entity *ent;
    
    /* Entity specified, retrieve it */
    ent = i_entity_local_get (self, reqdata->entaddr);
    if (!ent)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified device or site not found"); return 1; }

    /* Check if a site of a device is specified */
    if (ent->ent_type == ENT_SITE)
    {
      /* Site specified, adding new device */
      device = NULL;
      i_form_frame_start (reqdata->form_out, "device_edit", "Add New Device"); 
    }
    else if (ent->ent_type == ENT_DEVICE)
    {
      /* Device specified, editing existing device */
      device = (i_device *) ent;
      i_form_frame_start (reqdata->form_out, "device_edit", "Edit Existing Device"); 
    }
    else
    { i_form_string_add (reqdata->form_out, "error", "Error", "Invalid entity specified"); return 1; }
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No device or site specified"); return 1; }

  /* Name */
  if (device) 
  { 
    i_form_string_add (reqdata->form_out, "name_label", "Device Name", device->name_str);
    i_form_hidden_add (reqdata->form_out, "name", device->name_str);
  }
  else i_form_entry_add (reqdata->form_out, "name", "Device Name", NULL);

  /* Desc */
  if (device) i_form_entry_add (reqdata->form_out, "desc", "Device Description", device->desc_str);
  else i_form_entry_add (reqdata->form_out, "desc", "Device Description", NULL);

  /* IP */
  if (device) i_form_entry_add (reqdata->form_out, "ip", "Management IP", device->ip_str);
  else i_form_entry_add (reqdata->form_out, "ip", "Management IP", NULL);

  /* LOM IP */
  if (device) i_form_entry_add (reqdata->form_out, "lom_ip", "Lights-Out IP", device->lom_ip_str);
  else i_form_entry_add (reqdata->form_out, "lom_ip", "Lights-Out IP", NULL);

  /* SNMP Community */
  if (device) i_form_entry_add (reqdata->form_out, "snmpcomm", "SNMP Community", device->snmpcomm_str);
  else i_form_entry_add (reqdata->form_out, "snmpcomm", "SNMP Community", NULL);

  /* Username */
  if (device) i_form_entry_add (reqdata->form_out, "username", "Username", device->username_str);
  else i_form_entry_add (reqdata->form_out, "username", "Username", NULL);

  /* Password */
  if (device) i_form_password_add (reqdata->form_out, "password", "New Password", NULL);
  else i_form_password_add (reqdata->form_out, "password", "Password", NULL);

  /* LOM Username */
  if (device) i_form_entry_add (reqdata->form_out, "lom_username", "LOM Username", device->lom_username_str);
  else i_form_entry_add (reqdata->form_out, "lom_username", "LOM Username", NULL);

  /* LOM Password */
  if (device) i_form_password_add (reqdata->form_out, "lom_password", "New LOM Password", NULL);
  else i_form_password_add (reqdata->form_out, "lom_password", "LOM Password", NULL);

  /* Vendor */
  item = i_form_dropdown_create ("vendor", "Vendor Module");
  i_list *vendor_list = i_vendor_list (self);
  i_vendor *vendor;
  for (i_list_move_head(vendor_list); (vendor=i_list_restore(vendor_list))!=NULL; i_list_move_next(vendor_list))
  {
    /* Add option */
    i_form_dropdown_add_option (item, vendor->name_str, vendor->desc_str, 0);
  }
  i_form_add_item (reqdata->form_out, item);
  if (device) i_form_dropdown_set_selected (item, device->vendor_str); 

  /* Profile */
  if (device) i_form_entry_add (reqdata->form_out, "profile", "Device Profile", device->profile_str);
  else i_form_entry_add (reqdata->form_out, "profile", "Device Profile", NULL);

  /* Refresh Interval */
  if (device) 
  {
    asprintf (&str, "%li", device->refresh_interval);
    i_form_entry_add (reqdata->form_out, "refresh_interval", "Refresh Interval (sec)", str);
    free (str);
  }
  else i_form_entry_add (reqdata->form_out, "refresh_interval", "Refresh Interval (sec)", NULL);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, "device_edit");

  return 1;
}

int form_device_edit_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  i_device *device;
  i_form_item_option *name_opt;
  i_form_item_option *desc_opt;
  i_form_item_option *ip_opt;
  i_form_item_option *lom_ip_opt;
  i_form_item_option *snmpcomm_opt;
  i_form_item_option *username_opt;
  i_form_item_option *password_opt;
  i_form_item_option *lom_username_opt;
  i_form_item_option *lom_password_opt;
  i_form_item_option *vendor_opt;
  i_form_item_option *profile_opt;
  i_form_item_option *refresh_opt;
  long refresh_interval = DEVICE_DEFAULT_REFRESH_INTERVAL;

  /* Auth checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }
  
  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_device_add_submit unable to create form"); return -1; }

  /* Name */
  name_opt = i_form_get_value_for_item (reqdata->form_in, "name");
  if (!name_opt || !name_opt->data)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Name not found in form"); return 1; }

  /* Other fields */
  desc_opt = i_form_get_value_for_item (reqdata->form_in, "desc");
  if (!desc_opt || !desc_opt->data)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Description not found in form"); return 1; }
  
  ip_opt = i_form_get_value_for_item (reqdata->form_in, "ip");
  lom_ip_opt = i_form_get_value_for_item (reqdata->form_in, "lom_ip");
  snmpcomm_opt = i_form_get_value_for_item (reqdata->form_in, "snmpcomm");
  username_opt = i_form_get_value_for_item (reqdata->form_in, "username");
  password_opt = i_form_get_value_for_item (reqdata->form_in, "password");
  lom_username_opt = i_form_get_value_for_item (reqdata->form_in, "lom_username");
  lom_password_opt = i_form_get_value_for_item (reqdata->form_in, "lom_password");
  vendor_opt = i_form_get_value_for_item (reqdata->form_in, "vendor");
  profile_opt = i_form_get_value_for_item (reqdata->form_in, "profile");
  refresh_opt = i_form_get_value_for_item (reqdata->form_in, "refresh_interval");
  if (refresh_opt)
  { refresh_interval = atol ((char *)refresh_opt->data); }
  if (!desc_opt || !ip_opt || !snmpcomm_opt || !username_opt || !password_opt || !lom_username_opt || !lom_password_opt || !vendor_opt || !profile_opt || !refresh_opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Form is incomplete"); return 1; }

  /* Check for specified device/site */
  if (reqdata->entaddr)
  {
    i_entity *ent;
    
    /* Site specified, edit existing device */
    ent = i_entity_local_get (self, reqdata->entaddr);
    if (!ent)
    { i_form_string_add (reqdata->form_out, "error", "Error", "No device or site specified"); return 1; }

    /* Check entity type */
    if (ent->ent_type == ENT_DEVICE)
    {
      /* Device specified, update existing device */
      device = (i_device *) ent;
      
      /* Start frame */
      i_form_frame_start (reqdata->form_out, "device_edit", "Edit Existing Device");

      /* Rescind license */
      if (device->licensed)
      { l_lic_rescind (self, ENTITY(device)); }

      /* Description */
      if (device->desc_str) free (device->desc_str);
      if (desc_opt->data) device->desc_str = strdup ((char *)desc_opt->data);
      else device->desc_str = NULL;

      /* IP */
      if (device->ip_str) free (device->ip_str);
      if (ip_opt->data) device->ip_str = strdup ((char *)ip_opt->data);
      else device->ip_str = NULL;

      /* LOM IP */
      if (device->lom_ip_str) free (device->lom_ip_str);
      if (lom_ip_opt->data) device->lom_ip_str = strdup ((char *)lom_ip_opt->data);
      else device->lom_ip_str = NULL;

      /* SNMP Community */
      if (device->snmpcomm_str) free (device->snmpcomm_str);
      if (snmpcomm_opt->data) device->snmpcomm_str = strdup ((char *)snmpcomm_opt->data);
      else device->snmpcomm_str = NULL;
      
      /* Username */
      if (device->username_str) free (device->username_str);
      if (username_opt->data) device->username_str = strdup ((char *)username_opt->data);
      else device->username_str = NULL;
      
      /* Password */
      if (password_opt->data && strlen(password_opt->data) > 0)
      {
        if (device->password_str) free (device->password_str);
        device->password_str = strdup ((char *)password_opt->data);
      }

      /* LOM Username */
      if (device->lom_username_str) free (device->lom_username_str);
      if (lom_username_opt->data) device->lom_username_str = strdup ((char *)lom_username_opt->data);
      else device->lom_username_str = NULL;
      
      /* LOM Password */
      if (lom_password_opt->data && strlen(lom_password_opt->data) > 0)
      {
        if (device->lom_password_str) free (device->lom_password_str);
        device->lom_password_str = strdup ((char *)lom_password_opt->data);
      }

      /* Vendor */
      if (device->vendor_str) free (device->vendor_str);
      if (vendor_opt->data) device->vendor_str = strdup ((char *)vendor_opt->data);
      else device->vendor_str = NULL;

      /* Profile */
      if (device->profile_str) free (device->profile_str);
      if (profile_opt->data) device->profile_str = strdup ((char *)profile_opt->data);
      else device->profile_str = NULL;

      /* Refresh interval */
      device->refresh_interval = refresh_interval;

      /* Call update */
      num = l_device_update (self, device);
      if (num == 0)
      { i_form_string_add (reqdata->form_out, "msg", "Success", "Successfully updated device"); }
      else
      { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to update device"); }
    }
    else if (ent->ent_type == ENT_SITE)
    {
      /* Site specified, adding new device */
      i_site *site = (i_site *) ent;
      
      /* Start frame */
      i_form_frame_start (reqdata->form_out, "device_edit", "Add New Device");

      /* Create device */
      device = l_device_add (self, site, (char *) name_opt->data, (char *) desc_opt->data, (char *) ip_opt->data, (char *) lom_ip_opt->data, 1, (char *) snmpcomm_opt->data, NULL, NULL, 0, 0, (char *) username_opt->data, (char *) password_opt->data, (char *) lom_username_opt->data, (char *) lom_password_opt->data, (char *) vendor_opt->data, (char *) profile_opt->data, refresh_interval, 0, 0, 0, 0, 0, 0, 0);
      if (device)
      { i_form_string_add (reqdata->form_out, "msg", "Success", "Successfully added device"); }
      else
      { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add device"); }
    }
    else 
    { i_form_string_add (reqdata->form_out, "error", "Error", "Invalid entity specified"); }
  }
  else
  {
    /* No site/device specified */
    i_form_string_add (reqdata->form_out, "error", "Error", "No device or site specified"); 
    return 1; 
  }

  i_form_frame_end (reqdata->form_out, "device_edit");
  
  return 1;
}

