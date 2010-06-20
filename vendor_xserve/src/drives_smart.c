#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>
#include <induction/name.h>
#include <induction/str.h>
#include <lithium/snmp.h>
#include <lithium/record.h>

#include "osx_server.h"
#include "data.h"
#include "drives.h"

/* Xserve Drives Info */

/* SMART Status Handling
 *
 * The Xserve will report its status as "Verified" or "No Warnings".
 * This refresh callback converts that into an integer result for 
 * the "smart" metric
 */

int v_drives_smart_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  i_metric *raw = (i_metric *) ent;
  i_metric *status = (i_metric *) passdata;

  char *value_str = i_metric_valstr (raw, NULL);
  if (value_str)
  {
    i_metric_value *val = i_metric_value_create ();

    if (strcmp(value_str, "No Warnings") == 0)
    {
      /* No Warnings */
      val->integer = 1;
    }
    else if (strcmp(value_str, "Verified") == 0)
    { 
      /* Verified */
      val->integer = 1;
    }
    else if (strcmp(value_str, "Not Available") == 0)
    { 
      /* Verified */
      val->integer = 3;
    }
    else if (strcmp(value_str, "Unsupported") == 0)
    { 
      /* Verified */
      val->integer = 3;
    }
    else
    { 
      /* Else, warning */
      val->integer = 2;
    }

    i_metric_value_enqueue (self, status, val);
    status->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(status));
  }

  return 0;
}
