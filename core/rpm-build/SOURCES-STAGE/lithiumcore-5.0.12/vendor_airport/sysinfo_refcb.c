#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/navtree.h"
#include "induction/navform.h"
#include "induction/hierarchy.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "device/record.h"
#include "device/snmp.h"
#include "device/snmp_sysinfo.h"
#include "device/snmp_iface.h"
#include "device/snmp_ipaddr.h"

#include "main.h"
#include "client.h"
#include "clientcount.h"
#include "physical.h"
#include "sysinfo_refcb.h"

int v_sysinfo_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when the SNMP system information container has been refrshed 
   *
   * Return 0 to keep callback alive or -1 to kill 
   */

  int num;
  i_object *sysinfo_obj = (i_object *) ent;
  l_snmp_sysinfo_item *item = (l_snmp_sysinfo_item *) sysinfo_obj->itemptr;

  char *valstr = i_metric_valstr (item->descr, NULL);
  if (valstr)
  {
    if (strstr(valstr, "Apple AirPort"))
    {
      i_printf (1, "v_sysinfo_refcb enabling 802.11n extras (%s)", valstr);
      /* This is an 802.11n unit */
      v_nseries_extras_set (1);
      num = v_client_enable (self);
      if (num != 0)
      { i_printf (1, "v_sysinfo_refcb failed to enable Wireless Client sub-system"); }

      num = v_clientcount_enable (self);
      if (num != 0)
      { i_printf (1, "v_sysinfo_refcb failed to enable Wireless Client Count sub-system"); }

      num = l_snmp_iface_enable (self);
      if (num != 0)
      { i_printf (1, "v_sysinfo_refcb failed to enable snmp network interface sub-system"); }

      num = l_snmp_ipaddr_enable (self);
      if (num != 0)
      { i_printf (1, "v_sysinfo_refcb failed to enable SNMP Interface List sub-system"); }
    }
    else
    {
      i_printf (1, "v_sysinfo_refcb disabling 802.11n extras (%s)", valstr);
      /* Pre-802.11n Unit */
      v_nseries_extras_set (0);

      num = v_client_enable (self);
      if (num != 0)
      { i_printf (1, "v_sysinfo_refcb failed to enable Wireless Client sub-system"); }

      num = v_clientcount_enable (self);
      if (num != 0)
      { i_printf (1, "v_sysinfo_refcb failed to enable Wireless Client Count sub-system"); }

      num = v_physical_enable (self);
      if (num != 0)
      { i_printf (1, "v_sysinfo_refcb failed to enable physical interface sub-system"); }

      num = l_snmp_ipaddr_enable (self);
      if (num != 0)
      { i_printf (1, "v_sysinfo_refcb failed to enable SNMP Interface List sub-system"); }
    }

    /* Remove callback */
    return -1;
  }
  else
  {
    /* No value yet, keep callback in */
    return 0;
  }
}
