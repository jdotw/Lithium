#include <stdio.h>
#include <stdlib.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/opstate.h>
#include <induction/colour.h>
#include <induction/auth.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/hierarchy.h>

#include "device.h"

int form_device_list (i_resource *self, i_form_reqdata *reqdata)
{
  char *frame_str;
  char *labels[7];
  i_form_item *device_table;
  i_site *site;
  i_device *device;

  /* Authentication checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_device_list failed to create form"); return -1; }

  /* Retrieve site */
  if (reqdata->entaddr)
  {
    site = (i_site *) i_entity_local_get (self, reqdata->entaddr);
    if (!site)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified site not found"); return 1; }
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No site specified"); return 1; }

  /* Start frame */
  asprintf (&frame_str, "%s %s Device List", site->desc_str, site->suburb_str);
  i_form_frame_start (reqdata->form_out, "dev_list", frame_str);
  free (frame_str);

  /* Device List */
//  if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
//  { device_table = i_form_table_create (reqdata->form_out, "device_list", NULL, 7); }
//  else
//  { device_table = i_form_table_create (reqdata->form_out, "device_list", NULL, 4); }
  device_table = i_form_table_create (reqdata->form_out, "device_list", NULL, 3);

  if (!device_table)
  { i_form_string_add (reqdata->form_out, "error", "Device List", "Unable to create Device List table"); return 1; }
  i_form_item_add_option (device_table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (device_table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width */
  labels[0] = "Device Description";
  labels[1] = "Management IP";
  labels[2] = "State";
  labels[3] = "Licensed";
//  labels[5] = "Edit";
//  labels[6] = "Remove";
  i_form_table_add_row (device_table, labels);  

  for (i_list_move_head(site->dev_list); (device=i_list_restore(site->dev_list)) != NULL; i_list_move_next(site->dev_list))
  {
    int row;

    labels[0] = device->desc_str;
    labels[1] = device->ip_str;
    if (device->licensed)
    { 
      if (device->resaddr)
      { labels[2] = i_entity_opstatestr (device->opstate); }
      else
      { labels[2] = "(Starting)"; }
      labels[3] = "Yes"; 
    }
    else
    { 
      labels[2] = "N/A";
      labels[3] = "No"; 
    }
    
    row = i_form_table_add_row (device_table, labels);

    if (device->resaddr && device->licensed)
    {
      i_form_item_add_option (device_table, ITEM_OPTION_FGCOLOR, 1, 3, row, i_colour_fg_str(device->opstate), 8);
      if (device->opstate > ENTSTATE_NORMAL)
      { i_form_item_add_option (device_table, ITEM_OPTION_FGCOLOR, 1, 0, row, i_colour_fg_str(device->opstate), 8); }
      i_form_table_add_link (device_table, 0, row, device->resaddr, NULL, "main", 0, NULL, 0);
    }
    else if (!device->licensed)
    { i_form_item_add_option (device_table, ITEM_OPTION_FGCOLOR, 1, 4, row, i_colour_fg_str(ENTSTATE_FAILED), 8); }

//    if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
//    { 
//      i_form_table_add_link (device_table, 5, row, NULL, ENT_ADDR(device), "device_edit", 0, NULL, 0); 
//      i_form_table_add_link (device_table, 6, row, NULL, ENT_ADDR(device), "device_remove", 0, NULL, 0);
//    }
  }

//  if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
//  { 
//    item = i_form_string_add (reqdata->form_out, "device_add", "Add Device", NULL);
//    i_form_item_add_link (item, 0, 0, 0, NULL, ENT_ADDR(site), "device_edit", 0, NULL, 0);
//  }

  i_form_frame_end (reqdata->form_out, "dev_list");

  return 1;
}

