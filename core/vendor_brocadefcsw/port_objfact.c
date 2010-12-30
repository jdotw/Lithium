#include <stdlib.h>

#include "induction.h"
#include "induction/timer.h"
#include "induction/timeutil.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/hashtable.h"
#include "induction/list.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/str.h"
#include "induction/hierarchy.h"

#include "device/snmp.h"
#include "device/record.h"

#include "port.h"
#include "slot.h"

/* 
 * Brocade Fibre Channel Ports (Not Slot-specific) - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_port_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  /* Object Configuration */
  char *desc_str = l_snmp_get_string_from_pdu(pdu);

  /* Check to see if the specified (desc) has a slot/port format */
  if (strstr(desc_str, "/"))
  {
    /* Slot/Port detected in name, try to create a slot container
     * if one doesn't already exist and then disregard this port
     */
    char *slot_prefix;
    char *slot_cnt_name;
    asprintf(&slot_prefix, "%.*s", (strstr(desc_str, "/") - desc_str), desc_str);
    asprintf(&slot_cnt_name, "fcslot_%s", slot_prefix);
    i_container *slot_cnt = (i_container *) i_entity_child_get(ENTITY(cnt->dev), slot_cnt_name);
    if (!slot_cnt)
    {
      v_slot_enable(self, slot_prefix);
    }
    free (slot_cnt_name);
    free (slot_prefix);
    return -1;    // Disregard slot/port interface
  }
  else
  {
    /* Fabricate the port as normal 
     * (it will be under the "Other Ports" container) 
     */
    
    v_port_objfact_fab_port(self, cnt, obj, pdu, index_oidstr, passdata);

    return 0;
  }
}

/* 
 * Port Fabrication (used by both the Other Ports and Slot-Specific object factories)
 */

int v_port_objfact_fab_port (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  
  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu(pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_port_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create port item struct */
  v_port_item *port = v_port_item_create ();
  if (!port)
  { i_printf (1, "v_port_objfact_fab failed to create port item for object %s", obj->name_str); return -1; }
  port->obj = obj;
  obj->itemptr = port;

  /* 
   * Create trigger sets
   */
  
  obj->tset_list = i_list_create ();
  obj->tset_ht = i_hashtable_create (30);

  i_triggerset *tset = i_triggerset_create ("opstate", "Operational State", "opstate");
  i_triggerset_addtrg (self, tset, "unknown", "Unknown", VALTYPE_INTEGER, TRGTYPE_EQUAL, 0, NULL, 0, NULL, 0, ENTSTATE_FATAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "offline", "Offline", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_FATAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "testing", "Testing", VALTYPE_INTEGER, TRGTYPE_EQUAL, 3, NULL, 0, NULL, 0, ENTSTATE_FATAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "faulty", "Faulty", VALTYPE_INTEGER, TRGTYPE_EQUAL, 4, NULL, 0, NULL, 0, ENTSTATE_FATAL, TSET_FLAG_VALAPPLY);
  tset->default_applyflag = 1;
  i_triggerset_assign_obj (self, obj, tset);

  /* Errors per second */
  tset = i_triggerset_create ("error_rate", "Errors Per Second", "error_rate");
  i_triggerset_addtrg (self, tset, "errors", "Errors", VALTYPE_FLOAT, TRGTYPE_GT, 10, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  tset->default_applyflag = 1;
  i_triggerset_assign_obj (self, obj, tset);


  /* 
   * Port Status Metrics
   */

  /* Physical State */ 
  port->phystate = l_snmp_metric_create (self, obj, "phystate", "Physical State", METRIC_INTEGER, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.3", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (port->phystate, 1, "No Card");
  i_metric_enumstr_add (port->phystate, 2, "No Transceiver");
  i_metric_enumstr_add (port->phystate, 3, "Laser Fault");
  i_metric_enumstr_add (port->phystate, 4, "No Light");
  i_metric_enumstr_add (port->phystate, 5, "No Sync");
  i_metric_enumstr_add (port->phystate, 6, "In Sync");
  i_metric_enumstr_add (port->phystate, 7, "Port Fault");
  i_metric_enumstr_add (port->phystate, 8, "Diag Fault");
  i_metric_enumstr_add (port->phystate, 9, "Reference Locking");

  /* Op State */
  port->opstate = l_snmp_metric_create (self, obj, "opstate", "Operational State", METRIC_INTEGER, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.4", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (port->opstate, 0, "Unknown");
  i_metric_enumstr_add (port->opstate, 1, "Online");
  i_metric_enumstr_add (port->opstate, 2, "Offline");
  i_metric_enumstr_add (port->opstate, 3, "Testing");
  i_metric_enumstr_add (port->opstate, 4, "Faulty");

  /* Admin State */
  port->adminstate = l_snmp_metric_create (self, obj, "adminstate", "Admin State", METRIC_INTEGER, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.5", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (port->adminstate, 1, "Online");
  i_metric_enumstr_add (port->adminstate, 2, "Offline");
  i_metric_enumstr_add (port->adminstate, 3, "Testing");
  i_metric_enumstr_add (port->adminstate, 4, "Faulty");
  i_entity_refreshcb_add (ENTITY(port->adminstate), v_port_adminstate_refcb, port->adminstate);

  /* Link state */
  port->linkstate = l_snmp_metric_create (self, obj, "linkstate", "Link State", METRIC_INTEGER, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.6", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (port->linkstate, 1, "Enabled");
  i_metric_enumstr_add (port->linkstate, 2, "Disabled");
  i_metric_enumstr_add (port->linkstate, 3, "Loopback");

  /*
   * Port Info Metrics
   */

  /* Transmit Type (Media) */
  port->txtype = l_snmp_metric_create (self, obj, "txtype", "Media", METRIC_INTEGER, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.7", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (port->txtype, 1, "Unknown");
  i_metric_enumstr_add (port->txtype, 2, "Long-Wave Laser");
  i_metric_enumstr_add (port->txtype, 3, "Short-Wave Laser");
  i_metric_enumstr_add (port->txtype, 4, "Long-Wave LED");
  i_metric_enumstr_add (port->txtype, 5, "Copper");

  /* WWN */
  port->wwn = l_snmp_metric_create (self, obj, "wwn", "WWN", METRIC_HEXSTRING, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.34", index_oidstr, RECMETHOD_NONE, 0);

  /* Brocade Port Type */
  port->type = l_snmp_metric_create (self, obj, "type", "Port Type", METRIC_INTEGER, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.39", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (port->type, 1, "Unknown");
  i_metric_enumstr_add (port->type, 2, "Other");
  i_metric_enumstr_add (port->type, 3, "Public Loop (FL)");
  i_metric_enumstr_add (port->type, 4, "Fabric Port (F)");
  i_metric_enumstr_add (port->type, 5, "Expansion Port (E)");
  i_metric_enumstr_add (port->type, 6, "Generic Port (G)");
  i_metric_enumstr_add (port->type, 7, "EX");
  
  /* Speed (_enum is the SNMP metric, speed if set by the refcb) */
  i_entity_refresh_config refconfig;
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  port->speed_enum = l_snmp_metric_create (self, obj, "speed_enum", "Speed (Enumerator)", METRIC_INTEGER, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.35", index_oidstr, RECMETHOD_NONE, 0);
  port->speed = i_metric_create ("speed", "Speed", METRIC_FLOAT);
  port->speed->unit_str = strdup ("bit");
  port->speed->valstr_func = i_string_rate_metric;
  i_entity_register (self, ENTITY(obj), ENTITY(port->speed));
  i_entity_refresh_config_apply (self, ENTITY(port->speed), &refconfig);
  i_entity_refreshcb_add (ENTITY(port->speed_enum), v_port_speed_enum_refcb, port->speed);

  /*
   * Throughput Metrics
   */

  /* Octets (word count with an alloc_unit of 4) */
  port->tx_byte_count = l_snmp_metric_create (self, obj, "tx_byte_count", "Receive Octet Count", METRIC_COUNT, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.11", index_oidstr, RECMETHOD_NONE, 0);  
  port->tx_byte_count->alloc_unit = 4;
  port->tx_byte_count->hidden = 1;
  port->rx_byte_count = l_snmp_metric_create (self, obj, "rx_byte_count", "Transmit Octet Count", METRIC_COUNT, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.12", index_oidstr, RECMETHOD_NONE, 0);  
  port->rx_byte_count->alloc_unit = 4;
  port->rx_byte_count->hidden = 1;

  /* Bit per second calculated rate metrics */
  port->rx_bps = i_metric_acrate_create (self, obj, "rx_bps", "Input Bits Per Second", "bit/s", RECMETHOD_RRD, port->rx_byte_count, ACRATE_MOD_BYTETOBIT);
  port->rx_bps->record_defaultflag = 1;
  port->tx_bps = i_metric_acrate_create (self, obj, "tx_bps", "Output Bits Per Second", "bit/s", RECMETHOD_RRD, port->tx_byte_count, ACRATE_MOD_BYTETOBIT);
  port->tx_bps->record_defaultflag = 1;

  /* RX Utilization */
  port->rx_utilpc = i_metric_acpcent_create (self, obj, "rx_utilpc", "Input Utilisation", RECMETHOD_RRD, port->rx_bps, port->speed, ACPCENT_REFCB_GAUGE);
  port->rx_utilpc->record_defaultflag = 1;
  port->rx_utilpc->summary_flag = 1;

  /* TX Utilization */
  port->tx_utilpc = i_metric_acpcent_create (self, obj, "tx_utilpc", "Output Utilisation", RECMETHOD_RRD, port->tx_bps, port->speed, ACPCENT_REFCB_GAUGE);
  port->tx_utilpc->record_defaultflag = 1;
  port->tx_utilpc->summary_flag = 1;

  /*
   * Error Metrics (polled)
   */

  port->too_many_rdy_count = l_snmp_metric_create (self, obj, "too_many_rdy_count", "Too Many RDYs Count", METRIC_COUNT, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.19", index_oidstr, RECMETHOD_NONE, 0);  
  port->too_many_rdy_count->hidden = 1;
  port->too_many_rdy_rate = i_metric_acrate_create (self, obj, "too_many_rdy_rate", "Too Many RDYs", "err/s", RECMETHOD_RRD, port->too_many_rdy_count, 0);

  port->no_tx_credit_count = l_snmp_metric_create (self, obj, "no_tx_credit_count", "No TX Credit Count", METRIC_COUNT, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.20", index_oidstr, RECMETHOD_NONE, 0);  
  port->no_tx_credit_count->hidden = 1;
  port->no_tx_credit_rate = i_metric_acrate_create (self, obj, "no_tx_credit_rate", "No TX Credits", "err/s", RECMETHOD_RRD, port->no_tx_credit_count, 0);

  port->rx_inenc_err_count = l_snmp_metric_create (self, obj, "rx_inenc_err_count", "In-Frame Encoding Error Count", METRIC_COUNT, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.21", index_oidstr, RECMETHOD_NONE, 0);  
  port->rx_inenc_err_count->hidden = 1;
  port->rx_inenc_err_rate = i_metric_acrate_create (self, obj, "rx_inenc_err_rate", "In-Frame Encoding Errors", "err/s", RECMETHOD_RRD, port->rx_inenc_err_count, 0);

  port->rx_outenc_err_count = l_snmp_metric_create (self, obj, "rx_outenc_err_count", "Out-Frame Encoding Error Count", METRIC_COUNT, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.26", index_oidstr, RECMETHOD_NONE, 0);  
  port->rx_outenc_err_count->hidden = 1;
  port->rx_outenc_err_rate = i_metric_acrate_create (self, obj, "rx_outenc_err_rate", "Out-Frame Encoding Errors", "err/s", RECMETHOD_RRD, port->rx_outenc_err_count, 0);

  port->rx_crc_count = l_snmp_metric_create (self, obj, "rx_crc_count", "CRC Error Count", METRIC_COUNT, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.22", index_oidstr, RECMETHOD_NONE, 0);  
  port->rx_crc_count->hidden = 1;
  port->rx_crc_rate = i_metric_acrate_create (self, obj, "rx_crc_rate", "CRC Errors", "err/s", RECMETHOD_RRD, port->rx_crc_count, 0);

  port->rx_trunc_count = l_snmp_metric_create (self, obj, "rx_trunc_count", "Truncated Frame Count", METRIC_COUNT, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.23", index_oidstr, RECMETHOD_NONE, 0);  
  port->rx_trunc_count->hidden = 1;
  port->rx_trunc_rate = i_metric_acrate_create (self, obj, "rx_trunc_rate", "Truncated Frames", "err/s", RECMETHOD_RRD, port->rx_trunc_count, 0);

  port->rx_too_long_count = l_snmp_metric_create (self, obj, "rx_too_long_count", "Oversize Frame Count", METRIC_COUNT, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.24", index_oidstr, RECMETHOD_NONE, 0);  
  port->rx_too_long_count->hidden = 1;
  port->rx_too_long_rate = i_metric_acrate_create (self, obj, "rx_too_long_rate", "Oversize Frames", "err/s", RECMETHOD_RRD, port->rx_too_long_count, 0);

  port->rx_bad_eof_count = l_snmp_metric_create (self, obj, "rx_bad_eof_count", "Bad EOF Count", METRIC_COUNT, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.25", index_oidstr, RECMETHOD_NONE, 0);  
  port->rx_bad_eof_count->hidden = 1;
  port->rx_bad_eof_rate = i_metric_acrate_create (self, obj, "rx_bad_eof_rate", "Bad EOFs", "err/s", RECMETHOD_RRD, port->rx_bad_eof_count, 0);

  port->rx_bad_os_count = l_snmp_metric_create (self, obj, "rx_bad_os_count", "Bad Ordered-Set Count", METRIC_COUNT, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.27", index_oidstr, RECMETHOD_NONE, 0);  
  port->rx_bad_os_count->hidden = 1;
  port->rx_bad_os_rate = i_metric_acrate_create (self, obj, "rx_bad_os_rate", "Bad Ordered-Sets", "err/s", RECMETHOD_RRD, port->rx_bad_os_count, 0);

  port->rx_lip_count = l_snmp_metric_create (self, obj, "rx_lip_count", "Loop Inits Received Count", METRIC_COUNT, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.31", index_oidstr, RECMETHOD_NONE, 0);  
  port->rx_lip_count->hidden = 1;
  port->rx_lip_rate = i_metric_acrate_create (self, obj, "rx_lip_rate", "Loop Inits Received", "lip/s", RECMETHOD_RRD, port->rx_lip_count, 0);

  port->tx_lip_count = l_snmp_metric_create (self, obj, "tx_lip_count", "Loop Inits Sent Count", METRIC_COUNT, ".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.32", index_oidstr, RECMETHOD_NONE, 0);  
  port->tx_lip_count->hidden = 1;
  port->tx_lip_rate = i_metric_acrate_create (self, obj, "tx_lip_rate", "Loop Inits Sent", "lip/s", RECMETHOD_RRD, port->tx_lip_count, 0);


  /* 
   * Error Metrics (calculated aggregates)
   */

  /* Errors */
  port->error_count = i_metric_acsum_create(self, obj, "error_count", "Aggregate Error Count", METRIC_FLOAT, RECMETHOD_RRD, NULL, NULL, ACSUM_REFMETHOD_PARENT);
  port->error_count->hidden = 1;
  i_metric_acsum_addmet(port->error_count, port->too_many_rdy_count, 0);
  i_metric_acsum_addmet(port->error_count, port->no_tx_credit_count, 0);
  i_metric_acsum_addmet(port->error_count, port->rx_inenc_err_count, 0);
  i_metric_acsum_addmet(port->error_count, port->rx_crc_count, 0);
  i_metric_acsum_addmet(port->error_count, port->rx_trunc_count, 0);
  i_metric_acsum_addmet(port->error_count, port->rx_too_long_count, 0);
  i_metric_acsum_addmet(port->error_count, port->rx_bad_eof_count, 0);
  i_metric_acsum_addmet(port->error_count, port->rx_outenc_err_count, 0);
  i_metric_acsum_addmet(port->error_count, port->rx_bad_os_count, 0);
  i_metric_acsum_addmet(port->error_count, port->rx_lip_count, 0);
  i_metric_acsum_addmet(port->error_count, port->tx_lip_count, 0);

  /* Error Rate */
  port->error_rate = i_metric_acrate_create (self, obj, "error_rate", "Error Rate", "err/s", RECMETHOD_RRD, port->error_count, 0);
  port->error_rate->summary_flag = 1;
  port->error_rate->record_defaultflag = 1;

  /* Evaluate all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recording rules */
  l_record_eval_recrules_obj (self, obj);

  /* Enqueue the avail item */
  i_list_enqueue (cnt->item_list, port);

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_port_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_port_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  v_port_item *port = obj->itemptr;

  if (!port) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, port);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
