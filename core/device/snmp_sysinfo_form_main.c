#include <stdlib.h>

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

#include "snmp.h"
#include "snmp_sysinfo.h"

int form_snmp_sysinfo_main (i_resource *self, i_form_reqdata *reqdata)
{
  char *form_title;

  if (!self || !reqdata) return -1;
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }

  reqdata->form_out = i_form_create (0);
  if (!reqdata->form_out)
  { i_printf (1, "form_snmp_sysinfo_main failed to create form"); return -1; }
  asprintf (&form_title, "SNMP System Information for %s at %s", self->hierarchy->device_desc, self->hierarchy->site_desc);
  i_form_set_title (reqdata->form_out, form_title);
  free (form_title);

  i_form_string_add (reqdata->form_out, "site", "Site", self->hierarchy->site_desc);
  i_form_string_add (reqdata->form_out, "device", "Device", self->hierarchy->device_desc);
  i_form_string_add (reqdata->form_out, "device_id", "Device ID", self->hierarchy->device_id);
  i_form_spacer_add (reqdata->form_out);

  l_snmp_sysinfo_formsection (self, reqdata->form_out, reqdata->form_passdata, reqdata->form_passdata_size, reqdata->auth);

  return 1;
}

