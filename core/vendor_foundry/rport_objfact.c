#include <stdlib.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/timeutil.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/interface.h"
#include "induction/entity.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/str.h"
#include "device/snmp.h"

#include "rserv.h"
#include "rport.h"

/* 
 * Real Ports - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_rport_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{  
  int num;
  char *ip_str = passdata;
  char *tmp_str;
  v_rport_item *rport;

  /* Port 65535 */
  if (!strcmp(index_oidstr, "65535")) return -1;

  /* Object Configuration */  
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = v_rport_objform;
  obj->histform_func = v_rport_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_rport_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create rport struct */
  rport = v_rport_item_create ();
  if (!rport)
  { i_printf (1, "v_rport_objfact_fab failed to create rport item for obj %s", obj->name_str); return -1; }
  rport->obj = obj;
  obj->itemptr = rport;

  /* Modify index_oidstr to be prefaced with the IP of the server */
  asprintf (&tmp_str, "%s.%s", ip_str, index_oidstr);
  index_oidstr = tmp_str;

  /*
   * Metric Creation
   */

  /* Real Server Name */
  rport->rservname = l_snmp_metric_create (self, obj, "rservname", "Real Server Name", METRIC_STRING, "enterprises.1991.1.1.4.20.1.1.2", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  /* IP Address */
  rport->ipaddr = l_snmp_metric_create (self, obj, "ipaddr", "IP Address", METRIC_IP, "enterprises.1991.1.1.4.20.1.1.1", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  /* Status */
  rport->adminstate = l_snmp_metric_create (self, obj, "adminstate", "Administrative State", METRIC_INTEGER, "enterprises.1991.1.1.4.20.1.1.4", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_metric_enumstr_add (rport->adminstate, 0, "Disabled");
  i_metric_enumstr_add (rport->adminstate, 1, "Enabled");
  rport->opstate = l_snmp_metric_create (self, obj, "opstate", "Operational State", METRIC_INTEGER, "enterprises.1991.1.1.4.24.1.1.5", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_metric_enumstr_add (rport->opstate, 0, "Disabled");
  i_metric_enumstr_add (rport->opstate, 1, "Enabled");
  i_metric_enumstr_add (rport->opstate, 2, "Failed");
  i_metric_enumstr_add (rport->opstate, 3, "Testing");
  i_metric_enumstr_add (rport->opstate, 4, "Suspect");
  i_metric_enumstr_add (rport->opstate, 5, "Shutdown");
  i_metric_enumstr_add (rport->opstate, 6, "Active");

  /* Reassigns */ 
  rport->reassigncount = l_snmp_metric_create (self, obj, "reassigncount", "Reassign Count", METRIC_COUNT, "enterprises.1991.1.1.4.24.1.1.4", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  rport->reassigncount->prio--;
  rport->reassignps = i_metric_acrate_create (self, obj, "reassignps", "Reassigns Per Second", "ra/s", RECMETHOD_RRD, rport->reassigncount, 0);
  rport->reassignps->record_defaultflag = 1;

  /* Connections */
  rport->curconn = l_snmp_metric_create (self, obj, "curconn", "Current Connections", METRIC_GAUGE, "enterprises.1991.1.1.4.24.1.1.7", index_oidstr, RECMETHOD_RRD, 0);
  rport->curconn->record_defaultflag = 1;
  rport->curconn_delta = i_metric_acdiff_create (self, obj, "curconn_delta", "Current Connections Delta", METRIC_FLOAT, RECMETHOD_NONE, rport->curconn, NULL, ACDIFF_REFCB_XMET);
  rport->curconn_delta->prio--;

  rport->conncount = l_snmp_metric_create (self, obj, "conncount", "Connection Count", METRIC_COUNT, "enterprises.1991.1.1.4.24.1.1.8", index_oidstr, RECMETHOD_NONE, 0);
  rport->conncount->prio--;
  rport->conncount_delta = i_metric_acdiff_create (self, obj, "conncount_delta", "Connection Count Delta", METRIC_GAUGE, RECMETHOD_NONE, rport->conncount, NULL, ACDIFF_REFCB_XMET);
  rport->conncount_delta->prio--;
  rport->connps = i_metric_acrate_create (self, obj, "connps", "Connections Per Second", "conn/s", RECMETHOD_RRD, rport->conncount, 0);
  rport->connps->record_defaultflag = 1;

  rport->peakconn = l_snmp_metric_create (self, obj, "peakconn", "Peak Connections", METRIC_COUNT, "enterprises.1991.1.1.4.24.1.1.13", index_oidstr, RECMETHOD_NONE, 0);

  /* Connection Retention */
  rport->connretainpc = i_metric_acpcent_create (self, obj, "connretainpc", "Connection Retention", RECMETHOD_RRD, rport->curconn_delta, rport->conncount_delta, ACPCENT_REFCB_MAX);
  rport->connretainpc->record_defaultflag = 1;
  i_metric_value_free (rport->connretainpc->min_val);
  rport->connretainpc->min_val = NULL;
  i_metric_value_free (rport->connretainpc->max_val);
  rport->connretainpc->max_val = NULL;

  /* 
   * Combined/Custom Graphs 
   */

  rport->tput_cg = i_metric_cgraph_create (obj, "tput_cg", "Conn");
  asprintf (&rport->tput_cg->title_str, "Real Port %s Throughput", obj->desc_str);
  asprintf (&rport->tput_cg->render_str, "\"LINE1:met_%s_curconn_min#000E73:Min.\" \"LINE1:met_%s_curconn_avg#001EFF:Avg.\" \"LINE1:met_%s_curconn_max#00B4FF:Max. Current Conns\" \"GPRINT:met_%s_curconn_min:MIN:  Min %%.2lf Conn\" \"GPRINT:met_%s_curconn_avg:AVERAGE:Avg. %%.2lf Conn\" \"GPRINT:met_%s_curconn_max:MAX: Max. %%.2lf Conn\\n\" \"LINE1:met_%s_connps_min#006B00:Min.\" \"LINE1:met_%s_connps_avg#009B00:Avg.\" \"LINE1:met_%s_connps_max#00ED00:Max. Cn/sec\" \"GPRINT:met_%s_connps_min:MIN:         Min %%.2lf Cn/s\" \"GPRINT:met_%s_connps_avg:AVERAGE: Avg %%.2lf Cn/s\" \"GPRINT:met_%s_connps_max:MAX: Max %%.2lf Conn/s\\n\" \"LINE1:met_%s_reassignps_min#6B0000:Min.\" \"LINE1:met_%s_reassignps_avg#9B0000:Avg.\" \"LINE1:met_%s_reassignps_max#ED0000:Max. Reassign/sec\" \"GPRINT:met_%s_reassignps_min:MIN:   Min %%.2lf RA/s\" \"GPRINT:met_%s_reassignps_avg:AVERAGE: Avg %%.2lf RA/s\" \"GPRINT:met_%s_reassignps_max:MAX: Max %%.2lf Conn/s\\n\"",
   obj->name_str, obj->name_str, obj->name_str,
   obj->name_str, obj->name_str, obj->name_str,
   obj->name_str, obj->name_str, obj->name_str,
   obj->name_str, obj->name_str, obj->name_str,
   obj->name_str, obj->name_str, obj->name_str,
   obj->name_str, obj->name_str, obj->name_str);
  i_list_enqueue (rport->tput_cg->met_list, rport->curconn);
  i_list_enqueue (rport->tput_cg->met_list, rport->connps);
  i_list_enqueue (rport->tput_cg->met_list, rport->reassignps);

  /* 
   * Misc
   */

  /* Free the modified index_oidstr */
  free (index_oidstr);

  /* Enqueue the rport item */
  num = i_list_enqueue (cnt->item_list, rport);
  if (num != 0)
  { i_printf (1, "v_rport_objfact_fab failed to enqueue rport for object %s", obj->name_str); v_rport_item_free (rport); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_rport_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_rport_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  v_rport_item *rport = obj->itemptr;

  if (!rport) return 0;

  return 0;
}

