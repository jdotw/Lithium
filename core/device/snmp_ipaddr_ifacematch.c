#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/interface.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/ipregistry.h>

#include "snmp.h"
#include "snmp_iface.h"
#include "snmp_ipaddr.h"

/* snmp_ipaddr - SNMP IP Address Table Sub-System */

/* The l_snmp_ipaddr_ifacematch function is a refresh
 * callback that is triggered by the refresh of the
 * ipaddr container. It's purpose is to match l_snmp_ipaddr
 * items to interfaces in the l_snmp_iface item list
 */

int l_snmp_ipaddr_ifacematch (i_resource *self, i_container *addr_cnt, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the refcb alive */

  int num;
  i_container *iface_cnt;
  i_interface *iface;
  l_snmp_ipaddr *addr;

  /* Retrieve iface container */
  iface_cnt = l_snmp_iface_cnt ();

  /* Check containers/item_lists */
  if (!iface_cnt || !iface_cnt->item_list || !addr_cnt || !addr_cnt->item_list)
  { return 0; }

  /* Iterate through addr_cnt items */
  for (i_list_move_head(addr_cnt->item_list); (addr=i_list_restore(addr_cnt->item_list))!=NULL; i_list_move_next(addr_cnt->item_list))
  {
    /* Check to see if the addr is already matched */
    if (!addr->iface)
    {
      /* No match - Find one */
      i_metric_value *ifindexval;

      /* Retrieve current ifindex val for addr */
      ifindexval = i_metric_curval (addr->ifindex);
      if (!ifindexval) continue;
      
      /* Attempt to find match */
      for (i_list_move_head(iface_cnt->item_list); (iface=i_list_restore(iface_cnt->item_list))!=NULL; i_list_move_next(iface_cnt->item_list))
      { 
        if (iface->index == (unsigned long) ifindexval->integer) 
        {
          long msgid; 

          /* Match found */
          if (!iface->ip_list)
          { iface->ip_list = i_list_create (); }
          num = i_list_enqueue (iface->ip_list, addr); 
          if (num != 0)
          { 
            i_printf (2, "l_snmp_ipaddr_ifacematch failed to match ip address %s to interface index %s", addr->obj->name_str, iface->obj->name_str); 
            continue; 
          }
          addr->iface = iface;

          /* Set iface in IP registry */
          msgid = i_ipregistry_setiface (self, addr->obj, iface->obj);
          if (msgid == -1)
          { i_printf (2, "l_snmp_ipaddr_ifacematch failed to set interface in IP Registry for address %s interface %s", addr->obj->name_str, iface->obj->name_str); }
        }
      }      
    }
  }

  return 0;
}
