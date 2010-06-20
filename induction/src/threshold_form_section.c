#include <stdio.h>
#include <stdlib.h>

#include "induction.h"
#include "threshold.h"
#include "form.h"
#include "list.h"
#include "auth.h"
#include "hierarchy.h"

int form_threshold_section (i_resource *self, i_form_reqdata *reqdata)
{
  int row;
  char *title;
  char *labels[7] = { "Object Description", "Value Description", "Alert Value", "Critical Value", "Last Recorded", "Tolerance", "Edit" };
  i_form_item *table;
  i_threshold *thold;
  i_threshold_section *section;

  if (!self || !reqdata) return -1;
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT) 
  { return i_form_deliver_denied (self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_threshold_section failed to create form"); return -1; }
  asprintf (&title, "%s at %s Threshold Section", self->hierarchy->device_desc, self->hierarchy->site_desc);
  i_form_set_title (reqdata->form_out, title);
  free (title);

  i_form_string_add (reqdata->form_out, "site", "Site", self->hierarchy->site_desc);      
  i_form_string_add (reqdata->form_out, "device", "Device", self->hierarchy->device_desc);        
  i_form_string_add (reqdata->form_out, "device_id", "Device ID", self->hierarchy->device_id);

  if (!reqdata->form_passdata || reqdata->form_passdata_size < 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No section specified"); return 1; }

  section = i_threshold_section_get (self, reqdata->form_passdata);
  if (!section)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to find specified threshold section"); return 1; }
  asprintf (&title, "%s at %s Threshold", self->hierarchy->device_desc, self->hierarchy->site_desc, section->desc);
  i_form_set_title (reqdata->form_out, title);
  free (title);

  i_form_string_add (reqdata->form_out, "section_desc", "Threshold Section", section->desc);
  i_form_spacer_add (reqdata->form_out);

  if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
  { table = i_form_table_create (reqdata->form_out, "thresholds", "Thresholds", 7); }
  else
  { table = i_form_table_create (reqdata->form_out, "thresholds", "Thresholds", 6); }
  if (!table) 
  { i_form_string_add (reqdata->form_out, "msg", "Threshold Table", "Failed to create table"); return 1; }
  i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */                  
  i_form_table_add_row (table, labels);

  for (i_list_move_head(section->thresholds); (thold=i_list_restore(section->thresholds))!=NULL; i_list_move_next(section->thresholds))
  {
    char *pass_str;
    char *res_addr_str;

    if (thold->object_desc) 
    { labels[0] = strdup (thold->object_desc); }
    else
    { labels[0] = strdup ("Unknown"); }

    if (thold->value_desc) 
    { labels[1] = strdup (thold->value_desc); }
    else
    { labels[1] = strdup ("Unknown"); }

    asprintf (&labels[2], "%.2f", thold->alert_value);
    asprintf (&labels[3], "%.2f", thold->critical_value);
    asprintf (&labels[4], "%.2f", thold->last_recorded);
    asprintf (&labels[5], "%.2f%%", thold->tolerance_percent);
    
    row = i_form_table_add_row (table, labels);

    res_addr_str = i_resource_address_struct_to_string (RES_ADDR(self));
    asprintf (&pass_str, "%s:%s", section->name, thold->name);
    i_form_table_add_link (table, 6, row, res_addr_str, "threshold_edit", pass_str, strlen(pass_str)+1);
    free (pass_str);
    free (res_addr_str);

    free (labels[0]);
    free (labels[1]);
    free (labels[2]);
    free (labels[3]);
    free (labels[4]);
    free (labels[5]);
  }

  i_form_string_add (reqdata->form_out, "space", " ", NULL);
  i_form_string_add (reqdata->form_out, "note", "Note", "A threshold of -1 results in the threshold not being actively monitored");      

  return 1;
}
