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
#include <induction/str.h>
#include <lithium/snmp.h>

#include "vserv.h"
#include "vport.h"

/* 
 * Virtual Servers - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_vserv_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{  
  int num;
  v_vserv_item *vserv;

  /* Object Configuration */  
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = v_vserv_objform;
  obj->histform_func = v_vserv_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_vserv_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create vserv struct */
  vserv = v_vserv_item_create ();
  if (!vserv)
  { i_printf (1, "v_vserv_objfact_fab failed to create vserv item for obj %s", obj->name_str); return -1; }
  vserv->obj = obj;
  obj->itemptr = vserv;

  /*
   * Metric Creation
   */

  /* IP Address */
  vserv->ipaddr = l_snmp_metric_create (self, obj, "ipaddr", "IP Address", METRIC_IP, "enterprises.1991.1.1.4.21.1.1.1", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  /* Status */
  vserv->adminstate = l_snmp_metric_create (self, obj, "adminstate", "Administrative State", METRIC_INTEGER, "enterprises.1991.1.1.4.21.1.1.3", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_metric_enumstr_add (vserv->adminstate, 0, "Disabled");
  i_metric_enumstr_add (vserv->adminstate, 1, "Enabled");

  /* Connections */
  vserv->conncount = l_snmp_metric_create (self, obj, "conncount", "Connection Count", METRIC_COUNT, "enterprises.1991.1.1.4.25.1.1.5", index_oidstr, RECMETHOD_NONE, 0);
  vserv->conncount->prio--;
  vserv->connps = i_metric_acrate_create (self, obj, "connps", "Connections Per Second", "conn/s", RECMETHOD_RRD, vserv->conncount, 0);
  vserv->connps->record_defaultflag = 1;
  
  /* SDA Type */
  vserv->sdatype = l_snmp_metric_create (self, obj, "sdatype", "SDA Type", METRIC_INTEGER, "enterprises.1991.1.1.4.21.1.1.4", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_metric_enumstr_add (vserv->sdatype, 0, "Default");
  i_metric_enumstr_add (vserv->sdatype, 1, "Least Connections");
  i_metric_enumstr_add (vserv->sdatype, 2, "Round Robin");
  i_metric_enumstr_add (vserv->sdatype, 3, "Weighted");

  /* Symmetric (failover) */
  vserv->symstate = l_snmp_metric_create (self, obj, "symstate", "Sym. State", METRIC_INTEGER, "enterprises.1991.1.1.4.25.1.1.8", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_metric_enumstr_add (vserv->symstate, 0, "Inactive");
  i_metric_enumstr_add (vserv->symstate, 1, "Active");
  vserv->symprio = l_snmp_metric_create (self, obj, "symprio", "Sym. Priority", METRIC_INTEGER, "enterprises.1991.1.1.4.25.1.1.9", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  /* 
   * Misc
   */

  /* Enqueue the vserv item */
  num = i_list_enqueue (cnt->item_list, vserv);
  if (num != 0)
  { i_printf (1, "v_vserv_objfact_fab failed to enqueue vserv for object %s", obj->name_str); v_vserv_item_free (vserv); return -1; }

  /* Enable virtual port for vserv */
  num = v_vport_enable (self, vserv, index_oidstr);
  if (num != 0)
  { i_printf (1, "v_vserv_objfact_fab warning, failed to enable v_vport for object %s", obj->name_str); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_vserv_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_vserv_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  v_vserv_item *vserv = obj->itemptr;

  if (!vserv) return 0;

  return 0;
}

