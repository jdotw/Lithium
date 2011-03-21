#include <stdlib.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/timeutil.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/str.h"

#include "device/snmp.h"
#include "device/record.h"

#include "main.h"
#include "client.h"

/* 
 * Wirless Clients - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_client_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_client_item *client;

  /* Object Configuration */
  if (v_nseries_extras())
  { obj->desc_str = l_snmp_get_string_from_pdu (pdu); }
  else
  { obj->desc_str = l_snmp_var_to_hexstr (pdu->variables); }
  obj->mainform_func = v_client_objform;
  obj->histform_func = v_client_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_client_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create client item struct */
  client = v_client_item_create ();
  if (!client)
  { i_printf (1, "v_client_objfact_fab failed to create client item for object %s", obj->name_str); return -1; }
  client->obj = obj;
  obj->itemptr = client;

  /* 
   * Metric Creation 
   */

  /* Type */
  client->type = l_snmp_metric_create (self, obj, "type", "Type", METRIC_GAUGE, "enterprises.63.501.3.2.2.1.2", index_oidstr, RECMETHOD_NONE, 0);

  /* Time Associated */
  client->time_assoc = l_snmp_metric_create (self, obj, "time_assoc", "Time Associated", METRIC_INTERVAL, "enterprises.63.501.3.2.2.1.4", index_oidstr, RECMETHOD_NONE, 0);
  
  /* Strength */
  client->strength = l_snmp_metric_create (self, obj, "strength", "Signal Strength", METRIC_INTEGER, "enterprises.63.501.3.2.2.1.6", index_oidstr, RECMETHOD_NONE, 0);
  client->strength->unit_str = strdup ("dB");

  /* Noise */
  client->noise = l_snmp_metric_create (self, obj, "noise", "Noise Level", METRIC_INTEGER, "enterprises.63.501.3.2.2.1.7", index_oidstr, RECMETHOD_NONE, 0);
  client->noise->unit_str = strdup ("dB");

  /* Rate */
  client->rate = l_snmp_metric_create (self, obj, "rate", "Data Rate", METRIC_GAUGE, "enterprises.63.501.3.2.2.1.8", index_oidstr, RECMETHOD_NONE, 0);
  client->rate->unit_str = strdup ("Mbps");

  /* TxCount */
  client->tx_count = l_snmp_metric_create (self, obj, "tx_count", "Transmit Count", METRIC_COUNT, "enterprises.63.501.3.2.2.1.9", index_oidstr, RECMETHOD_NONE, 0);
  client->tx_rate = i_metric_acrate_create (self, obj, "tx_rate", "Transmit Rate", "pps", RECMETHOD_RRD, client->tx_count, 0);

  /* RxCount */
  client->rx_count = l_snmp_metric_create (self, obj, "rx_count", "Receive Count", METRIC_COUNT, "enterprises.63.501.3.2.2.1.10", index_oidstr, RECMETHOD_NONE, 0);
  client->rx_rate = i_metric_acrate_create (self, obj, "rx_rate", "Receive Rate", "pps", RECMETHOD_RRD, client->rx_count, 0);

  /* Tx Errors */
  client->tx_errors = l_snmp_metric_create (self, obj, "tx_errors", "Transmit Error Count", METRIC_COUNT, "enterprises.63.501.3.2.2.1.11", index_oidstr, RECMETHOD_NONE, 0);
  client->tx_eps = i_metric_acrate_create (self, obj, "tx_eps", "Transmit Error Rate", "eps", RECMETHOD_RRD, client->tx_errors, 0);

  /* TxCount */
  client->rx_errors = l_snmp_metric_create (self, obj, "rx_errors", "Receive Error Count", METRIC_COUNT, "enterprises.63.501.3.2.2.1.12", index_oidstr, RECMETHOD_NONE, 0);
  client->rx_eps = i_metric_acrate_create (self, obj, "rx_eps", "Receive Error Rate", "eps", RECMETHOD_RRD, client->rx_errors, 0);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);
  
  /* Enqueue the client item */
  num = i_list_enqueue (cnt->item_list, client);
  if (num != 0)
  { i_printf (1, "v_client_objfact_fab failed to enqueue client for object %s", obj->name_str); v_client_item_free (client); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_client_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_client_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the client and remove
   * it from the item_list
   */

  int num;
  v_client_item *client = obj->itemptr;

  if (!client) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, client);
  if (num == 0)
  { i_list_delete (cnt->item_list); }

  /* Free the client */
  v_client_item_free (client);
  obj->itemptr = NULL;
  
  return 0;
}
