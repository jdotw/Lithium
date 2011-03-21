#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/form.h"
#include "induction/navtree.h"
#include "induction/hierarchy.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/value.h"
#include "device/snmp.h"
#include "device/record.h"

#include "fanps.h"

/* Allied Telesyn Fan and Power Supply */

static int static_enabled = 0;
static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Pointer retrieval */

int v_fanps_enabled ()
{ return static_enabled; }

i_container* v_fanps_cnt ()
{ return static_cnt; }

i_object* v_fanps_obj ()
{ return static_obj; }

/* Enable / Disable */

int v_fanps_enable (i_resource *self)
{
  int num;
  v_fanps_item *fanps;
  i_triggerset *tset;
  static i_entity_refresh_config defrefconfig;
  
  if (static_enabled == 1)
  { i_printf (1, "v_fanps_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("fanps", "Fan and Power Supply");
  if (!static_cnt)
  { i_printf (1, "v_fanps_enable failed to create container"); v_fanps_disable (self); return -1; }
  static_cnt->mainform_func = v_fanps_cntform;
  static_cnt->sumform_func = v_fanps_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register container */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_fanps_enable failed to register container"); v_fanps_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_fanps_enable failed to load and apply container refresh config"); v_fanps_disable (self); return -1; }

  /* 
   * Trigger sets 
   */

  tset = i_triggerset_create ("mainpsu_status", "Main PSU Status", "mainpsu_status");
  i_triggerset_addtrg (self, tset, "faulty", "Faulty", VALTYPE_INTEGER, TRGTYPE_EQUAL, 3, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  tset = i_triggerset_create ("rpsconn_status", "RPS Connection Status", "rpsconn_status");
  i_triggerset_addtrg (self, tset, "not_connected", "Not Connected", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("redundantpsu_status", "RPS Status", "redundantpsu_status");
  i_triggerset_addtrg (self, tset, "off", "Off", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("mainfan_status", "Main Fan Status", "mainfan_status");
  i_triggerset_addtrg (self, tset, "not_running", "Not Running", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_INTEGER, TRGTYPE_EQUAL, 3, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("redundantpsu_status", "Redundant Fan Status", "redundantfan_status");
  i_triggerset_addtrg (self, tset, "not_running", "Not Running", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("temperature_status", "Temperature Status", "temperature_status");
  i_triggerset_addtrg (self, tset, "out_of_range", "Out of operating range", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("fantray_status", "Fan Tray Status", "fantray_status");
  i_triggerset_addtrg (self, tset, "failed", "Failed", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  tset = i_triggerset_create ("mainmonitoring_status", "Main Monitoring Status", "mainmonitoring_status");
  i_triggerset_addtrg (self, tset, "failed", "Failed", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  tset = i_triggerset_create ("accelfan_status", "Accelerator Cards Fan Status", "accelfan_status");
  i_triggerset_addtrg (self, tset, "failed", "Failed", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  /*
   * Master object
   */
  
  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_fanps_enable failed to create item_list"); v_fanps_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "v_fanps_enable failed to create master object"); v_fanps_disable (self); return -1; }
  static_obj->mainform_func = v_fanps_objform;
  static_obj->navtree_expand = NAVTREE_EXP_ALWAYS;

  /* Create fanps item */
  fanps = v_fanps_item_create ();
  if (!fanps)
  { i_printf (1, "v_fanps_enable failed to create static_fanps struct"); v_fanps_disable (self); return -1; }
  fanps->obj = static_obj;
  static_obj->itemptr = fanps;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, fanps);
  if (num != 0)
  { i_printf (1, "v_fanps_enable failed to enqueue item into static_cnt->item_list"); v_fanps_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  fanps->mainpsu_status = l_snmp_metric_create (self, static_obj, "mainpsu_status", "Main Power Supply Status", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.3.1.2", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fanps->mainpsu_status, 1, "On");
  i_metric_enumstr_add (fanps->mainpsu_status, 2, "Off");
  i_metric_enumstr_add (fanps->mainpsu_status, 3, "FAULTY");

  fanps->rpsconn_status = l_snmp_metric_create (self, static_obj, "rpsconn_status", "RPS Connection Status", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.3.1.1", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fanps->rpsconn_status, 0, "Not Supported");
  i_metric_enumstr_add (fanps->rpsconn_status, 1, "Connected");
  i_metric_enumstr_add (fanps->rpsconn_status, 2, "Not Connected");
  i_metric_enumstr_add (fanps->rpsconn_status, 3, "Not Monitored");

  fanps->redundantpsu_status = l_snmp_metric_create (self, static_obj, "redundantpsu_status", "RPS Status", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.3.1.3", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fanps->redundantpsu_status, 0, "Not Supported");
  i_metric_enumstr_add (fanps->redundantpsu_status, 1, "On");
  i_metric_enumstr_add (fanps->redundantpsu_status, 2, "Off");
  i_metric_enumstr_add (fanps->redundantpsu_status, 3, "Not Monitored");

  fanps->rpsmonitoring_status = l_snmp_metric_create (self, static_obj, "rpsmonitoring_status", "RPS Monitoring Status", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.3.1.4", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fanps->rpsmonitoring_status, 0, "Not Supported");
  i_metric_enumstr_add (fanps->rpsmonitoring_status, 1, "On");
  i_metric_enumstr_add (fanps->rpsmonitoring_status, 2, "Off");

  fanps->mainfan_status = l_snmp_metric_create (self, static_obj, "mainfan_status", "Main Fan Status", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.3.1.5", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fanps->mainfan_status, 0, "Not Supported");
  i_metric_enumstr_add (fanps->mainfan_status, 1, "OK");
  i_metric_enumstr_add (fanps->mainfan_status, 2, "Not Running");
  i_metric_enumstr_add (fanps->mainfan_status, 3, "Warning");

  fanps->redundantfan_status = l_snmp_metric_create (self, static_obj, "redundantfan_status", "Redundant Fan Status", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.3.1.6", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fanps->redundantfan_status, 0, "Not Supported");
  i_metric_enumstr_add (fanps->redundantfan_status, 1, "OK");
  i_metric_enumstr_add (fanps->redundantfan_status, 2, "Not Running");
  i_metric_enumstr_add (fanps->redundantfan_status, 3, "Not Monitored");

  fanps->temperature_status = l_snmp_metric_create (self, static_obj, "temperature_status", "Temperature Status", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.3.1.7", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fanps->temperature_status, 1, "OK");
  i_metric_enumstr_add (fanps->temperature_status, 2, "Out of operating range");

  fanps->fantray_present = l_snmp_metric_create (self, static_obj, "fantray_present", "Fan Tray Present", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.3.1.8", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fanps->fantray_present, 0, "Not Supported");
  i_metric_enumstr_add (fanps->fantray_present, 1, "Present");
  i_metric_enumstr_add (fanps->fantray_present, 2, "Not Present");

  fanps->fantray_status = l_snmp_metric_create (self, static_obj, "fantray_status", "Fan Tray Status", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.3.1.9", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fanps->fantray_status, 0, "Not Supported");
  i_metric_enumstr_add (fanps->fantray_status, 1, "OK");
  i_metric_enumstr_add (fanps->fantray_status, 2, "FAILED");

  fanps->mainmonitoring_status = l_snmp_metric_create (self, static_obj, "mainmonitoring_status", "Main Monitoring Status", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.3.1.10", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fanps->mainmonitoring_status, 0, "Not Supported");
  i_metric_enumstr_add (fanps->mainmonitoring_status, 1, "OK");
  i_metric_enumstr_add (fanps->mainmonitoring_status, 2, "FAILED");

  fanps->accelfan_status = l_snmp_metric_create (self, static_obj, "accelfan_status", "Accelerator Cards Fan Status", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.3.1.12", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fanps->accelfan_status, 0, "Not Supported");
  i_metric_enumstr_add (fanps->accelfan_status, 1, "OK");
  i_metric_enumstr_add (fanps->accelfan_status, 2, "FAILED");

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);
  
  return 0;
}

int v_fanps_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "v_fanps_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  if (static_obj)
  { 
    v_fanps_item *item = static_obj->itemptr;
    if (item) { v_fanps_item_free (item); }
    i_entity_deregister (self, ENTITY(static_obj));
    i_entity_free (ENTITY(static_obj));
    static_obj = NULL;
  }

  if (static_cnt)
  {
    i_entity_deregister (self, ENTITY(static_cnt));
    i_entity_free (ENTITY(static_cnt));
    static_cnt = NULL;
  }

  return 0;
}

