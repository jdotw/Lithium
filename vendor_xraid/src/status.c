#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>
#include <lithium/record.h>
#include <lithium/snmp.h>

#include "status.h"

/* Xraid Status Information Sub-System */

static i_container *static_cnt = NULL;
static v_status_item *static_top_item = NULL;
static v_status_item *static_bottom_item = NULL;

/* Variable Retrieval */

i_container* v_status_cnt ()
{ return static_cnt; }

v_status_item* v_status_static_top_item ()
{ return static_top_item; }

v_status_item* v_status_static_bottom_item ()
{ return static_bottom_item; }

/* Enable / Disable */

int v_status_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("xrstatus", "Controller Status");
  if (!static_cnt)
  { i_printf (1, "v_status_enable failed to create container"); v_status_disable (self); return -1; }
  static_cnt->mainform_func = v_status_cntform;
  static_cnt->sumform_func = v_status_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_status_enable failed to register container"); v_status_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_status_enable failed to load and apply container refresh config"); v_status_disable (self); return -1; }

  /*
   * Triggers
   */
  i_triggerset *tset;

  /* Drive Temp */
  tset = i_triggerset_create ("drivetemp", "Drive Temp", "drivetemp");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_STRING, TRGTYPE_NOTEQUAL, 0, "ok", 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* Blower Temp */
  tset = i_triggerset_create ("blowertemp", "Blower Temp", "blowertemp");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_STRING, TRGTYPE_NOTEQUAL, 0, "ok", 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* RAID Temp */
  tset = i_triggerset_create ("raidtemp", "RAID Controller Temp", "raidtemp");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_STRING, TRGTYPE_NOTEQUAL, 0, "ok", 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* Blower Speed */
  tset = i_triggerset_create ("blowerspeed", "Blower Speed", "blowerspeed");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_STRING, TRGTYPE_NOTEQUAL, 0, "ok", 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* EMU Temp */
  tset = i_triggerset_create ("emutemp", "EMU Temp", "emutemp");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_STRING, TRGTYPE_NOTEQUAL, 0, "ok", 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* Blower */
  tset = i_triggerset_create ("blower", "Blower", "blower");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_STRING, TRGTYPE_NOTEQUAL, 0, "ok", 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* SMART */
  tset = i_triggerset_create ("smart", "SMART Status", "smart");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_STRING, TRGTYPE_NOTEQUAL, 0, "ok", 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* RAID Comms */
  tset = i_triggerset_create ("raidcomms", "RAID Comms", "raidcomms");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_STRING, TRGTYPE_NOTEQUAL, 0, "ok", 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* Ambient Temp */
  tset = i_triggerset_create ("ambientstate", "Ambient State", "ambientstate");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_STRING, TRGTYPE_NOTEQUAL, 0, "ok", 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  /* Battery */
  tset = i_triggerset_create ("battery", "Battery", "battery");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_STRING, TRGTYPE_NOTEQUAL, 0, "ok", 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  /* Buzzer */
  tset = i_triggerset_create ("buzzer", "Buzzer", "buzzer");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_STRING, TRGTYPE_NOTEQUAL, 0, "off", 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  /* Power State */
  tset = i_triggerset_create ("powerstate", "Power State", "powerstate");
  i_triggerset_addtrg (self, tset, "off", "Off", VALTYPE_INTEGER, TRGTYPE_EQUAL, 0, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  /* UPS Battery Low */
  tset = i_triggerset_create ("upsbatterylow", "UPS Battery", "batterylow");
  i_triggerset_addtrg (self, tset, "inuse", "In Use", VALTYPE_INTEGER, TRGTYPE_EQUAL, 1, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  /* UPS Battery In Use */
  tset = i_triggerset_create ("upsbatteryinuse", "UPS Battery", "batteryinuse");
  i_triggerset_addtrg (self, tset, "low", "Low", VALTYPE_INTEGER, TRGTYPE_EQUAL, 1, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  /* UPS Line Power Down */
  tset = i_triggerset_create ("upslinepower", "UPS Line Power", "linepowerdown");
  i_triggerset_addtrg (self, tset, "down", "Down", VALTYPE_INTEGER, TRGTYPE_EQUAL, 1, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  /* 
   * Items and objects
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_status_enable failed to create item_list"); v_status_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create top and bottom items */
  int i;
  for (i=0; i < 2; i++)
  {  
    /* Create table object */
    if (i == 0)
    { obj = i_object_create ("upper", "Upper Controller"); }
    else
    { obj = i_object_create ("lower", "Lower Controller"); }
    
    obj->mainform_func = v_status_objform; 
    obj->histform_func = v_status_objform_hist; 

    /* Register table object */
    i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

    /* Create item */
    v_status_item *item;
    item = v_status_item_create ();
    item->obj = obj;
    obj->itemptr = item;
    if (i == 0) static_top_item = item;
    else static_bottom_item = item;
    i_list_enqueue (static_cnt->item_list, item);

    /*
     * Create Metrics 
     */ 
  
    memset (&refconfig, 0, sizeof(i_entity_refresh_config));
    refconfig.refresh_method = REFMETHOD_EXTERNAL;
    refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
    refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

    item->powerstate = i_metric_create ("powerstate", "Power State", METRIC_INTEGER);
    i_metric_enumstr_add (item->powerstate, 0, "Off");
    i_metric_enumstr_add (item->powerstate, 1, "On");
    i_entity_register (self, ENTITY(obj), ENTITY(item->powerstate));
    i_entity_refresh_config_apply (self, ENTITY(item->powerstate), &refconfig);

    item->ambient_temp = i_metric_create ("ambient_temp", "Ambient Temperature", METRIC_GAUGE);
    item->ambient_temp->unit_str = strdup ("deg.C");
    item->ambient_temp->record_method = RECMETHOD_RRD;
    item->ambient_temp->record_defaultflag = 1;
    i_entity_register (self, ENTITY(obj), ENTITY(item->ambient_temp));
    i_entity_refresh_config_apply (self, ENTITY(item->ambient_temp), &refconfig);

    item->batterylow = i_metric_create ("batterylow", "Battery Low", METRIC_INTEGER);
    i_metric_enumstr_add (item->batterylow, -1, "Unknown");
    i_metric_enumstr_add (item->batterylow, 0, "False");
    i_metric_enumstr_add (item->batterylow, 1, "True");
    i_entity_register (self, ENTITY(obj), ENTITY(item->batterylow));
    i_entity_refresh_config_apply (self, ENTITY(item->batterylow), &refconfig);

    item->batteryinuse = i_metric_create ("batteryinuse", "Battery in Use", METRIC_INTEGER);
    i_metric_enumstr_add (item->batteryinuse, -1, "Unknown");
    i_metric_enumstr_add (item->batteryinuse, 0, "False");
    i_metric_enumstr_add (item->batteryinuse, 1, "True");
    i_entity_register (self, ENTITY(obj), ENTITY(item->batteryinuse));
    i_entity_refresh_config_apply (self, ENTITY(item->batteryinuse), &refconfig);

    item->linepowerdown = i_metric_create ("linepowerdown", "Line Power Down", METRIC_INTEGER);
    i_metric_enumstr_add (item->linepowerdown, -1, "Unknown");
    i_metric_enumstr_add (item->linepowerdown, 0, "False");
    i_metric_enumstr_add (item->linepowerdown, 1, "True");
    i_entity_register (self, ENTITY(obj), ENTITY(item->linepowerdown));
    i_entity_refresh_config_apply (self, ENTITY(item->linepowerdown), &refconfig);
  
    item->raidcont_state = i_metric_create ("raidcont_state", "RAID Controller", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->raidcont_state));
    i_entity_refresh_config_apply (self, ENTITY(item->raidcont_state), &refconfig);

//    item->powersupply = i_metric_create ("powersupply", "Power Supply", METRIC_STRING);
//    i_entity_register (self, ENTITY(obj), ENTITY(item->powersupply));
//    i_entity_refresh_config_apply (self, ENTITY(item->powersupply), &refconfig);

//    item->raidset = i_metric_create ("raidset", "RAID Set", METRIC_STRING);
//    i_entity_register (self, ENTITY(obj), ENTITY(item->raidset));
//    i_entity_refresh_config_apply (self, ENTITY(item->raidset), &refconfig);

    item->drivetemp = i_metric_create ("drivetemp", "Drive Temperature", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->drivetemp));
    i_entity_refresh_config_apply (self, ENTITY(item->drivetemp), &refconfig);

    item->blowertemp = i_metric_create ("blowertemp", "Blower Temperature", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->blowertemp));
    i_entity_refresh_config_apply (self, ENTITY(item->blowertemp), &refconfig);

    item->raidtemp = i_metric_create ("raidtemp", "RAID Temperature", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->raidtemp));
    i_entity_refresh_config_apply (self, ENTITY(item->raidtemp), &refconfig);

    item->blowerspeed = i_metric_create ("blowerspeed", "Blower Speed", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->blowerspeed));
    i_entity_refresh_config_apply (self, ENTITY(item->blowerspeed), &refconfig);

    item->emutemp = i_metric_create ("emutemp", "EMU Temperature", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->emutemp));
    i_entity_refresh_config_apply (self, ENTITY(item->emutemp), &refconfig);

    item->smart = i_metric_create ("smart", "SMART Status", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->smart));
    i_entity_refresh_config_apply (self, ENTITY(item->smart), &refconfig);

    item->blower = i_metric_create ("blower", "Blower", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->blower));
    i_entity_refresh_config_apply (self, ENTITY(item->blower), &refconfig);

    item->raidcomms = i_metric_create ("raidcomms", "RAID Communication", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->raidcomms));
    i_entity_refresh_config_apply (self, ENTITY(item->raidcomms), &refconfig);

    item->ambientstate = i_metric_create ("ambientstate", "Ambient Temperature Level", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->ambientstate));
    i_entity_refresh_config_apply (self, ENTITY(item->ambientstate), &refconfig);

    item->battery = i_metric_create ("battery", "Battery", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->battery));
    i_entity_refresh_config_apply (self, ENTITY(item->battery), &refconfig);

    item->serviceid = i_metric_create ("serviceid", "Service Identifier", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->serviceid));
    i_entity_refresh_config_apply (self, ENTITY(item->serviceid), &refconfig);

    item->buzzer = i_metric_create ("buzzer", "buzzer", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->buzzer));
    i_entity_refresh_config_apply (self, ENTITY(item->buzzer), &refconfig);

    item->xsyncstate = i_metric_create ("xsyncstate", "xsyncstate", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->xsyncstate));
    i_entity_refresh_config_apply (self, ENTITY(item->xsyncstate), &refconfig);

    /* Evaluate apprules for all triggersets */
    i_triggerset_evalapprules_allsets (self, obj);

    /* Evaluate recrules for all metrics */
    l_record_eval_recrules_obj (self, obj);
  }

  return 0;
}

int v_status_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

