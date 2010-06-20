#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>

#include <lithium/snmp.h>

#include "tcpconn.h"

/* 
 * TCP Connections - Object Factory Functions 
 */

/* Object Factory Fabrication */

int l_snmp_nstcpconn_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  l_snmp_nstcpconn_item *tcpconn;

  /* Object Configuration */
  if ((int) *pdu->variables->val.integer == 2)
  {
    /* Listener */
    asprintf (&obj->desc_str, "LISTENER: %i.%i.%i.%i:%i", 
      (int) pdu->variables->name[pdu->variables->name_length-10],
      (int) pdu->variables->name[pdu->variables->name_length-9],
      (int) pdu->variables->name[pdu->variables->name_length-8],
      (int) pdu->variables->name[pdu->variables->name_length-7],
      (int) pdu->variables->name[pdu->variables->name_length-6]);
  }
  else
  {
    /* Session */
    asprintf (&obj->desc_str, "SESSION: %i.%i.%i.%i:%i <-> %i.%i.%i.%i:%i", 
      (int) pdu->variables->name[pdu->variables->name_length-10],
      (int) pdu->variables->name[pdu->variables->name_length-9],
      (int) pdu->variables->name[pdu->variables->name_length-8],
      (int) pdu->variables->name[pdu->variables->name_length-7],
      (int) pdu->variables->name[pdu->variables->name_length-6],
      (int) pdu->variables->name[pdu->variables->name_length-5],
      (int) pdu->variables->name[pdu->variables->name_length-4],
      (int) pdu->variables->name[pdu->variables->name_length-3],
      (int) pdu->variables->name[pdu->variables->name_length-2],
      (int) pdu->variables->name[pdu->variables->name_length-1]);
  }
  //obj->mainform_func = l_snmp_nstcpconn_objform;
  //obj->histform_func = l_snmp_nstcpconn_objform_hist;
  
  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "l_snmp_nstcpconn_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create tcpconn item struct */
  tcpconn = l_snmp_nstcpconn_item_create ();
  if (!tcpconn)
  { i_printf (1, "l_snmp_nstcpconn_objfact_fab failed to create tcpconn item for object %s", obj->name_str); return -1; }
  tcpconn->obj = obj;
  obj->itemptr = tcpconn;

  /* 
   * Metric Creation 
   */

  tcpconn->state = l_snmp_metric_create (self, obj, "state", "State", METRIC_INTEGER, ".1.3.6.1.2.1.6.13.1.1", index_oidstr, RECMETHOD_NONE, 0);
  
  tcpconn->local_ip = l_snmp_metric_create (self, obj, "local_ip", "Local IP", METRIC_IP, ".1.3.6.1.2.1.6.13.1.2", index_oidstr, RECMETHOD_NONE, 0);
  tcpconn->local_port = l_snmp_metric_create (self, obj, "local_port", "Local Port", METRIC_INTEGER, ".1.3.6.1.2.1.6.13.1.3", index_oidstr, RECMETHOD_NONE, 0);

  tcpconn->remote_ip = l_snmp_metric_create (self, obj, "remote_ip", "Remote IP", METRIC_IP, ".1.3.6.1.2.1.6.13.1.4", index_oidstr, RECMETHOD_NONE, 0);
  tcpconn->remote_port = l_snmp_metric_create (self, obj, "remote_port", "Remote Port", METRIC_INTEGER, ".1.3.6.1.2.1.6.13.1.5", index_oidstr, RECMETHOD_NONE, 0);

  /* Enqueue the tcpconn item */
  num = i_list_enqueue (cnt->item_list, tcpconn);
  if (num != 0)
  { i_printf (1, "l_snmp_nstcpconn_objfact_fab failed to enqueue tcpconn for object %s", obj->name_str); l_snmp_nstcpconn_item_free (tcpconn); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int l_snmp_nstcpconn_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int l_snmp_nstcpconn_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the tcpconn and remove
   * it from the item_list
   */

  int num;
  l_snmp_nstcpconn_item *tcpconn = obj->itemptr;

  if (!tcpconn) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, tcpconn);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
