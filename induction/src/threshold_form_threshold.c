#include <stdio.h>
#include <stdlib.h>

#include "induction.h"
#include "threshold.h"
#include "form.h"
#include "auth.h"
#include "hierarchy.h"

int form_threshold_edit (i_resource *self, i_form_reqdata *reqdata)
{
  char *title;
  char *str;
  char *section_name;
  char *thold_name;
  i_threshold *thold;
  i_threshold_section *section;

  if (!self || !reqdata) return -1;
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_threshold_edit failed to create form"); return -1; }
  asprintf (&title, "%s at %s Threshold", self->hierarchy->device_desc, self->hierarchy->site_desc);
  i_form_set_title (reqdata->form_out, title);
  free (title);

  i_form_string_add (reqdata->form_out, "site", "Site", self->hierarchy->site_desc);      
  i_form_string_add (reqdata->form_out, "device", "Device", self->hierarchy->device_desc);        
  i_form_string_add (reqdata->form_out, "device_id", "Device ID", self->hierarchy->device_id);

  if (!reqdata->form_passdata || reqdata->form_passdata_size < 2)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No threshold or section specified"); return 1; }

  section_name = (char *) reqdata->form_passdata;
  thold_name = strchr (section_name, ':');
  if (!thold_name)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Malformed threshold/section specification"); return 1; }
  *thold_name = '\0';
  thold_name++;

  section = i_threshold_section_get (self, section_name);
  if (!section)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified threshold section was not found"); return 1; }

  thold = i_threshold_get (self, section, thold_name);
  if (!thold)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified threshold was not found"); return 1; }

  i_form_hidden_add (reqdata->form_out, "section_name", section->name);
  i_form_hidden_add (reqdata->form_out, "thold_name", thold->name);
  i_form_string_add (reqdata->form_out, "section_desc", "Threshold Section", section->desc);
  i_form_string_add (reqdata->form_out, "object_desc", "Object Description", thold->object_desc);
  i_form_string_add (reqdata->form_out, "value_desc", "Value Description", thold->value_desc);

  i_form_spacer_add (reqdata->form_out);

  asprintf (&str, "%.2f", thold->alert_value);
  i_form_entry_add (reqdata->form_out, "alert_value", "Alert Value", str);
  free (str);
  asprintf (&str, "%.2f", thold->alert_default);
  i_form_string_add (reqdata->form_out, "alert_default", "Alert Value Default", str);
  free (str);

  asprintf (&str, "%.2f", thold->critical_value);
  i_form_entry_add (reqdata->form_out, "critical_value", "Critical Value", str);
  free (str);
  asprintf (&str, "%.2f", thold->critical_default);
  i_form_string_add (reqdata->form_out, "critical_default", "Critical Value Default", str);
  free (str);

  asprintf (&str, "%.2f", thold->tolerance_percent);
  i_form_entry_add (reqdata->form_out, "tolerance_percent", "Tolerance (%)", str);
  free (str);
  asprintf (&str, "%.2f", thold->tolerance_default);
  i_form_string_add (reqdata->form_out, "tolerance_default", "Tolerance Default (%)", str);
  free (str);

  i_form_string_add (reqdata->form_out, "space", " ", NULL);
  i_form_string_add (reqdata->form_out, "note", "Note", "A threshold of -1 results in the threshold not being actively monitored");

  i_form_set_submit (reqdata->form_out, 1);

  return 1;
}

int form_threshold_edit_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  char *title;
  char *section_name;
  char *thold_name;
  i_form_item_option *opt;
  i_threshold_section *section;
  i_threshold *thold;

  if (!self || !reqdata) return -1;
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_threshold_edit_submit failed to create form"); return -1; }
  asprintf (&title, "%s at %s Threshold", self->hierarchy->device_desc, self->hierarchy->site_desc);
  i_form_set_title (reqdata->form_out, title);
  free (title);

  i_form_string_add (reqdata->form_out, "site", "Site", self->hierarchy->site_desc);
  i_form_string_add (reqdata->form_out, "device", "Device", self->hierarchy->device_desc);
  i_form_string_add (reqdata->form_out, "device_id", "Device ID", self->hierarchy->device_id);

  opt = i_form_get_value_for_item (reqdata->form_in, "section_name");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to find section_name in form"); return 1; }
  section_name = (char *) opt->data;

  opt = i_form_get_value_for_item (reqdata->form_in, "thold_name");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to find thold_name in form"); return 1; }
  thold_name = (char *) opt->data;

  section = i_threshold_section_get (self, section_name);
  if (!section)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to find specified threshold section"); return 1; }
  i_form_string_add (reqdata->form_out, "section_desc", "Threshold Section", section->desc);

  thold = i_threshold_get (self, section, thold_name);
  if (!thold)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to find specified threshold"); return 1; }
  i_form_string_add (reqdata->form_out, "object_desc", "Object Description", thold->object_desc); 
  i_form_string_add (reqdata->form_out, "value_desc", "Value Description", thold->value_desc); 

  i_form_spacer_add (reqdata->form_out);

  opt = i_form_get_value_for_item (reqdata->form_in, "alert_value");
  if (opt) { thold->alert_value = atof ((char *)opt->data); }
  opt = i_form_get_value_for_item (reqdata->form_in, "alert_default");
  if (opt) { thold->alert_default = atof ((char *)opt->data); }
  
  opt = i_form_get_value_for_item (reqdata->form_in, "critical_value");
  if (opt) { thold->critical_value = atof ((char *)opt->data); }
  opt = i_form_get_value_for_item (reqdata->form_in, "critical_default");
  if (opt) { thold->critical_default = atof ((char *)opt->data); }
  
  opt = i_form_get_value_for_item (reqdata->form_in, "tolerance_percent");
  if (opt) { thold->tolerance_percent = atof ((char *)opt->data); }
  opt = i_form_get_value_for_item (reqdata->form_in, "tolerance_default");
  if (opt) { thold->tolerance_default = atof ((char *)opt->data); }

  num = i_threshold_sql_update (self, thold);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to update threshold in SQL database"); return 1; }

  i_form_string_add (reqdata->form_out, "msg", "Success", "Successfully updated threshold"); 
  
  return 1;
}

