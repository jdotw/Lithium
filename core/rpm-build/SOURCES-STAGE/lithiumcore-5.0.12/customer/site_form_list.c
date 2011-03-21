#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/colour.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/hierarchy.h>
#include <induction/site.h>
#include <induction/list.h>
#include <induction/auth.h>

#include "site.h"

int form_site_list (i_resource *self, i_form_reqdata *reqdata)
{
  i_site *site;
  i_form_item *item;
  char *labels[7];

  if (!self || !reqdata) return -1;

  /* Authentication checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }

  /* Form Creation */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_user_list failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "sitelist", "Site List");
            
  /* Create table */
//  if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
//  { item = i_form_table_create (reqdata->form_out, "site_list", NULL, 7); }
//  else
//  { item = i_form_table_create (reqdata->form_out, "site_list", NULL, 4); }
  item = i_form_table_create (reqdata->form_out, "site_list", NULL, 4);
  if (!item) { i_printf (1, "l_site_formsection unable to create table"); return 0; }

  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width */

  labels[0] = "Site Name";
  labels[1] = "Suburb";
  labels[2] = "Address";
  labels[3] = "State";
//  labels[4] = "Licensed";
//  labels[5] = "Edit";
//  labels[6] = "Remove";
  i_form_table_add_row (item, labels);
  
  for (i_list_move_head(self->hierarchy->cust->site_list); (site=i_list_restore(self->hierarchy->cust->site_list)) != NULL; i_list_move_next(self->hierarchy->cust->site_list))
  {
    int row;

    if (site->desc_str)
    { labels[0] = site->desc_str; }
    else
    { labels[0] = site->name_str; }
    labels[1] = site->suburb_str;
    labels[2] = site->addr1_str;

    labels[3] = i_entity_opstatestr (site->opstate);
    
    row = i_form_table_add_row (item, labels);
    
    i_form_table_add_link (item, 0, row, NULL, ENT_ADDR(site), "device_list", 0, NULL, 0); 
    i_form_item_add_option (item, ITEM_OPTION_FGCOLOR, 1, 3, row, i_colour_fg_str(site->opstate), 8); 
    if (site->opstate > ENTSTATE_NORMAL)
    { i_form_item_add_option (item, ITEM_OPTION_FGCOLOR, 1, 0, row, i_colour_fg_str(site->opstate), 8); }

//    if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
//    {
//      i_form_table_add_link (item, 5, row, NULL, ENT_ADDR(site), "site_edit", 0, NULL, 0);
//      i_form_table_add_link (item, 6, row, NULL, ENT_ADDR(site), "site_remove", 0, NULL, 0);
//    }
  }

//  if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
//  { 
//    item = i_form_string_add (reqdata->form_out, "add", "Add New Site", NULL);
//    if (!item) { i_printf (1, "l_site_formsection failed to create Add Site string"); }
//    i_form_item_add_link (item, 0, 0, 0, NULL, NULL, "site_edit", 0, NULL, 0);
//  }

  return 1;
}

