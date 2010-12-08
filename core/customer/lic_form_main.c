#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#include "demorego.h"
#include "lic.h"

int form_lic_main (i_resource *self, i_form_reqdata *reqdata)
{
  char *labels[6];
  i_form_item *table;
  i_form_item *item;

  /* Authentication checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN)
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_lic_list failed to create form"); return -1; }

  /* 
   * License Entitlements 
   */

  /* Check license */
  l_lic_entitlement *lic = l_lic_static_entitlement();
  if (lic->free == 1)
  { form_demorego_main (self, reqdata); }
  
  /* Start Key frame */
  i_form_frame_start (reqdata->form_out, "ement_list", "License Entitlements");

  /* Get entitlements */
  l_lic_entitlement *ement = l_lic_static_entitlement();

  /* Customer license */
  if (ement->customer_licensed)
  {
    i_form_string_add (reqdata->form_out, "customer", "Customer Licensed", "Yes");
  }
  else
  {
    i_form_string_add (reqdata->form_out, "customer", "Customer Licensed", "No");
  }
  i_form_spacer_add (reqdata->form_out);
  
  /* Create key table */
  table = i_form_table_create (reqdata->form_out, "ement_list", NULL, 4);
//  i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width */
  labels[0] = "Entitlement Type";
  labels[1] = "Currently Configured";
  labels[2] = "Maximum Allowed";
  labels[3] = "Excess";
  i_form_table_add_row (table, labels);

  /* Devices */
  labels[0] = "Monitored Devices";
  asprintf(&labels[1], "%li", ement->devices_used);
  if (ement->devices_max == 0)
  { asprintf(&labels[2], "Unlimited"); }
  else
  { asprintf(&labels[2], "%li", ement->devices_max); }
  asprintf(&labels[3], "%li", ement->devices_excess);
  i_form_table_add_row (table, labels);
  free (labels[1]);
  free (labels[2]);
  free (labels[3]);

  i_form_spacer_add (reqdata->form_out);
  
  i_form_string_add (reqdata->form_out, "note1", "Note", "\"Current\" is the number of licenses that are currently allocated to configured devices");
  i_form_string_add (reqdata->form_out, "note1", "Note", "\"Excess\" is the number of configured devices for which there is no available license");
  i_form_string_add (reqdata->form_out, "note1", "Note", "\"Maximum Allowed\" is the maximum number of monitored devices that is permitted by virtue of your Lithium License");
  
  /* End frame */
  i_form_frame_end (reqdata->form_out, "ement_list");
  
  /* 
   * License Keys 
   */
     
  /* Start Key frame */
  i_form_frame_start (reqdata->form_out, "key_list", "Installed License Keys");

  /* Create key table */
  table = i_form_table_create (reqdata->form_out, "device_list", NULL, 6);
  i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width */
  labels[0] = "License Key";
  labels[1] = "Type";
  labels[2] = "Serial";
  labels[3] = "Product";
  labels[4] = "Status";
  labels[5] = "Remove";
  i_form_table_add_row (table, labels);  

  /* Add keys */
  l_lic_key *key;
  i_list *keys = l_lic_loadkeys (self);
  for (i_list_move_head(keys); (key=i_list_restore(keys)) != NULL; i_list_move_next(keys))
  {
    int row;

    labels[0] = key->enc_str;
    labels[1] = key->type_str;
    asprintf (&labels[2], "%li", key->serial);
    labels[3] = key->product_str;
    labels[4] = l_lic_key_status (key->status);
    
    row = i_form_table_add_row (table, labels);

    if (reqdata->auth->level >= AUTH_LEVEL_ADMIN && key->id != 0)
    { 
      char *str;
      asprintf (&str, "%li", key->id);
      i_form_table_add_link (table, 5, row, NULL, NULL, "lic_remove", 0, str, strlen(str)+1);
      free (str); 
    }

    free (labels[2]);
  }

  /* Key add link */
  if (reqdata->auth->level >= AUTH_LEVEL_ADMIN)
  { 
    item = i_form_string_add (reqdata->form_out, "lic_add", "Add License Key", NULL);
    i_form_item_add_link (item, 0, 0, 0, NULL, NULL, "lic_add", 0, NULL, 0);
  }

  /* End frame */
  i_form_frame_end (reqdata->form_out, "key_list");

  /* 
   * Apply 
   */

  /* Frame */
  i_form_frame_start (reqdata->form_out, "apply", "Apply License Changes");
  i_form_string_add (reqdata->form_out, "note", "Note", "License/Entitlement changes may not take effect until the customer process is restarted");
  item = i_form_string_add (reqdata->form_out, "link", "Link", "Click here to restart the customer process");
  i_form_item_add_link (item, 0, 0, 0, NULL, NULL, "restart", 0, NULL, 0);
  i_form_frame_end (reqdata->form_out, "apply");
  
  return 1;
}

