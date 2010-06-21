#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/device.h>
#include <induction/threshold.h>
#include <induction/path.h>
#include <induction/hierarchy.h>
#include <induction/navform.h>
#include <induction/mainform.h>
#include <induction/postgresql.h>
#include <induction/formdb.h>

#include "snmp.h"
#include "snmp_sysinfo.h"

#define DEFAULT_REFRESH_SECONDS 3600          /* 1 hour standard refresh */
#define DEFAULT_UPTIME_REFRESH_SECONDS 60     /* 1 minute uptime refresh */

/* Struct Manipulation */

l_snmp_sysinfo_config* l_snmp_sysinfo_config_create ()
{
  l_snmp_sysinfo_config *config;

  config = (l_snmp_sysinfo_config *) malloc (sizeof(l_snmp_sysinfo_config));
  if (!config)
  { i_printf (1, "l_snmp_sysinfo_config_create failed to malloc config struct"); return NULL; }
  memset (config, 0, sizeof(l_snmp_sysinfo_config));

  return config;
}

void l_snmp_sysinfo_config_free (void *configptr)
{
  l_snmp_sysinfo_config *config = configptr;

  if (!config) return;

  free (config);
}

/* Config Load */

l_snmp_sysinfo_config* l_snmp_sysinfo_config_load (i_resource *self)
{
  i_form *form;
  i_form_item_option *opt;
  l_snmp_sysinfo_config *config;

  config = l_snmp_sysinfo_config_create ();
  if (!config)
  { i_printf (1, "l_snmp_sysinfo_config_load failed to create config struct"); return NULL; }

  form = i_formdb_get (self, "snmp_sysinfo_config");

  /* At this stage there may or may not be a form
   * The i_form_get_value_for_item func is OK
   * with a NULL form
   */

  opt = i_form_get_value_for_item (form, "refresh_seconds");
  if (opt) { config->refresh_seconds = atol ((char *)opt->data); }
  else { config->refresh_seconds = DEFAULT_REFRESH_SECONDS; } 
  
  opt = i_form_get_value_for_item (form, "uptime_refresh_seconds");
  if (opt) { config->uptime_refresh_seconds = atol ((char *)opt->data); }
  else { config->uptime_refresh_seconds = DEFAULT_UPTIME_REFRESH_SECONDS; } 

  if (form) i_form_free (form);

  return config;
}

/* Forms */

int form_snmp_sysinfo_config (i_resource *self, i_form_reqdata *reqdata)
{
  char *str;
  l_snmp_sysinfo_config *config;

  if (!self || !reqdata) return -1;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }

  reqdata->form_out = i_form_create (1);
  if (!reqdata->form_out)
  { i_printf (1, "form_snmp_sysinfo_config failed to create form"); return -1; }
  i_form_set_title (reqdata->form_out, "SNMP System Information Configuration"); 

  if (self->hierarchy)
  {
    i_form_string_add (reqdata->form_out, "device_name", "Device Name", self->hierarchy->device_desc);
    i_form_string_add (reqdata->form_out, "site_name", "Site", self->hierarchy->site_desc);
    i_form_spacer_add (reqdata->form_out);
  }

  config = l_snmp_sysinfo_config_load (self);
  if (!config)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to load configuration for SNMP System Information"); return 1; }

  asprintf (&str, "%li", config->uptime_refresh_seconds);
  i_form_entry_add (reqdata->form_out, "uptime_refresh_seconds", "Uptime Refresh Interval (Seconds)", str);
  free (str);

  asprintf (&str, "%li", config->refresh_seconds);
  i_form_entry_add (reqdata->form_out, "refresh_seconds", "Standard Refresh Interval (Seconds)", str);
  free (str);

  l_snmp_sysinfo_config_free (config);

  return 1;
}

int form_snmp_sysinfo_config_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;

  /* Create the return form */

  reqdata->form_out = i_form_create (0);
  if (!reqdata->form_out)
  { i_printf (1, "form_snmp_sysinfo_config_submit failed to create form_out"); return -1; }
  i_form_set_title (reqdata->form_out, "SNMP System Information Configuration"); 

  /* Remove the old, process the new */

  i_formdb_del (self, "snmp_sysinfo_config");

  num = i_formdb_put (self, "snmp_sysinfo_config", reqdata->form_in);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to put updated configuration form into form database"); return 1; }

  i_form_string_add (reqdata->form_out, "msg", "Success", "Successfully stored updated configuration");

  /* Call the refresh-update function */

  l_snmp_sysinfo_refresh_configupdated (self);

  /* Finished */

  return 1;
}


