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

#include "vserv.h"
#include "vport.h"

/* 
 * Virtual Ports - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_vport_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{  
  int num;
  char *ip_str = passdata;
  char *tmp_str;
  v_vport_item *vport;

  /* Port 65535 - ignore */
  if (!strcmp(index_oidstr, "65535")) return -1;

  /* Object Configuration */  
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = v_vport_objform;
  obj->histform_func = v_vport_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_vport_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create vport struct */
  vport = v_vport_item_create ();
  if (!vport)
  { i_printf (1, "v_vport_objfact_fab failed to create vport item for obj %s", obj->name_str); return -1; }
  vport->obj = obj;
  obj->itemptr = vport;

  /* Modify index_oidstr to be prefaced with the IP of the server */
  asprintf (&tmp_str, "%s.%s", ip_str, index_oidstr);
  index_oidstr = tmp_str;

  /*
   * Metric Creation
   */

  /* Virtual Server Name */
  vport->vservname = l_snmp_metric_create (self, obj, "vservname", "Virtual Server Name", METRIC_STRING, "enterprises.1991.1.1.4.22.1.1.3", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  /* IP Address */
  vport->ipaddr = l_snmp_metric_create (self, obj, "ipaddr", "IP Address", METRIC_IP, "enterprises.1991.1.1.4.22.1.1.1", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  /* Status */
  vport->adminstate = l_snmp_metric_create (self, obj, "adminstate", "Administrative State", METRIC_INTEGER, "enterprises.1991.1.1.4.22.1.1.4", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_metric_enumstr_add (vport->adminstate, 0, "Disabled");
  i_metric_enumstr_add (vport->adminstate, 1, "Enabled");

  /* Connections */
  vport->curconn = l_snmp_metric_create (self, obj, "curconn", "Current Connections", METRIC_GAUGE, "enterprises.1991.1.1.4.26.1.1.4", index_oidstr, RECMETHOD_RRD, 0);
  vport->curconn->record_defaultflag = 1;
  vport->curconn_delta = i_metric_acdiff_create (self, obj, "curconn_delta", "Current Connections Delta", METRIC_FLOAT, RECMETHOD_NONE, vport->curconn, NULL, ACDIFF_REFCB_XMET);
  vport->curconn_delta->prio--;
  
  vport->conncount = l_snmp_metric_create (self, obj, "conncount", "Connection Count", METRIC_COUNT, "enterprises.1991.1.1.4.26.1.1.5", index_oidstr, RECMETHOD_NONE, 0);
  vport->conncount->prio--;
  vport->conncount_delta = i_metric_acdiff_create (self, obj, "conncount_delta", "Connection Count Delta", METRIC_GAUGE, RECMETHOD_NONE, vport->conncount, NULL, ACDIFF_REFCB_XMET);
  vport->conncount_delta->prio--;
  vport->connps = i_metric_acrate_create (self, obj, "connps", "Connections Per Second", "conn/s", RECMETHOD_RRD, vport->conncount, 0);
  vport->connps->record_defaultflag = 1;
  
  vport->peakconn = l_snmp_metric_create (self, obj, "peakconn", "Peak Connections", METRIC_COUNT, "enterprises.1991.1.1.4.26.1.1.6", index_oidstr, RECMETHOD_NONE, 0);

  /* Connection Retention */
  vport->connretainpc = i_metric_acpcent_create (self, obj, "connretainpc", "Connection Retention", RECMETHOD_RRD, vport->curconn_delta, vport->conncount_delta, ACPCENT_REFCB_MAX);
  vport->connretainpc->record_defaultflag = 1;
  i_metric_value_free (vport->connretainpc->min_val);
  vport->connretainpc->min_val = NULL;
  i_metric_value_free (vport->connretainpc->max_val);
  vport->connretainpc->max_val = NULL;


  /* 
   * Combined/Custom Graphs 
   */

  vport->tput_cg = i_metric_cgraph_create (obj, "tput_cg", "Conn");
  asprintf (&vport->tput_cg->title_str, "Virtual Port %s Throughput", obj->desc_str);
  asprintf (&vport->tput_cg->render_str, "\"LINE1:met_%s_curconn_min#000E73:Min.\" \"LINE1:met_%s_curconn_avg#001EFF:Avg.\" \"LINE1:met_%s_curconn_max#00B4FF:Max. Current Conns\" \"GPRINT:met_%s_curconn_min:MIN:  Min %%.2lf Conn\" \"GPRINT:met_%s_curconn_avg:AVERAGE:Avg. %%.2lf Conn\" \"GPRINT:met_%s_curconn_max:MAX: Max. %%.2lf Conn\\n\" \"LINE1:met_%s_connps_min#006B00:Min.\" \"LINE1:met_%s_connps_avg#009B00:Avg.\" \"LINE1:met_%s_connps_max#00ED00:Max. Cn/sec\" \"GPRINT:met_%s_connps_min:MIN:         Min %%.2lf Cn/s\" \"GPRINT:met_%s_connps_avg:AVERAGE: Avg %%.2lf Cn/s\" \"GPRINT:met_%s_connps_max:MAX: Max %%.2lf Conn/s\\n\"",
   obj->name_str, obj->name_str, obj->name_str,
   obj->name_str, obj->name_str, obj->name_str,
   obj->name_str, obj->name_str, obj->name_str,
   obj->name_str, obj->name_str, obj->name_str);
  i_list_enqueue (vport->tput_cg->met_list, vport->curconn);
  i_list_enqueue (vport->tput_cg->met_list, vport->connps);

  /* 
   * Misc
   */

  /* Free the modified index_oidstr */
  free (index_oidstr);

  /* Enqueue the vport item */
  num = i_list_enqueue (cnt->item_list, vport);
  if (num != 0)
  { i_printf (1, "v_vport_objfact_fab failed to enqueue vport for object %s", obj->name_str); v_vport_item_free (vport); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_vport_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_vport_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  v_vport_item *vport = obj->itemptr;

  if (!vport) return 0;

  return 0;
}

