#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/interface.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/ipregistry.h>
#include <induction/str.h>

#include "snmp.h"
#include "snmp_iface.h"
#include "snmp_ipaddr.h"

/* 
 * SNMP IP Addresses - Netmask Functions 
 */

int l_snmp_ipaddr_netmask_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when the netmask metric of an snmp_ipaddr is refreshed
   *
   * Always return 0 to keep callback alive
   */

  long msgid;
  int set_flag = 0;
  char *curvalstr;
  char *prevalstr;
  i_metric *netmask = (i_metric *) ent;
  i_metric_value *curval;
  i_metric_value *preval;
  l_snmp_ipaddr *ipaddr = netmask->obj->itemptr;

  if (ipaddr->netmask_set == 1)
  {
    /* Netmask has already been set in the ipregistry,
     * check for a change in the netmask
     */

    /* Retrieve values */
    i_list_move_head (netmask->val_list);
    curval = i_list_restore (netmask->val_list);
    i_list_move_next (netmask->val_list);
    preval = i_list_restore (netmask->val_list);
    if (!curval || !preval) return 0;

    /* Get value strings */
    curvalstr = i_metric_valstr (netmask, curval);
    prevalstr = i_metric_valstr (netmask, preval);
    if (!curvalstr || !prevalstr) 
    { 
      if (curvalstr) free (curvalstr);
      if (prevalstr) free (prevalstr);
      return 0;
    }

    if (strcmp(curvalstr, prevalstr) != 0)
    { 
      /* Change detected */
      set_flag = 1;
    }

    if (curvalstr) free (curvalstr);
    if (prevalstr) free (prevalstr);
  }
  else
  {
    /* Netmask has not yet been set in the registry,
     * make sure a current value is present, then
     * set the set_flag accordingly
     */
    curvalstr = i_metric_valstr (netmask, NULL);
    if (curvalstr)
    {
      set_flag = 1;
      free (curvalstr);
    }
  }

  /* If the set_flag = 1, set the netmask
   * within the IP registry
   */
  if (set_flag == 1)
  {
    msgid = i_ipregistry_setnetmask (self, netmask->obj, netmask);
    if (msgid == -1)
    { i_printf (2, "l_snmp_ipaddr_netmask_refcb failed to set netmask within ipregistry for ipaddr %s", netmask->obj->desc_str); }
    ipaddr->netmask_set = 1;
  }

  return 0;
}
