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
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/str.h>

#include <lithium/snmp.h>
#include <lithium/record.h>

#include "physical.h"

/* 
 * Wirless Clients - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_physical_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_physical_item *physical;

  /* Object Configuration */
  asprintf (&obj->desc_str, "Interface %s (%s)", index_oidstr, l_snmp_get_string_from_pdu (pdu));
  obj->mainform_func = v_physical_objform;
  obj->histform_func = v_physical_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_physical_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create physical item struct */
  physical = v_physical_item_create ();
  if (!physical)
  { i_printf (1, "v_physical_objfact_fab failed to create physical item for object %s", obj->name_str); return -1; }
  physical->obj = obj;
  obj->itemptr = physical;

  /* 
   * Metric Creation 
   */

  /* Desc */
  physical->desc = l_snmp_metric_create (self, obj, "desc", "Description", METRIC_STRING, "enterprises.63.501.3.4.2.1.2", index_oidstr, RECMETHOD_NONE, 0);

  /* Speed */
  physical->speed = l_snmp_metric_create (self, obj, "speed", "Speed", METRIC_GAUGE, "enterprises.63.501.3.4.2.1.4", index_oidstr, RECMETHOD_NONE, 0);
  physical->speed->unit_str = strdup ("bit");
  physical->speed->valstr_func = i_string_volume_metric;
  
  /* State */
  physical->state = l_snmp_metric_create (self, obj, "state", "State", METRIC_INTEGER, "enterprises.63.501.3.4.2.1.5", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (physical->state, 0, "Down");
  i_metric_enumstr_add (physical->state, 1, "Up");

  /* Duplex */
  physical->duplex = l_snmp_metric_create (self, obj, "duplex", "Duplex", METRIC_INTEGER, "enterprises.63.501.3.4.2.1.6", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (physical->duplex, 0, "Half");
  i_metric_enumstr_add (physical->duplex, 1, "Full");

  /* TxCount */
  physical->tx_count = l_snmp_metric_create (self, obj, "tx_count", "Transmit Count", METRIC_COUNT, "enterprises.63.501.3.4.2.1.7", index_oidstr, RECMETHOD_NONE, 0);
  physical->tx_rate = i_metric_acrate_create (self, obj, "tx_rate", "Transmit Rate", "pps", RECMETHOD_RRD, physical->tx_count, 0);

  /* RxCount */
  physical->rx_count = l_snmp_metric_create (self, obj, "rx_count", "Receive Count", METRIC_COUNT, "enterprises.63.501.3.4.2.1.8", index_oidstr, RECMETHOD_NONE, 0);
  physical->rx_rate = i_metric_acrate_create (self, obj, "rx_rate", "Receive Rate", "pps", RECMETHOD_RRD, physical->rx_count, 0);

  /* Tx Errors */
  physical->tx_errors = l_snmp_metric_create (self, obj, "tx_errors", "Transmit Error Count", METRIC_COUNT, "enterprises.63.501.3.4.2.1.9", index_oidstr, RECMETHOD_NONE, 0);
  physical->tx_eps = i_metric_acrate_create (self, obj, "tx_eps", "Transmit Error Rate", "eps", RECMETHOD_RRD, physical->tx_errors, 0);

  /* TxCount */
  physical->rx_errors = l_snmp_metric_create (self, obj, "rx_errors", "Receive Error Count", METRIC_COUNT, "enterprises.63.501.3.4.2.1.10", index_oidstr, RECMETHOD_NONE, 0);
  physical->rx_eps = i_metric_acrate_create (self, obj, "rx_eps", "Receive Error Rate", "eps", RECMETHOD_RRD, physical->rx_errors, 0);

  /*
   * Custom/Combined Graphs
   */
  physical->tput_cg = i_metric_cgraph_create (obj, "tput_cg", "pps");
  physical->tput_cg->title_str = strdup ("Throughput");
  asprintf (&physical->tput_cg->render_str, "\"LINE1:met_%s_tx_rate_min#000E73:Min.\" \"LINE1:met_%s_tx_rate_avg#001EFF:Avg.\" \"LINE1:met_%s_tx_rate_max#00B4FF:Max. Transmit Throughput\" \"GPRINT:met_%s_tx_rate_min:MIN:   Min %%.2lf %%%%\" \"GPRINT:met_%s_tx_rate_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_tx_rate_max:MAX: Max. %%.2lf %%%%\\n\" \"LINE1:met_%s_rx_rate_min#006B00:Min.\" \"LINE1:met_%s_rx_rate_avg#009B00:Avg.\" \"LINE1:met_%s_rx_rate_max#00ED00:Max. Receive Throughput\" \"GPRINT:met_%s_rx_rate_min:MIN:    Min %%.2lf %%%%\" \"GPRINT:met_%s_rx_rate_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_rx_rate_max:MAX: Max. %%.2lf %%%%\\n\"",
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str);
  i_list_enqueue (physical->tput_cg->met_list, physical->tx_rate);
  i_list_enqueue (physical->tput_cg->met_list, physical->rx_rate);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  /* Enqueue the physical item */
  num = i_list_enqueue (cnt->item_list, physical);
  if (num != 0)
  { i_printf (1, "v_physical_objfact_fab failed to enqueue physical for object %s", obj->name_str); v_physical_item_free (physical); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_physical_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_physical_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the physical and remove
   * it from the item_list
   */

  int num;
  v_physical_item *physical = obj->itemptr;

  if (!physical) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, physical);
  if (num == 0)
  { i_list_delete (cnt->item_list); }

  /* Free the physical */
  v_physical_item_free (physical);
  obj->itemptr = NULL;
  
  return 0;
}
