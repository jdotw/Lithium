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
#include <induction/value.h>
#include <induction/str.h>
#include <lithium/snmp.h>

#include "rserv.h"
#include "rport.h"

/* 
 * Real Servers - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_rserv_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{  
  int num;
  v_rserv_item *rserv;

  /* Object Configuration */  
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = v_rserv_objform;
  obj->histform_func = v_rserv_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_rserv_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create rserv struct */
  rserv = v_rserv_item_create ();
  if (!rserv)
  { i_printf (1, "v_rserv_objfact_fab failed to create rserv item for obj %s", obj->name_str); return -1; }
  rserv->obj = obj;
  obj->itemptr = rserv;

  /*
   * Metric Creation
   */

  /* IP Address */
  rserv->ipaddr = l_snmp_metric_create (self, obj, "ipaddr", "IP Address", METRIC_IP, "enterprises.1991.1.1.4.19.1.1.1", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  /* Status */
  rserv->adminstate = l_snmp_metric_create (self, obj, "adminstate", "Administrative State", METRIC_INTEGER, "enterprises.1991.1.1.4.19.1.1.3", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_metric_enumstr_add (rserv->adminstate, 0, "Disabled");
  i_metric_enumstr_add (rserv->adminstate, 1, "Enabled");
  rserv->opstate = l_snmp_metric_create (self, obj, "opstate", "Operational State", METRIC_INTEGER, "enterprises.1991.1.1.4.23.1.1.8", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_metric_enumstr_add (rserv->opstate, 0, "Disabled");
  i_metric_enumstr_add (rserv->opstate, 1, "Enabled");
  i_metric_enumstr_add (rserv->opstate, 2, "Failed");
  i_metric_enumstr_add (rserv->opstate, 3, "Testing");
  i_metric_enumstr_add (rserv->opstate, 4, "Suspect");
  i_metric_enumstr_add (rserv->opstate, 5, "Shutdown");
  i_metric_enumstr_add (rserv->opstate, 6, "Active");

  /* Reassigns */
  rserv->reassigncount = l_snmp_metric_create (self, obj, "reassigncount", "Reassign Count", METRIC_COUNT, "enterprises.1991.1.1.4.23.1.1.9", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  rserv->reassigncount->prio--;
  rserv->reassignps = i_metric_acrate_create (self, obj, "reassignps", "Reassigns Per Second", "ra/s", RECMETHOD_RRD, rserv->reassigncount, 0);  
  rserv->reassignps->record_defaultflag = 1;
  rserv->reassignlimit = l_snmp_metric_create (self, obj, "reassigncount", "Reassign Count", METRIC_COUNT, "enterprises.1991.1.1.4.23.1.1.10", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  /* Connections */
  rserv->curconn = l_snmp_metric_create (self, obj, "curconn", "Current Connections", METRIC_GAUGE, "enterprises.1991.1.1.4.23.1.1.5", index_oidstr, RECMETHOD_RRD, 0);
  rserv->curconn->record_defaultflag = 1;
  rserv->curconn_delta = i_metric_acdiff_create (self, obj, "curconn_delta", "Current Connections Delta", METRIC_FLOAT, RECMETHOD_NONE, rserv->curconn, NULL, ACDIFF_REFCB_XMET);
  rserv->curconn_delta->prio--;
    
  rserv->conncount = l_snmp_metric_create (self, obj, "conncount", "Connection Count", METRIC_COUNT, "enterprises.1991.1.1.4.23.1.1.6", index_oidstr, RECMETHOD_NONE, 0);
  rserv->conncount->prio--;
  rserv->conncount_delta = i_metric_acdiff_create (self, obj, "conncount_delta", "Connection Count Delta", METRIC_GAUGE, RECMETHOD_NONE, rserv->conncount, NULL, ACDIFF_REFCB_XMET);
  rserv->conncount_delta->prio--;
  rserv->connps = i_metric_acrate_create (self, obj, "connps", "Connections Per Second", "conn/s", RECMETHOD_RRD, rserv->conncount, 0);
  rserv->connps->record_defaultflag = 1;
  
  rserv->peakconn = l_snmp_metric_create (self, obj, "peakconn", "Peak Connections", METRIC_COUNT, "enterprises.1991.1.1.4.23.1.1.13", index_oidstr, RECMETHOD_NONE, 0);
  rserv->maxconn = l_snmp_metric_create (self, obj, "maxconn", "Maximum Connections", METRIC_INTEGER, "enterprises.1991.1.1.4.19.1.1.4", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  /* Connection Retention */
  rserv->connretainpc = i_metric_acpcent_create (self, obj, "connretainpc", "Connection Retention", RECMETHOD_RRD, rserv->curconn_delta, rserv->conncount_delta, ACPCENT_REFCB_MAX);
  rserv->connretainpc->record_defaultflag = 1;
  i_metric_value_free (rserv->connretainpc->min_val);
  rserv->connretainpc->min_val = NULL;
  i_metric_value_free (rserv->connretainpc->max_val);
  rserv->connretainpc->max_val = NULL;

  /* Weight */
  rserv->weight = l_snmp_metric_create (self, obj, "weight", "Weight", METRIC_INTEGER, "enterprises.1991.1.1.4.19.1.1.5", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  /* 
   * Combined/Custom Graphs 
   */

  rserv->tput_cg = i_metric_cgraph_create (obj, "tput_cg", "Conn");
  asprintf (&rserv->tput_cg->title_str, "Real Server %s Throughput", obj->desc_str);
//  asprintf (&rserv->tput_cg->render_str, "\"LINE1:met_%s_curconn_min#000E73:Min.\" \"LINE1:met_%s_curconn_avg#001EFF:Avg.\" \"LINE1:met_%s_curconn_max#00B4FF:Max. Current Conns\" \"GPRINT:met_%s_curconn_min:MIN:  Min %%.2lf Conn\" \"GPRINT:met_%s_curconn_avg:AVERAGE:Avg. %%.2lf Conn\" \"GPRINT:met_%s_curconn_max:MAX: Max. %%.2lf Conn\\n\" \"LINE1:met_%s_connps_min#006B00:Min.\" \"LINE1:met_%s_connps_avg#009B00:Avg.\" \"LINE1:met_%s_connps_max#00ED00:Max. Cn/sec\" \"GPRINT:met_%s_connps_min:MIN:         Min %%.2lf Cn/s\" \"GPRINT:met_%s_connps_avg:AVERAGE: Avg %%.2lf Cn/s\" \"GPRINT:met_%s_connps_max:MAX: Max %%.2lf Conn/s\\n\" \"LINE1:met_%s_reassignps_min#6B0000:Min.\" \"LINE1:met_%s_reassignps_avg#9B0000:Avg.\" \"LINE1:met_%s_reassignps_max#ED0000:Max. Reassign/sec\" \"GPRINT:met_%s_reassignps_min:MIN:   Min %%.2lf RA/s\" \"GPRINT:met_%s_reassignps_avg:AVERAGE: Avg %%.2lf RA/s\" \"GPRINT:met_%s_reassignps_max:MAX: Max %%.2lf Conn/s\\n\"",
//   obj->name_str, obj->name_str, obj->name_str, 
//   obj->name_str, obj->name_str, obj->name_str, 
//   obj->name_str, obj->name_str, obj->name_str, 
//   obj->name_str, obj->name_str, obj->name_str, 
//   obj->name_str, obj->name_str, obj->name_str, 
//   obj->name_str, obj->name_str, obj->name_str);
  i_list_enqueue (rserv->tput_cg->met_list, rserv->curconn);
  i_list_enqueue (rserv->tput_cg->met_list, rserv->connps);
  i_list_enqueue (rserv->tput_cg->met_list, rserv->reassignps);

  /* 
   * Misc
   */

  /* Enqueue the rserv item */
  num = i_list_enqueue (cnt->item_list, rserv);
  if (num != 0)
  { i_printf (1, "v_rserv_objfact_fab failed to enqueue rserv for object %s", obj->name_str); v_rserv_item_free (rserv); return -1; }

  /* Enable real port for rserv */
  num = v_rport_enable (self, rserv, index_oidstr);
  if (num != 0)
  { i_printf (1, "v_rserv_objfact_fab warning, failed to enable v_rport for object %s", obj->name_str); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_rserv_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_rserv_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  v_rserv_item *rserv = obj->itemptr;

  if (!rserv) return 0;

  return 0;
}

