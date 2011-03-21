#include <stdlib.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/formdb.h>
#include <induction/hierarchy.h>

#include "snmp.h"

#define DEFAULT_TIMEOUT_USECONDS 1000000L
#define DEFAULT_RETRIES 5

/* Struct Manipulation */

l_snmp_config* l_snmp_config_create ()
{
  l_snmp_config *config;

  config = (l_snmp_config *) malloc (sizeof(l_snmp_config));
  if (!config)
  { i_printf (1, "l_snmp_config_create failed to malloc config struct"); return NULL; }
  memset (config, 0, sizeof(l_snmp_config));

  return config;
}

void l_snmp_config_free (void *configptr)
{
  l_snmp_config *config = configptr;

  if (!config) return;

  free (config);
}

/* Config Load */

l_snmp_config* l_snmp_config_load (i_resource *self)
{
  i_form *form;
  i_form_item_option *opt;
  l_snmp_config *config;

  config = l_snmp_config_create ();
  if (!config)
  { i_printf (1, "l_snmp_config_load failed to create config struct"); return NULL; }

  /* At this stage there may or may not be a form
   * The i_form_get_value_for_item func is OK
   * with a NULL form
   */

  form = i_formdb_get (self, "snmp_config");

  opt = i_form_get_value_for_item (form, "timeout_useconds");
  if (opt) { config->timeout_useconds = atol ((char *)opt->data); }
  else { config->timeout_useconds = DEFAULT_TIMEOUT_USECONDS; } 

  opt = i_form_get_value_for_item (form, "retires");
  if (opt) { config->retries = atoi ((char *)opt->data); }
  else { config->retries = DEFAULT_RETRIES; } 

  if (form) i_form_free (form);

  return config;
}

/* Forms */

int form_snmp_config (i_resource *self, i_form_reqdata *reqdata)
{
  char *str;
  l_snmp_config *config;

  if (!self || !reqdata) return -1;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_snmp_config failed to create form"); return -1; }
  i_form_set_title (reqdata->form_out, "SNMP Configuration"); 

  if (self->hierarchy)
  {
    i_form_string_add (reqdata->form_out, "device_name", "Device Name", self->hierarchy->device_desc);
    i_form_string_add (reqdata->form_out, "site_name", "Site", self->hierarchy->site_desc);
    i_form_spacer_add (reqdata->form_out);
  }

  config = l_snmp_config_load (self);
  if (!config)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to load SNMP configuration"); return 1; }

  asprintf (&str, "%i", config->timeout_useconds);
  i_form_entry_add (reqdata->form_out, "timeout_useconds", "Initial Timeout Before Retry (uSeconds)", str);
  free (str);

  asprintf (&str, "%i", config->retries);
  i_form_entry_add (reqdata->form_out, "retries", "Maximum Retries", str);
  free (str);

  
  l_snmp_config_free (config);

  return 1;
}

int form_snmp_config_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;

  /* Create the return form */

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_snmp_config_submit failed to create form_out"); return -1; }

  /* Remove the old, store the new */

  i_formdb_del (self, "snmp_config");

  num = i_formdb_put (self, "snmp_config", reqdata->form_in);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to put updated configuration form into form database"); return 1; }

  i_form_string_add (reqdata->form_out, "msg", "Success", "Successfully stored updated configuration");

  /* Finished */

  return 1;
}

