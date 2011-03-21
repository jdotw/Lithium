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
#include "snmp_route.h"

/* snmp_route - SNMP Routing Table Sub-System */

/* The l_snmp_route_ifacematch function is a refresh
 * callback that is triggered by the refresh of the
 * route container. It's purpose is to match l_snmp_route
 * items to interfaces in the l_snmp_iface item list
 */

int l_snmp_route_ifacematch (i_resource *self, i_container *route_cnt, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the refcb alive */

  int num;
  i_container *iface_cnt;
  i_interface *iface;
  l_snmp_route_item *route;

  /* Retrieve iface container */
  iface_cnt = l_snmp_iface_cnt ();

  /* Check containers/item_lists */
  if (!iface_cnt || !iface_cnt->item_list || !route_cnt || !route_cnt->item_list)
  { return 0; }

  /* Iterate through route_cnt items */
  for (i_list_move_head(route_cnt->item_list); (route=i_list_restore(route_cnt->item_list))!=NULL; i_list_move_next(route_cnt->item_list))
  {
    /* Check to see if the route is already matched */
    if (!route->iface)
    {
      /* No match - Find one */
      i_metric_value *ifindexval;

      /* Retrieve current ifindex val for route */
      ifindexval = i_metric_curval (route->ifindex);
      if (!ifindexval) continue;
      
      /* Attempt to find match */
      for (i_list_move_head(iface_cnt->item_list); (iface=i_list_restore(iface_cnt->item_list))!=NULL; i_list_move_next(iface_cnt->item_list))
      { 
        if (iface->index == (unsigned long) ifindexval->integer) 
        {
          /* Match found */
          if (!iface->route_list)
          { iface->route_list = i_list_create (); }
          num = i_list_enqueue (iface->route_list, route); 
          if (num != 0)
          { 
            i_printf (1, "l_snmp_route_ifacematch failed to match ip route %s to interface index %s", route->obj->name_str, iface->obj->name_str); 
            continue; 
          }
          route->iface = iface;
        }
        /* End of iface loop */
      }      
    }

    /* End of route loop */
  }

  return 0;
}
