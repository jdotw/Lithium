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
#include "induction/str.h"
#include "induction/hierarchy.h"
#include "induction/list.h"

#include "device/snmp.h"

#include "unit.h"
#include "port.h"

/* 
 * Fibre Channel Alliance -- Ports
 */

/* Object Factory Fabrication */

int v_port_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_port_item *port;
  v_unit_item *unit = passdata;

  /* Object Configuration */
  char *port_str = l_snmp_get_string_from_pdu(pdu);
  asprintf (&obj->desc_str, "%s Port %s", unit->obj->desc_str, port_str);
  free (port_str);
  obj->prio = 10;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_port_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create port item struct */
  port = v_port_item_create ();
  if (!port)
  { i_printf (1, "v_port_objfact_fab failed to create port item for object %s", obj->name_str); return -1; }
  port->obj = obj;
  obj->itemptr = port;

  /* 
   * Metric Creation 
   */


  /* Speed */
  i_entity_refresh_config refconfig;
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  port->speed_kbyte = l_snmp_metric_create (self, obj, "speed_kbye", "Speed (Kbyte)", METRIC_GAUGE, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.1.10.1.15"), index_oidstr, RECMETHOD_NONE, 0);
  port->speed_kbyte->unit_str = strdup ("KByte/s");
  port->speed_kbyte->hidden = 1;
  port->speed = i_metric_create ("speed", "Speed", METRIC_FLOAT);
  port->speed->unit_str = strdup ("bit");
  port->speed->valstr_func = i_string_rate_metric;
  i_entity_register (self, ENTITY(obj), ENTITY(port->speed));
  i_entity_refresh_config_apply (self, ENTITY(port->speed), &refconfig);
  i_entity_refreshcb_add (ENTITY(port->speed_kbyte), v_port_speed_refcb, port->speed);
            
  /* State */
  port->state = l_snmp_metric_create (self, obj, "state", "Admin State", METRIC_INTEGER, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.1.10.1.6"), index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (port->state, 1, "Unknown");
  i_metric_enumstr_add (port->state, 2, "Online");
  i_metric_enumstr_add (port->state, 3, "Offline");
  i_metric_enumstr_add (port->state, 4, "Bypassed");
  i_metric_enumstr_add (port->state, 5, "Diagnostic");
  port->state->summary_flag = 1;
  i_entity_refreshcb_add (ENTITY(port->state), v_port_adminstate_refcb, NULL);
  
  /* Status */
  port->status = l_snmp_metric_create (self, obj, "status", "Operational Status", METRIC_INTEGER, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.1.10.1.7"), index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (port->status, 1, "Unknown");
  i_metric_enumstr_add (port->status, 2, "Unused");
  i_metric_enumstr_add (port->status, 3, "Ready");
  i_metric_enumstr_add (port->status, 4, "Warning");
  i_metric_enumstr_add (port->status, 5, "Failure");
  i_metric_enumstr_add (port->status, 6, "Not Participating");
  i_metric_enumstr_add (port->status, 7, "Initializing");
  i_metric_enumstr_add (port->status, 8, "Bypass");
  i_metric_enumstr_add (port->status, 9, "FCP Offline Status");
  i_metric_enumstr_add (port->status, 10, "Other");
  port->status->summary_flag = 1;
  i_entity_refreshcb_add (ENTITY(port->status), v_port_status_refcb, NULL);

  /* Hardware State */
  port->hwstate = l_snmp_metric_create (self, obj, "hwstate", "Hardware State", METRIC_INTEGER, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.1.10.1.23"), index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (port->hwstate, 1, "Unknown");
  i_metric_enumstr_add (port->hwstate, 2, "Failed");
  i_metric_enumstr_add (port->hwstate, 3, "Bypassed");
  i_metric_enumstr_add (port->hwstate, 4, "Active");
  i_metric_enumstr_add (port->hwstate, 5, "Loopback");
  i_metric_enumstr_add (port->hwstate, 6, "TX Fault");
  i_metric_enumstr_add (port->hwstate, 7, "No Media");
  i_metric_enumstr_add (port->hwstate, 8, "Link Down");
  port->hwstate->summary_flag = 1;

  /* Alias (Name) */
  port->alias = l_snmp_metric_create (self, obj, "alias", "Alias", METRIC_STRING, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.1.10.1.17"), index_oidstr, RECMETHOD_NONE, 0);
  port->alias->summary_flag = 1;
  
  /* WWN */
  port->wwn = l_snmp_metric_create (self, obj, "wwn", "WWN", METRIC_HEXSTRING, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.1.10.1.10"), index_oidstr, RECMETHOD_NONE, 0);
  
  /* Transmitter Type */
  port->txtype = l_snmp_metric_create (self, obj, "txtype", "Media", METRIC_INTEGER, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.1.10.1.8"), index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (port->txtype, 1, "Unknown");
  i_metric_enumstr_add (port->txtype, 2, "Other");
  i_metric_enumstr_add (port->txtype, 3, "Unused");
  i_metric_enumstr_add (port->txtype, 4, "Shortwave");
  i_metric_enumstr_add (port->txtype, 5, "Longwave");
  i_metric_enumstr_add (port->txtype, 6, "Copper");
  i_metric_enumstr_add (port->txtype, 7, "SCSI");
  i_metric_enumstr_add (port->txtype, 8, "Longwave");
  i_metric_enumstr_add (port->txtype, 9, "Shortwave");
  i_metric_enumstr_add (port->txtype, 10, "LongwaveLED");
  i_metric_enumstr_add (port->txtype, 11, "SSA");

  /* Module Type */
  port->modtype = l_snmp_metric_create (self, obj, "modtype", "Module", METRIC_INTEGER, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.1.10.1.9"), index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (port->modtype, 1, "Unknown");
  i_metric_enumstr_add (port->modtype, 2, "Other");
  i_metric_enumstr_add (port->modtype, 3, "GBIC");
  i_metric_enumstr_add (port->modtype, 4, "Embedded");
  i_metric_enumstr_add (port->modtype, 5, "GLM");
  i_metric_enumstr_add (port->modtype, 6, "GBIC");
  i_metric_enumstr_add (port->modtype, 7, "GBIC");
  i_metric_enumstr_add (port->modtype, 8, "Not Installed");
  i_metric_enumstr_add (port->modtype, 9, "SFP");
  
  /* 
   * Throughout Stats 
   */

  port->octets_in = l_snmp_metric_create (self, obj, "octetes_in", "Receive Element Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.7"), index_oidstr, RECMETHOD_NONE, 0);  
  port->octets_in->hidden = 1;
  port->bps_in = i_metric_acrate_create (self, obj, "bps_in", "Input Rate", "byte/s", RECMETHOD_RRD, port->octets_in, 0);
  port->bps_in->record_defaultflag = 1;
  port->octets_out = l_snmp_metric_create (self, obj, "octetes_out", "Transmit Element Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.6"), index_oidstr, RECMETHOD_NONE, 0);
  port->octets_out->hidden = 1;
  port->bps_out = i_metric_acrate_create (self, obj, "bps_out", "Output Rate", "byte/s", RECMETHOD_RRD, port->octets_out, 0);
  port->bps_out->record_defaultflag = 1;
  port->utilpc_in = i_metric_acpcent_create (self, obj, "utilpc_in", "Input Utilisation", RECMETHOD_RRD, port->bps_in, port->speed, ACPCENT_REFCB_GAUGE);
  port->utilpc_in->record_defaultflag = 1;
  port->utilpc_out = i_metric_acpcent_create (self, obj, "utilpc_out", "Output Utilisation", RECMETHOD_RRD, port->bps_out, port->speed, ACPCENT_REFCB_GAUGE);
  port->utilpc_out->record_defaultflag = 1;

  /* 
   * Error Stats
   */

  port->invalid_crc_count = l_snmp_metric_create (self, obj, "invalid_crc_count", "CRC Error Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.40"), index_oidstr, RECMETHOD_NONE, 0);
  port->invalid_crc_count->hidden = 1;
  port->invalid_crc_rate = i_metric_acrate_create (self, obj, "invalid_crc_rate", "CRC Errors", "err/s", RECMETHOD_RRD, port->invalid_crc_count, 0);
  port->invalid_crc_rate->record_defaultflag = 1;

  port->encoding_error_count = l_snmp_metric_create (self, obj, "encoding_error_count", "Encoding Error Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.50"), index_oidstr, RECMETHOD_NONE, 0);
  port->encoding_error_count->hidden = 1;
  port->encoding_error_rate = i_metric_acrate_create (self, obj, "encoding_error_rate", "Encoding Errors", "err/s", RECMETHOD_RRD, port->encoding_error_count, 0);
  port->encoding_error_rate->record_defaultflag = 1;

  port->rx_link_reset_count = l_snmp_metric_create (self, obj, "rx_link_reset_count", "Link Resets Received Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.33"), index_oidstr, RECMETHOD_NONE, 0);
  port->rx_link_reset_count->hidden = 1;
  port->rx_link_reset_rate = i_metric_acrate_create (self, obj, "rx_link_reset_rate", "Link Resets Received", "lr/s", RECMETHOD_RRD, port->rx_link_reset_count, 0);
//  port->rx_link_reset_rate->record_defaultflag = 1;

  port->tx_link_reset_count = l_snmp_metric_create (self, obj, "tx_link_reset_count", "Link Resets Sent Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.34"), index_oidstr, RECMETHOD_NONE, 0);
  port->tx_link_reset_count->hidden = 1;
  port->tx_link_reset_rate = i_metric_acrate_create (self, obj, "tx_link_reset_rate", "Link Resets Sent", "lr/s", RECMETHOD_RRD, port->tx_link_reset_count, 0);
//  port->tx_link_reset_rate->record_defaultflag = 1;

  port->rx_ols_count = l_snmp_metric_create (self, obj, "rx_ols_count", "Offline Seq Received Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.36"), index_oidstr, RECMETHOD_NONE, 0);
  port->rx_ols_count->hidden = 1;
  port->rx_ols_rate = i_metric_acrate_create (self, obj, "rx_ols_rate", "Offline Seq Received", "seq/s", RECMETHOD_RRD, port->rx_ols_count, 0);
//  port->rx_ols_rate->record_defaultflag = 1;

  port->tx_ols_count = l_snmp_metric_create (self, obj, "tx_ols_count", "Offline Seq Sent Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.37"), index_oidstr, RECMETHOD_NONE, 0);
  port->tx_ols_count->hidden = 1;
  port->tx_ols_rate = i_metric_acrate_create (self, obj, "tx_ols_rate", "Offline Seq Sent", "seq/s", RECMETHOD_RRD, port->tx_ols_count, 0);
//  port->tx_ols_rate->record_defaultflag = 1;

  port->link_failure_count = l_snmp_metric_create (self, obj, "link_failure_count", "Link Failures", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.39"), index_oidstr, RECMETHOD_NONE, 0);
  port->link_failure_count->hidden = 1;
  port->link_failure_rate = i_metric_acrate_create (self, obj, "link_failure_rate", "Link Failure Rate", "err/s", RECMETHOD_RRD, port->link_failure_count, 0);
//  port->link_failure_rate->record_defaultflag = 1;

  port->invalid_word_count = l_snmp_metric_create (self, obj, "invalid_word_count", "Invalid Word Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.41"), index_oidstr, RECMETHOD_NONE, 0);
  port->invalid_word_count->hidden = 1;
  port->invalid_word_rate = i_metric_acrate_create (self, obj, "invalid_word_rate", "Invalid Word Errors", "err/s", RECMETHOD_RRD, port->invalid_word_count, 0);
//  port->invalid_word_rate->record_defaultflag = 1;

  port->sequence_error_count = l_snmp_metric_create (self, obj, "sequence_error_count", "Sequence Error Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.42"), index_oidstr, RECMETHOD_NONE, 0);
  port->sequence_error_count->hidden = 1;
  port->sequence_error_rate = i_metric_acrate_create (self, obj, "sequence_error_rate", "Sequence Errors", "err/s", RECMETHOD_RRD, port->sequence_error_count, 0);
//  port->sequence_error_rate->record_defaultflag = 1;

  port->lost_signal_count = l_snmp_metric_create (self, obj, "lost_signal_count", "Lost Signal Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.43"), index_oidstr, RECMETHOD_NONE, 0);
  port->lost_signal_count->hidden = 1;
  port->lost_signal_rate = i_metric_acrate_create (self, obj, "lost_signal_rate", "Lost Signal Rate", "err/s", RECMETHOD_RRD, port->lost_signal_count, 0);
//  port->lost_signal_rate->record_defaultflag = 1;

  port->lost_sync_count = l_snmp_metric_create (self, obj, "lost_sync_count", "Lost Sync Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.44"), index_oidstr, RECMETHOD_NONE, 0);
  port->lost_sync_count->hidden = 1;
  port->lost_sync_rate = i_metric_acrate_create (self, obj, "lost_sync_rate", "Lost Sync Rate", "err/s", RECMETHOD_RRD, port->lost_sync_count, 0);
//  port->lost_sync_rate->record_defaultflag = 1;

  port->invalid_os_count = l_snmp_metric_create (self, obj, "invalid_os_count", "Bad Ordered Set Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.45"), index_oidstr, RECMETHOD_NONE, 0);
  port->invalid_os_count->hidden = 1;
  port->invalid_os_rate = i_metric_acrate_create (self, obj, "invalid_os_rate", "Bad Ordered Sets", "err/s", RECMETHOD_RRD, port->invalid_os_count, 0);
//  port->invalid_os_rate->record_defaultflag = 1;

  port->frame_too_long_count = l_snmp_metric_create (self, obj, "frame_too_long_count", "Oversize Frame Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.46"), index_oidstr, RECMETHOD_NONE, 0);
  port->frame_too_long_count->hidden = 1;
  port->frame_too_long_rate = i_metric_acrate_create (self, obj, "frame_too_long_rate", "Oversize Frames", "err/s", RECMETHOD_RRD, port->frame_too_long_count, 0);
//  port->frame_too_long_rate->record_defaultflag = 1;

  port->frame_trunc_count = l_snmp_metric_create (self, obj, "frame_trunc_count", "Truncated Frame Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.47"), index_oidstr, RECMETHOD_NONE, 0);
  port->frame_trunc_count->hidden = 1;
  port->frame_trunc_rate = i_metric_acrate_create (self, obj, "frame_trunc_rate", "Truncated Frames", "err/s", RECMETHOD_RRD, port->frame_trunc_count, 0);
//  port->frame_trunc_rate->record_defaultflag = 1;

  port->addr_error_count = l_snmp_metric_create (self, obj, "addr_error_count", "Address Error Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.48"), index_oidstr, RECMETHOD_NONE, 0);
  port->addr_error_count->hidden = 1;
  port->addr_error_rate = i_metric_acrate_create (self, obj, "addr_error_rate", "Address Errors", "err/s", RECMETHOD_RRD, port->addr_error_count, 0);
//  port->addr_error_rate->record_defaultflag = 1;

  port->delim_error_count = l_snmp_metric_create (self, obj, "delim_error_count", "Delimiter Error Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.49"), index_oidstr, RECMETHOD_NONE, 0);
  port->delim_error_count->hidden = 1;
  port->delim_error_rate = i_metric_acrate_create (self, obj, "delim_error_rate", "Delimiter Errors", "err/s", RECMETHOD_RRD, port->delim_error_count, 0);
//  port->delim_error_rate->record_defaultflag = 1;

  /* 
   * Generic Error Stats 
   */

  port->errors = l_snmp_metric_create (self, obj, "errors", "Total Error Count", METRIC_COUNT_HEX64, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.4.5.1.3"), index_oidstr, RECMETHOD_NONE, 0);
  port->errors->hidden = 1;
  port->eps = i_metric_acrate_create (self, obj, "eps", "Total Error Rate", "err/s", RECMETHOD_RRD, port->errors, 0);
  port->eps->record_defaultflag = 1;
  
  /* Enqueue the port item */
  num = i_list_enqueue (cnt->item_list, port);
  if (num != 0)
  { i_printf (1, "v_port_objfact_fab failed to enqueue port for object %s", obj->name_str); v_port_item_free (port); return -1; }

  /*
   * Combined/Custom Graphs
   */
  port->tput_cg = i_metric_cgraph_create (obj, "tput_cg", "Bits/sec");
  asprintf (&port->tput_cg->title_str, "Port '%s' Throughput", obj->desc_str);
  asprintf (&port->tput_cg->render_str, "\"CDEF:kbps_in_min=met_%s_bps_in_min,1000,/\" \"CDEF:kbps_in_avg=met_%s_bps_in_avg,1000,/\" \"CDEF:kbps_in_max=met_%s_bps_in_max,1000,/\" \"CDEF:kbps_out_min=met_%s_bps_out_min,1000,/\" \"CDEF:kbps_out_avg=met_%s_bps_out_avg,1000,/\" \"CDEF:kbps_out_max=met_%s_bps_out_max,1000,/\" \"LINE1:met_%s_bps_in_min#000E73:Min.\" \"LINE1:met_%s_bps_in_avg#001EFF:Avg.\" \"LINE1:met_%s_bps_in_max#00B4FF: Max. Input\" \"GPRINT:kbps_in_min:MIN:   Min %%.2lf Kbits/s\" \"GPRINT:kbps_in_avg:AVERAGE: Avg %%.2lf Kbits/s\" \"GPRINT:kbps_in_max:MAX: Max %%.2lf Kbits/s\\n\" \"LINE1:met_%s_bps_out_min#006B00:Min.\" \"LINE1:met_%s_bps_out_avg#009B00:Avg.\" \"LINE1:met_%s_bps_out_max#00ED00: Max. Output\" \"GPRINT:kbps_out_min:MIN:  Min %%.2lf Kbits/s\" \"GPRINT:kbps_out_avg:AVERAGE: Avg %%.2lf Kbits/s\" \"GPRINT:kbps_out_max:MAX: Max %%.2lf Kbits/s\\n\"",
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str);
  i_list_enqueue (port->tput_cg->met_list, port->bps_in);
  i_list_enqueue (port->tput_cg->met_list, port->bps_out);
  
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
  /* FIX needs to free the port and remove
   * it from the item_list
   */

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
