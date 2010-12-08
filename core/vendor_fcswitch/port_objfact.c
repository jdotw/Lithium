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

static int curswitch_id = 0;
static int curswitch_domain = 0;

/* 
 * Cisco CPU Resources - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_port_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_port_item *port;

  /* Object Configuration */
  int switch_domain = (int) pdu->variables->val.string[0];
  int switch_port = (int) pdu->variables->val.string[1];
  if (switch_domain != curswitch_domain)
  {
    curswitch_id++;
    curswitch_domain = switch_domain;
  }
  asprintf (&obj->desc_str, "Switch %i Port %i", curswitch_id, switch_port);
  obj->mainform_func = v_port_objform;
  obj->histform_func = v_port_objform_hist;

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
  port->switch_domain = switch_domain;
  port->switch_port = switch_port;

  /* 
   * Metric Creation 
   */


  /* Speed */
  i_entity_refresh_config refconfig;
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  port->speed_kbyte = l_snmp_metric_create (self, obj, "speed_kbye", "Speed (Kbyte)", METRIC_GAUGE, ".1.3.6.1.3.94.1.10.1.15", index_oidstr, RECMETHOD_NONE, 0);
  port->speed_kbyte->unit_str = strdup ("KByte/s");
  port->speed = i_metric_create ("speed", "Speed", METRIC_FLOAT);
  port->speed->unit_str = strdup ("bit");
  port->speed->valstr_func = i_string_rate_metric;
  i_entity_register (self, ENTITY(obj), ENTITY(port->speed));
  i_entity_refresh_config_apply (self, ENTITY(port->speed), &refconfig);
  i_entity_refreshcb_add (ENTITY(port->speed_kbyte), v_port_speed_refcb, port->speed);
            
  /* State */
  port->state = l_snmp_metric_create (self, obj, "state", "State", METRIC_INTEGER, ".1.3.6.1.3.94.1.10.1.6", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (port->state, 1, "Unknown");
  i_metric_enumstr_add (port->state, 2, "Online");
  i_metric_enumstr_add (port->state, 3, "Offline");
  i_metric_enumstr_add (port->state, 4, "Bypassed");
  i_metric_enumstr_add (port->state, 5, "Diagnostic");
  
  /* Status */
  port->status = l_snmp_metric_create (self, obj, "status", "Status", METRIC_INTEGER, ".1.3.6.1.3.94.1.10.1.7", index_oidstr, RECMETHOD_NONE, 0);
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

  /* WWN */
  port->wwn = l_snmp_metric_create (self, obj, "wwn", "WWN", METRIC_HEXSTRING, ".1.3.6.1.3.94.1.10.1.10", index_oidstr, RECMETHOD_NONE, 0);
  
  /* Hardware State */
  port->hwstate = l_snmp_metric_create (self, obj, "hwstate", "Hardware State", METRIC_INTEGER, ".1.3.6.1.3.94.1.10.1.23", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (port->hwstate, 1, "Unknown");
  i_metric_enumstr_add (port->hwstate, 2, "Failed");
  i_metric_enumstr_add (port->hwstate, 3, "Bypassed");
  i_metric_enumstr_add (port->hwstate, 4, "Active");
  i_metric_enumstr_add (port->hwstate, 5, "Loopback");
  i_metric_enumstr_add (port->hwstate, 6, "TX Fault");
  i_metric_enumstr_add (port->hwstate, 7, "No Media");
  i_metric_enumstr_add (port->hwstate, 8, "Link Down");

  /* Elements/Octetes */
  port->octets_in = l_snmp_metric_create (self, obj, "octetes_in", "Receive Element Count", METRIC_COUNT_HEX64, ".1.3.6.1.3.94.4.5.1.7", index_oidstr, RECMETHOD_NONE, 0);  
  port->bps_in = i_metric_acrate_create (self, obj, "bps_in", "Input Bits Per Second", "bit/s", RECMETHOD_RRD, port->octets_in, ACRATE_MOD_BYTETOBIT);
  port->bps_in->record_defaultflag = 1;
  port->octets_out = l_snmp_metric_create (self, obj, "octetes_out", "Transmit Element Count", METRIC_COUNT_HEX64, ".1.3.6.1.3.94.4.5.1.6", index_oidstr, RECMETHOD_NONE, 0);
  port->bps_out = i_metric_acrate_create (self, obj, "bps_out", "Output Bits Per Second", "bit/s", RECMETHOD_RRD, port->octets_out, ACRATE_MOD_BYTETOBIT);
  port->bps_out->record_defaultflag = 1;
  port->utilpc_in = i_metric_acpcent_create (self, obj, "utilpc_in", "Input Utilisation", RECMETHOD_RRD, port->bps_in, port->speed, ACPCENT_REFCB_GAUGE);
  port->utilpc_in->record_defaultflag = 1;
  port->utilpc_out = i_metric_acpcent_create (self, obj, "utilpc_out", "Output Utilisation", RECMETHOD_RRD, port->bps_out, port->speed, ACPCENT_REFCB_GAUGE);
  port->utilpc_out->record_defaultflag = 1;

  /* Errors */
  port->errors = l_snmp_metric_create (self, obj, "errors", "Error Count", METRIC_COUNT_HEX64, ".1.3.6.1.3.94.4.5.1.3", index_oidstr, RECMETHOD_NONE, 0);
  port->eps = i_metric_acrate_create (self, obj, "eps", "Errors Per Second", "err/s", RECMETHOD_RRD, port->errors, ACRATE_MOD_BYTETOBIT);
  
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

  /* Check to see if this is a single-switch */
  if (curswitch_id <= 1)
  {
    /* Re-do iface descriptions */
    i_object *obj;
    for (i_list_move_head(cnt->obj_list); (obj=i_list_restore(cnt->obj_list))!=NULL; i_list_move_next(cnt->obj_list))
    {
      v_port_item *port = obj->itemptr;
      if (obj->desc_str) free (obj->desc_str);
      asprintf (&obj->desc_str, "Port %i", port->switch_port);
    }
  }
  else
  {
    /* This is a multi-unit/stacked switch */
    v_unit_enable (self);
  }

  /* Reset counters */
  curswitch_id = 0;
  curswitch_domain = 0;

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
