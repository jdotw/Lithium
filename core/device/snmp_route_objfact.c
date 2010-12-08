#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/interface.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>

#include "snmp.h"
#include "snmp_route.h"

/* 
 * SNMP Routing Table - Object Factory Functions 
 */

/* Object Factory Fabrication */

int l_snmp_route_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  l_snmp_route_item *route;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = l_snmp_route_objform;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "l_snmp_route_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create interface struct */
  route = l_snmp_route_item_create ();
  if (!route)
  { i_printf (1, "l_snmp_route_objfact_fab failed to create route for object %s", obj->name_str); return -1; }
  route->obj = obj;
  obj->itemptr = route;

  /* 
   * Metric Creation 
   */

  /* Iface Index */
  route->ifindex = l_snmp_metric_create (self, obj, "ifindex", "Interface Index", METRIC_INTEGER, ".1.3.6.1.2.1.4.21.1.2", index_oidstr, RECMETHOD_NONE, 0);

  /* Metrics */
  route->metric1 = l_snmp_metric_create (self, obj, "metric1", "Primary Metric", METRIC_INTEGER, ".1.3.6.1.2.1.4.21.1.3", index_oidstr, RECMETHOD_NONE, 0);
  route->metric2 = l_snmp_metric_create (self, obj, "metric2", "Metric 2", METRIC_INTEGER, ".1.3.6.1.2.1.4.21.1.4", index_oidstr, RECMETHOD_NONE, 0);
  route->metric3 = l_snmp_metric_create (self, obj, "metric3", "Metric 3", METRIC_INTEGER, ".1.3.6.1.2.1.4.21.1.5", index_oidstr, RECMETHOD_NONE, 0);
  route->metric4 = l_snmp_metric_create (self, obj, "metric4", "Metric 4", METRIC_INTEGER, ".1.3.6.1.2.1.4.21.1.6", index_oidstr, RECMETHOD_NONE, 0);
  route->metric5 = l_snmp_metric_create (self, obj, "metric4", "Metric 4", METRIC_INTEGER, ".1.3.6.1.2.1.4.21.1.12", index_oidstr, RECMETHOD_NONE, 0);

  /* Addresses */
  route->mask = l_snmp_metric_create (self, obj, "mask", "Mask", METRIC_IP, ".1.3.6.1.2.1.4.21.1.11", index_oidstr, RECMETHOD_NONE, 0);
  route->nexthop = l_snmp_metric_create (self, obj, "nexthop", "Next Hop", METRIC_IP, ".1.3.6.1.2.1.4.21.1.7", index_oidstr, RECMETHOD_NONE, 0);

  /* Type */
  route->type = l_snmp_metric_create (self, obj, "type", "Type", METRIC_INTEGER, ".1.3.6.1.2.1.4.21.1.8", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (route->type, 1, "Other");
  i_metric_enumstr_add (route->type, 2, "Invalid");
  i_metric_enumstr_add (route->type, 3, "Direct");
  i_metric_enumstr_add (route->type, 4, "Indirect");

  /* Protocol */
  route->protocol = l_snmp_metric_create (self, obj, "protocol", "Protocol", METRIC_INTEGER, ".1.3.6.1.2.1.4.21.1.9", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (route->protocol, 1, "Other");
  i_metric_enumstr_add (route->protocol, 2, "Local");
  i_metric_enumstr_add (route->protocol, 3, "Network Manegement");
  i_metric_enumstr_add (route->protocol, 4, "ICMP");
  i_metric_enumstr_add (route->protocol, 5, "EGP");
  i_metric_enumstr_add (route->protocol, 6, "GGP");
  i_metric_enumstr_add (route->protocol, 7, "Hello");
  i_metric_enumstr_add (route->protocol, 8, "RIP");
  i_metric_enumstr_add (route->protocol, 9, "IS-IS");
  i_metric_enumstr_add (route->protocol, 10, "ES-IS");
  i_metric_enumstr_add (route->protocol, 11, "IGRP");
  i_metric_enumstr_add (route->protocol, 12, "SPF IGP");
  i_metric_enumstr_add (route->protocol, 13, "OSPF");
  i_metric_enumstr_add (route->protocol, 14, "BGP");

  /* Age */
  route->age = l_snmp_metric_create (self, obj, "age", "Age", METRIC_INTERVAL, ".1.3.6.1.2.1.4.21.1.10", index_oidstr, RECMETHOD_NONE, 0);
  
  /* Enqueue the route item */
  num = i_list_enqueue (cnt->item_list, route);
  if (num != 0)
  { i_printf (1, "l_snmp_route_objfact_fab failed to enqueue route for object %s", obj->name_str); l_snmp_route_item_free (route); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int l_snmp_route_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
{
  /* Check the result */
  if (result == SNMP_ERROR_NOERROR)
  {
    /* No errors, set item list state to NORMAL */
    cnt->item_list_state = ITEMLIST_STATE_NORMAL;
  }

  return 0;
}

/* Object Factory Clean Func
 *
 * Called when an object is obsolete prior to it being deregistered and free
 */

int l_snmp_route_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  l_snmp_route_item *route = obj->itemptr;

  if (!route) return 0;

  /* Remove addr from the route_list of a
   * matching snmp_iface interface 
   */
  if (route->iface && route->iface->route_list)
  {
    num = i_list_search (route->iface->route_list, route);
    if (num == 0)
    { i_list_delete (route->iface->route_list); }
  }

  /* Remove from item list */
  num = i_list_search (cnt->item_list, route);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;

  return 0;

}
