#include <stdio.h>
#include <stdlib.h>

#include "induction.h"
#include "threshold.h"
#include "status.h"
#include "list.h"

int i_threshold_record (i_resource *self, i_threshold *thold, float value)
{
  /* Takes record of the value and sets any
   * status conditions where applicable 
   */

  char *status_str;
  char *status_desc;

  if (!self || !thold || !thold->section) return -1;

  thold->last_recorded = value;

  asprintf (&status_str, "%s_%s", thold->section->name, thold->name);

  if (value >= thold->alert_value || value >= thold->critical_value)
  {
    if (value >= thold->critical_value)
    {
      /* Critical Condition */
      if (thold->critical_value != -1)
      {
        asprintf (&status_desc, "%s Threshold Exceeded CRITICAL %s Level", thold->object_desc, thold->value_desc);
        i_status_set (self, STATUS_CRITICAL, status_str, status_desc);
        free (status_desc);
      }
    }

    if (value >= thold->alert_value && (value < thold->critical_value || thold->critical_value == -1))
    {
      /* Alert Condition */
      if (thold->alert_value != -1)
      {
        asprintf (&status_desc, "%s Threshold Exceeded %s Alert Level", thold->object_desc, thold->value_desc);
        i_status_set (self, STATUS_ALERT, status_str, status_desc);
        free (status_desc);
      }
    }
  }
  else
  {
    /* Below alert and critical */

    float alert_tolerance;
    float critical_tolerance;

    alert_tolerance = thold->alert_value - (thold->alert_value * (thold->tolerance_percent / 100));
    critical_tolerance = thold->critical_value - (thold->critical_value * (thold->tolerance_percent / 100));

    if ((value < alert_tolerance || thold->alert_value == -1) && (value < critical_tolerance || thold->critical_value == -1))
    { 
      /* Value is below both the alert tolerance 
       * and critical tolerance. clear status
       */
      i_status_clear (self, status_str);
    }
  }

  free (status_str);

  return 0;
}
