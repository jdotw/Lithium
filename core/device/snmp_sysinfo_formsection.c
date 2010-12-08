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
#include <induction/str.h>
#include <induction/timeutil.h>

#include "snmp.h"
#include "snmp_sysinfo.h"

int l_snmp_sysinfo_formsection (i_resource *self, i_form *form, void *passdata, int passdatasize, i_authentication *auth)
{
  /* Return 0 to keep the formsection active */

  l_snmp_sysinfo *sysinfo;

  sysinfo = l_snmp_sysinfo_get ();

  if (sysinfo)
  {
    char *str;

    if (sysinfo->descr_str && sysinfo->descr_current != -1)
    {
      if (sysinfo->descr_current == 1)
      { i_form_string_add (form, "sysdescr", "System Description", sysinfo->descr_str); }
      else
      {
        asprintf (&str, "%s *", sysinfo->descr_str);
        i_form_string_add (form, "sysdescr", "System Description", str);
        free (str);
      }
    }

    if (sysinfo->name_str && sysinfo->name_current != -1)
    {
      if (sysinfo->name_current == 1)
      { i_form_string_add (form, "sysname", "System Name", sysinfo->name_str); }
      else
      {
        asprintf (&str, "%s *", sysinfo->name_str);
        i_form_string_add (form, "sysname", "System Name", str);
        free (str);
      }
    }

    if (sysinfo->location_str && sysinfo->location_current != -1)
    {
      if (sysinfo->location_current == 1)
      { i_form_string_add (form, "syslocation", "System Location", sysinfo->location_str); }
      else
      {
        asprintf (&str, "%s *", sysinfo->location_str);
        i_form_string_add (form, "syslocation", "System Location", str);
        free (str);
      }
    }

    if (sysinfo->contact_str && sysinfo->contact_current != -1)
    {
      if (sysinfo->contact_current == 1)
      { i_form_string_add (form, "syscontact", "System Contact", sysinfo->contact_str); }
      else
      {
        asprintf (&str, "%s *", sysinfo->contact_str);
        i_form_string_add (form, "syscontact", "System Contact", str);
        free (str);
      }
    }

    if (sysinfo->uptime_sec > 0 && sysinfo->uptime_sec_current != -1)
    {
      char *uptime_str;

      uptime_str = i_time_interval_string (sysinfo->uptime_sec);
      if (uptime_str)
      {
        if (sysinfo->uptime_sec_current == 1)
        { i_form_string_add (form, "sysuptime", "System Up-Time", uptime_str); }
        else
        {
          asprintf (&str, "%s *", uptime_str);
          i_form_string_add (form, "sysuptime", "System Up-Time", str);
          free (str);
        }
        free (uptime_str);
      }
    }
      
    i_form_string_add (form, "sysinfo_current_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    i_form_string_add (form, "msg", "Processors", "The Processor Resource list has not yet been populated");
  }

  return 0;
}
