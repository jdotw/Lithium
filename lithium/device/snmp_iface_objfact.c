#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/interface.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/device.h>
#include <induction/metric.h>
#include <induction/navtree.h>
#include <induction/str.h>

#include "navtree.h"
#include "snmp.h"
#include "snmp_ipaddr.h"
#include "snmp_route.h"
#include "nwtput.h"
#include "snmp_iface.h"

/* 
 * SNMP Interfaces - Object Factory Functions 
 */

/* Object Factory Fabrication */

int l_snmp_iface_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  i_interface *iface;
  l_snmp_objfact *fact = l_snmp_iface_objfact();

  /* Object Configuration */
  if (fact->enable_dupname_handling)
  { asprintf (&obj->desc_str, "%s %s", l_snmp_get_string_from_pdu (pdu), index_oidstr); }
  else
  { obj->desc_str = l_snmp_get_string_from_pdu (pdu); }
  obj->mainform_func = l_snmp_iface_objform;
  obj->histform_func = l_snmp_iface_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "l_snmp_iface_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create interface struct */
  iface = i_interface_create ();
  if (!iface)
  { i_printf (1, "l_snmp_iface_objfact_fab failed to create iface for object %s", obj->name_str); return -1; }
  iface->obj = obj;
  obj->itemptr = iface;
  iface->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* System Uptime */
  iface->sysuptime = l_snmp_metric_create (self, obj, "sysuptime", "System Uptime", METRIC_INTERVAL, "sysUpTime", "0", RECMETHOD_NONE, 0);
  
  /* States */
  iface->adminstate = l_snmp_metric_create (self, obj, "adminstate", "Administrative State", METRIC_INTEGER, "ifAdminStatus", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (iface->adminstate, 1, "Up");
  i_metric_enumstr_add (iface->adminstate, 2, "Down");
  i_metric_enumstr_add (iface->adminstate, 3, "Testing");
  i_entity_refreshcb_add (ENTITY(iface->adminstate), l_snmp_iface_adminstate_refcb, NULL);
  iface->opstate = l_snmp_metric_create (self, obj, "opstate", "Operational State", METRIC_INTEGER, "ifOperStatus", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (iface->opstate, 1, "Up");
  i_metric_enumstr_add (iface->opstate, 2, "Down");
  i_metric_enumstr_add (iface->opstate, 3, "Testing");
  i_metric_enumstr_add (iface->opstate, 4, "Unknown");
  i_metric_enumstr_add (iface->opstate, 5, "Dormant");
  i_metric_enumstr_add (iface->opstate, 6, "Not Present");
  i_metric_enumstr_add (iface->opstate, 7, "Lower Layer Down");

  /* Misc */
  iface->alias = l_snmp_metric_create (self, obj, "alias", "Alias", METRIC_STRING, "ifAlias", index_oidstr, RECMETHOD_NONE, 0);
  iface->mtu = l_snmp_metric_create (self, obj, "mtu", "MTU", METRIC_INTEGER, "ifMtu", index_oidstr, RECMETHOD_NONE, 0);
  iface->speed = l_snmp_metric_create (self, obj, "speed", "Speed", METRIC_GAUGE, "ifSpeed", index_oidstr, RECMETHOD_NONE, 0);
  iface->speed->unit_str = strdup ("bit");
  iface->speed->valstr_func = i_string_rate_metric;
  iface->lastchange_uptime = l_snmp_metric_create (self, obj, "lastchange_uptime", "Uptime at Last Change", METRIC_INTERVAL, "ifLastChange", index_oidstr, RECMETHOD_NONE, 0);
  iface->lastchange_uptime->prio--;
  iface->lastchange = i_metric_acdiff_create (self, obj, "lastchange", "Last Change", METRIC_INTERVAL, RECMETHOD_NONE, iface->sysuptime, iface->lastchange_uptime, ACDIFF_REFCB_YMET);
  iface->discont_uptime = l_snmp_metric_create (self, obj, "discont_uptime", "Uptime at Counter Discontinuity", METRIC_INTERVAL, "ifCounterDiscontinuityTime", index_oidstr, RECMETHOD_NONE, 0);
  iface->discont_uptime->prio--;
  iface->discont = i_metric_acdiff_create (self, obj, "discont", "Counter Discontinuity", METRIC_INTERVAL, RECMETHOD_NONE, iface->sysuptime, iface->discont_uptime, ACDIFF_REFCB_YMET);

  /* In/Out Octets, Bps rate and volume metrics */
  iface->octets_in = l_snmp_metric_create (self, obj, "octets_in", "Input Octet Count", METRIC_COUNT, "ifInOctets", index_oidstr, RECMETHOD_NONE, 0);
  iface->octets_in->prio--;
  iface->octets_out = l_snmp_metric_create (self, obj, "octets_out", "Output Octet Count", METRIC_COUNT, "ifOutOctets", index_oidstr, RECMETHOD_NONE, 0);
  iface->octets_out->prio--;
  iface->bps_in = i_metric_acrate_create (self, obj, "bps_in", "Input Bits Per Second", "bit/s", RECMETHOD_RRD, iface->octets_in, ACRATE_MOD_BYTETOBIT);
  iface->bps_in->record_defaultflag = 1;
  iface->bps_out = i_metric_acrate_create (self, obj, "bps_out", "Output Bits Per Second", "bit/s", RECMETHOD_RRD, iface->octets_out, ACRATE_MOD_BYTETOBIT);
  iface->bps_out->record_defaultflag = 1;

  /* Volume metrics (SQL recorded) */
  iface->mb_day_in = i_metric_acvol_create (self, obj, "mb_day_in", "Input Data Volume (Daily Total)", "bytes", (1024 * 1024), RECMETHOD_SQL, iface->octets_in, iface->sysuptime, iface->discont, VALPERIOD_DAY);
  iface->mb_day_in->kbase = 1024;
  iface->mb_day_in->record_defaultflag = 1;
  iface->mb_day_out = i_metric_acvol_create (self, obj, "mb_day_out", "Output Data Volume (Daily Total)", "bytes", (1024 * 1024), RECMETHOD_SQL, iface->octets_out, iface->sysuptime, iface->discont, VALPERIOD_DAY);
  iface->mb_day_out->kbase = 1024;
  iface->mb_day_out->record_defaultflag = 1;
  iface->mb_month_in = i_metric_acvol_create (self, obj, "mb_month_in", "Input Data Volume (Monthly Total)", "bytes", (1024 * 1024), RECMETHOD_SQL, iface->octets_in, iface->sysuptime, iface->discont, VALPERIOD_MONTH);
  iface->mb_month_in->kbase = 1024;
  iface->mb_month_in->record_defaultflag = 1;
  iface->mb_month_out = i_metric_acvol_create (self, obj, "mb_month_out", "Output Data Volume (Monthly Total)", "bytes", (1024 * 1024), RECMETHOD_SQL, iface->octets_out, iface->sysuptime, iface->discont, VALPERIOD_MONTH);
  iface->mb_month_out->kbase = 1024;
  iface->mb_month_out->record_defaultflag = 1;

  /* Utilisation */
  iface->utilpc_in = i_metric_acpcent_create (self, obj, "utilpc_in", "Input Utilisation", RECMETHOD_RRD, iface->bps_in, iface->speed, ACPCENT_REFCB_GAUGE);
  iface->utilpc_in->record_defaultflag = 1;
  iface->utilpc_out = i_metric_acpcent_create (self, obj, "utilpc_out", "Output Utilisation", RECMETHOD_RRD, iface->bps_out, iface->speed, ACPCENT_REFCB_GAUGE);
  iface->utilpc_out->record_defaultflag = 1;
    
  /* Packet counters */
  iface->ucastpkts_in = l_snmp_metric_create (self, obj, "ucastpkts_in", "Input Unicast Packet Count", METRIC_COUNT, "ifInUcastPkts", index_oidstr, RECMETHOD_NONE, 0);
  iface->ucastpkts_in->prio--;
  iface->ucastpkts_out = l_snmp_metric_create (self, obj, "ucastpkts_out", "Output Unicast Packet Count", METRIC_COUNT, "ifOutUcastPkts", index_oidstr, RECMETHOD_NONE, 0);
  iface->ucastpkts_out->prio--;
  iface->upps_in = i_metric_acrate_create (self, obj, "upps_in", "Input Unicast Packets Per Second", "pkts/s", RECMETHOD_NONE, iface->ucastpkts_in, 0);
  iface->upps_out = i_metric_acrate_create (self, obj, "upps_out", "Output Unicast Packets Per Second", "pkts/s", RECMETHOD_NONE, iface->ucastpkts_out, 0);
  iface->nucastpkts_in = l_snmp_metric_create (self, obj, "nucastpkts_in", "Input Non-Unicast Packet Count", METRIC_COUNT, "ifInNUcastPkts", index_oidstr, RECMETHOD_NONE, 0);
  iface->nucastpkts_in->prio--;
  iface->nucastpkts_out = l_snmp_metric_create (self, obj, "nucastpkts_out", "Output Non-Unicast Packet Count", METRIC_COUNT, "ifOutNUcastPkts", index_oidstr, RECMETHOD_NONE, 0);
  iface->nucastpkts_out->prio--;
  iface->nupps_in = i_metric_acrate_create (self, obj, "nupps_in", "Input Non-Unicast Packets Per Second", "pkts/s", RECMETHOD_NONE, iface->nucastpkts_in, 0);
  iface->nupps_out = i_metric_acrate_create (self, obj, "nupps_out", "Output Non-Unicast Packets Per Second", "pkts/s", RECMETHOD_NONE, iface->nucastpkts_out, 0);
  iface->pkts_in = i_metric_acsum_create (self, obj, "pkts_in", "Input Packet Count", METRIC_COUNT, RECMETHOD_NONE, iface->ucastpkts_in, iface->nucastpkts_in, ACSUM_REFCB_YMET);
  iface->pkts_in->prio--;
  iface->pkts_out = i_metric_acsum_create (self, obj, "pkts_out", "Output Packet Count", METRIC_COUNT, RECMETHOD_NONE, iface->ucastpkts_out, iface->nucastpkts_out, ACSUM_REFCB_YMET);
  iface->pkts_out->prio--;
  iface->pps_in = i_metric_acrate_create (self, obj, "pps_in", "Input Packets Per Second", "pkts/s", RECMETHOD_RRD, iface->pkts_in, 0);
  iface->pps_in->record_priority = 2;
  iface->pps_in->record_defaultflag = 1;
  iface->pps_out = i_metric_acrate_create (self, obj, "pps_out", "Output Packets Per Second", "pkts/s", RECMETHOD_RRD, iface->pkts_out, 0);
  iface->pps_out->record_priority = 2;
  iface->pps_out->record_defaultflag = 1;

  /* Errors/Discards */
  iface->errors_in = l_snmp_metric_create (self, obj, "errors_in", "Input Error Count", METRIC_COUNT, "ifInErrors", index_oidstr, RECMETHOD_NONE, 0);
  iface->errors_in->prio--;
  iface->errors_out = l_snmp_metric_create (self, obj, "errors_out", "Output Error Count", METRIC_COUNT, "ifOutErrors", index_oidstr, RECMETHOD_NONE, 0);
  iface->errors_out->prio--;
  iface->eps_in = i_metric_acrate_create (self, obj, "eps_in", "Input Errors Per Second", "err/s", RECMETHOD_RRD, iface->errors_in, 0);
  iface->eps_in->record_priority = 2;
  iface->eps_in->record_defaultflag = 1;
  iface->eps_out = i_metric_acrate_create (self, obj, "eps_out", "Output Errors Per Second", "err/s", RECMETHOD_RRD, iface->errors_out, 0);
  iface->eps_out->record_priority = 2;
  iface->eps_out->record_defaultflag = 1;
  iface->discards_in = l_snmp_metric_create (self, obj, "discards_in", "Input Discard Count", METRIC_COUNT, "ifInDiscards", index_oidstr, RECMETHOD_NONE, 0);
  iface->discards_in->prio--;
  iface->discards_out = l_snmp_metric_create (self, obj, "discards_out", "Output Discard Count", METRIC_COUNT, "ifOutDiscards", index_oidstr, RECMETHOD_NONE, 0);
  iface->discards_out->prio--;
  iface->dps_in = i_metric_acrate_create (self, obj, "dps_in", "Input Discards Per Second", "disc/s", RECMETHOD_RRD, iface->discards_in, 0);
  iface->dps_in->record_priority = 2;
  iface->dps_in->record_defaultflag = 1;
  iface->dps_out = i_metric_acrate_create (self, obj, "dps_out", "Output Discards Per Second", "disc/s", RECMETHOD_RRD, iface->discards_out, 0);
  iface->dps_out->record_priority = 2;
  iface->dps_out->record_defaultflag = 1;

  /*
   * Network Throughput 
   */
  l_nwtput_metset_add (self, iface->bps_in, iface->bps_out, iface->pps_in, iface->pps_out);

  /*
   * Combined/Custom Graphs
   */
  iface->tput_cg = i_metric_cgraph_create (obj, "tput_cg", "Bits/sec");
  asprintf (&iface->tput_cg->title_str, "Interface '%s' Throughput", obj->desc_str); 
  asprintf (&iface->tput_cg->render_str, "\"CDEF:kbps_in_min=met_%s_bps_in_min,1000,/\" \"CDEF:kbps_in_avg=met_%s_bps_in_avg,1000,/\" \"CDEF:kbps_in_max=met_%s_bps_in_max,1000,/\" \"CDEF:kbps_out_min=met_%s_bps_out_min,1000,/\" \"CDEF:kbps_out_avg=met_%s_bps_out_avg,1000,/\" \"CDEF:kbps_out_max=met_%s_bps_out_max,1000,/\" \"LINE1:met_%s_bps_in_min#000E73:Min.\" \"LINE1:met_%s_bps_in_avg#001EFF:Avg.\" \"LINE1:met_%s_bps_in_max#00B4FF: Max. Input\" \"GPRINT:kbps_in_min:MIN:   Min %%.2lf Kbits/s\" \"GPRINT:kbps_in_avg:AVERAGE: Avg %%.2lf Kbits/s\" \"GPRINT:kbps_in_max:MAX: Max %%.2lf Kbits/s\\n\" \"LINE1:met_%s_bps_out_min#006B00:Min.\" \"LINE1:met_%s_bps_out_avg#009B00:Avg.\" \"LINE1:met_%s_bps_out_max#00ED00: Max. Output\" \"GPRINT:kbps_out_min:MIN:  Min %%.2lf Kbits/s\" \"GPRINT:kbps_out_avg:AVERAGE: Avg %%.2lf Kbits/s\" \"GPRINT:kbps_out_max:MAX: Max %%.2lf Kbits/s\\n\"",
    obj->name_str, obj->name_str, obj->name_str, 
    obj->name_str, obj->name_str, obj->name_str, 
    obj->name_str, obj->name_str, obj->name_str, 
    obj->name_str, obj->name_str, obj->name_str);
  i_list_enqueue (iface->tput_cg->met_list, iface->bps_in);
  i_list_enqueue (iface->tput_cg->met_list, iface->bps_out);

  /* Enqueue the iface item */
  num = i_list_enqueue (cnt->item_list, iface);
  if (num != 0)
  { i_printf (1, "l_snmp_iface_objfact_fab failed to enqueue iface for object %s", obj->name_str); i_interface_free (iface); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int l_snmp_iface_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int l_snmp_iface_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  i_interface *iface = obj->itemptr;
  l_snmp_ipaddr *addr;
  l_snmp_route_item *route;

  if (!iface) return 0;

  /* Remove from nwtput */
  l_nwtput_metset_remove (self, iface->bps_in, iface->bps_out, iface->pps_in, iface->pps_out);

  /* NULLify iface pointer on any matching snmp_ipaddr addrs */
  for (i_list_move_head(iface->ip_list); (addr=i_list_restore(iface->ip_list))!=NULL; i_list_move_next(iface->ip_list))
  { addr->iface = NULL; }

  /* NULLify iface pointer on any matching snmp_route route */
  for (i_list_move_head(iface->route_list); (route=i_list_restore(iface->route_list))!=NULL; i_list_move_next(iface->route_list))
  { route->iface = NULL; }

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, iface);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;

  return 0;
}
