#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/cement.h>
#include <induction/auth.h>
#include <induction/interface.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/hierarchy.h>
#include <induction/str.h>

#include "snmp.h"
#include "icmp.h"

/* 
 * ICMP - Object Factory Functions 
 */

/* Object Factory Fabrication */

int l_icmp_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  l_icmp_item *icmp;
  i_object *masterobj;

  /* Object Configuration */
  asprintf (&obj->desc_str, "%d.%d.%d.%d",
    (u_char) pdu->variables->val.string[0], (u_char) pdu->variables->val.string[1],
    (u_char) pdu->variables->val.string[2], (u_char) pdu->variables->val.string[3]);
  obj->mainform_func = l_icmp_objform;

  /* Address check */
  masterobj = l_icmp_masterobj ();
  if (!strcmp(obj->desc_str, "0.0.0.0") || !strcmp(obj->desc_str, "127.0.0.1") || !strcmp(obj->desc_str, masterobj->desc_str))
  { return -1; }

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "l_icmp_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create interface struct */
  icmp = l_icmp_item_create ();
  if (!icmp)
  { i_printf (1, "l_icmp_objfact_fab failed to create ICMP item for object %s", obj->name_str); return -1; }
  icmp->obj = obj;
  obj->itemptr = icmp;

  /* Create metrics */
  l_icmp_objfact_createmets (self, obj, icmp);

  /* Enqueue the icmp item */
  num = i_list_enqueue (cnt->item_list, icmp);
  if (num != 0)
  { i_printf (1, "l_icmp_objfact_fab failed to enqueue ICMP item for object %s", obj->name_str); l_icmp_item_free (icmp); return -1; }

  return 0;
}

int l_icmp_objfact_createmets (i_resource *self, i_object *obj, l_icmp_item *icmp)
{
  /* Metric Creation utility */
  static i_entity_refresh_config refconfig;

  /* Configure default refresh options */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* Reachable */
  icmp->reachable = i_metric_create ("reachable", "Reachable", METRIC_INTEGER);
  i_metric_enumstr_add (icmp->reachable, 0, "No");
  i_metric_enumstr_add (icmp->reachable, 1, "Yes");
  icmp->reachable->refresh_func = l_icmp_refresh;
  i_entity_register (self, ENTITY(obj), ENTITY(icmp->reachable));
  i_entity_refresh_config_loadapply (self, ENTITY(icmp->reachable), &refconfig);

  /* Response */
  icmp->response = i_metric_create ("response", "Response", METRIC_INTEGER);
  icmp->response->refresh_func = l_icmp_refresh;
  i_metric_enumstr_add (icmp->response, 100, "Echo Reply");
  i_metric_enumstr_add (icmp->response, 101, "Failed");
  i_metric_enumstr_add (icmp->response, 102, "Timeout");
  i_metric_enumstr_add (icmp->response, 103, "Source Quench");
  i_metric_enumstr_add (icmp->response, 104, "Redirected");
  i_metric_enumstr_add (icmp->response, 105, "Time-to-live Exceeded");
  i_metric_enumstr_add (icmp->response, 106, "Parameter Problem");
  i_metric_enumstr_add (icmp->response, 0, "Network Unreachable");
  i_metric_enumstr_add (icmp->response, 1, "Host Unreachable");
  i_metric_enumstr_add (icmp->response, 2, "Protocol Unreachable");
  i_metric_enumstr_add (icmp->response, 3, "Port Unreachable");
  i_metric_enumstr_add (icmp->response, 4, "Cannot Fragment");
  i_metric_enumstr_add (icmp->response, 5, "Source Route Failed");
  i_metric_enumstr_add (icmp->response, 6, "Network Unknown");
  i_metric_enumstr_add (icmp->response, 7, "Host Unknown");
  i_metric_enumstr_add (icmp->response, 8, "Isolated");
  i_metric_enumstr_add (icmp->response, 9, "Network Prohibited");
  i_metric_enumstr_add (icmp->response, 10, "Host Prohibited");
  i_metric_enumstr_add (icmp->response, 11, "Network TOS Prohibited");
  i_metric_enumstr_add (icmp->response, 12, "Host TOS Prohibited");
  i_metric_enumstr_add (icmp->response, 13, "Administratively Prohibited");
  i_metric_enumstr_add (icmp->response, 14, "Host Precedence Violation");
  i_metric_enumstr_add (icmp->response, 15, "Host Precedence Cut Off");
  i_entity_register (self, ENTITY(obj), ENTITY(icmp->response));
  i_entity_refresh_config_loadapply (self, ENTITY(icmp->response), &refconfig);

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int l_icmp_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int l_icmp_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  l_icmp_item *icmp = obj->itemptr;

  /* Remove from item list */
  num = i_list_search (cnt->item_list, icmp);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;

  return 0;
}
