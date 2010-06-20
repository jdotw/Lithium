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
 * SNMP IP Addresses - Object Factory Functions 
 */

/* Object Factory Fabrication */

int l_snmp_ipaddr_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  long msgid;
  l_snmp_ipaddr *ipaddr;

  /* Check for pdu */
  if (!pdu || !pdu->variables || pdu->variables->val_len < 4 || !pdu->variables->val.string || !obj)
  { 
    i_printf (1, "l_snmp_ipaddr_objfact_fab ignoring invalid PDU");
    return -1;
  }

  /* Object Configuration */
  asprintf (&obj->desc_str, "%d.%d.%d.%d",
    (u_char) pdu->variables->val.string[0], (u_char) pdu->variables->val.string[1],
    (u_char) pdu->variables->val.string[2], (u_char) pdu->variables->val.string[3]);
  obj->mainform_func = l_snmp_ipaddr_objform;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "l_snmp_ipaddr_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create interface struct */
  ipaddr = l_snmp_ipaddr_create ();
  if (!ipaddr)
  { i_printf (1, "l_snmp_ipaddr_objfact_fab failed to create ipaddr item for object %s", obj->name_str); return -1; }
  ipaddr->obj = obj;
  obj->itemptr = ipaddr;

  /* 
   * Metric Creation 
   */

  ipaddr->addr = l_snmp_metric_create (self, obj, "addr", "IP Address", METRIC_IP, "ipAdEntAddr", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  ipaddr->ifindex = l_snmp_metric_create (self, obj, "ifindex", "Interface Index", METRIC_INTEGER, "ipAdEntIfIndex", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  ipaddr->netmask = l_snmp_metric_create (self, obj, "netmask", "Subnet Mask", METRIC_IP, "ipAdEntNetMask", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  ipaddr->netmask_refcb = i_entity_refreshcb_add (ENTITY(ipaddr->netmask), l_snmp_ipaddr_netmask_refcb, NULL);
  
  /*
   * End Metric Creation
   */

  /* Enqueue the ipaddr item */
  num = i_list_enqueue (cnt->item_list, ipaddr);
  if (num != 0)
  { i_printf (1, "l_snmp_ipaddr_objfact_fab failed to enqueue ipaddr for object %s", obj->name_str); l_snmp_ipaddr_free (ipaddr); return -1; }

  /*
   * Register IP Address 
   */

  msgid = i_ipregistry_register (self, obj);
  if (msgid == -1)
  { i_printf (2, "l_snmp_ipaddr_objfact_fab warning, failed to register ipaddr object %s", obj->name_str); }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int l_snmp_ipaddr_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int l_snmp_ipaddr_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  long msgid;
  l_snmp_ipaddr *addr = obj->itemptr;

  if (!addr) return 0;

  /*
   * De-register the IP 
   */

  msgid = i_ipregistry_deregister (self, self->hierarchy->cust_addr, self->hierarchy->dev_addr, obj);
  if (msgid == -1)
  { i_printf (2, "l_snmp_ipaddr_objfact_clean warning, failed to deregister ipaddr object %s", obj->name_str); }

  /* Remove addr from the ip_list of a
   * matching snmp_iface interface 
   */
  if (addr->iface && addr->iface->ip_list)
  {
    num = i_list_search (addr->iface->ip_list, addr);
    if (num == 0)
    { i_list_delete (addr->iface->ip_list); }
  }

  /* Remove from item list */
  num = i_list_search (cnt->item_list, addr);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;

  return 0;
}
