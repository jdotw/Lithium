#include <stdio.h>
#include <stdlib.h>

#include "induction.h"
#include "threshold.h"
#include "list.h"
#include "form.h"
#include "auth.h"
#include "hierarchy.h"

int form_threshold_main (i_resource *self, i_form_reqdata *reqdata)
{
  int row;
  char *title;
  char *labels[2] = { "Description", "Name" };
  i_list *sections;
  i_form_item *table;
  i_threshold_section *section;

  if (!self || !reqdata) return -1;
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_threshold_main failed to create form"); return -1; }
  asprintf (&title, "Threshold Sections on %s at %s", self->hierarchy->device_desc, self->hierarchy->site_desc);
  i_form_set_title (reqdata->form_out, title);
  free (title);

  i_form_string_add (reqdata->form_out, "site", "Site", self->hierarchy->site_desc);
  i_form_string_add (reqdata->form_out, "device", "Device", self->hierarchy->device_desc);
  i_form_string_add (reqdata->form_out, "device_id", "Device ID", self->hierarchy->device_id);
  i_form_spacer_add (reqdata->form_out);

  table = i_form_table_create (reqdata->form_out, "threshold_sections", "Threshold Sections", 2);
  if (!table)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create threshold sections table"); return 1; }
  i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */          
  i_form_table_add_row (table, labels);

  sections = i_threshold_section_list ();
  if (sections && sections->size > 0)
  {
  
    for (i_list_move_head(sections); (section=i_list_restore(sections))!=NULL; i_list_move_next(sections))
    {
      char *res_addr_str;
  
      if (section->desc)
      { labels[0] = strdup (section->desc); }
      else
      { labels[0] = strdup ("Unknown"); }
  
      if (section->name)
      { labels[1] = strdup (section->name); }
      else
      { labels[1] = strdup ("Unknown"); }
  
      row = i_form_table_add_row (table, labels);
  
      res_addr_str = i_resource_address_struct_to_string (RES_ADDR(self));
      i_form_table_add_link (table, 0, row, res_addr_str, "threshold_section", labels[1], strlen(labels[1])+1);
      free (res_addr_str);

      free (labels[0]);
      free (labels[1]);
    }
  }
  else
  { i_form_string_add (reqdata->form_out, "msg", "Threshold Sections", "No thresholds are currently configured"); }

  return 1;
}
  
