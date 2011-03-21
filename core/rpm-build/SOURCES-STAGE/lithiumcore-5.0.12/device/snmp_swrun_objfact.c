#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/interface.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>

#include "snmp.h"
#include "snmp_swrun.h"

/* 
 * SNMP Running Software - Object Factory Functions 
 */

/* Object Factory Fabrication */

int l_snmp_swrun_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  l_snmp_swrun *swrun;
  i_entity_refresh_config refconfig;

  /* Object Configuration */
  obj->desc_str = strdup (obj->name_str);

  /* Configure refresh (PARENT) 
   * Because the process objects are transient in nature,
   * there's no need to attempt to load a config from 
   * the SQL db. The fixed config of REFMETHOD_PARENT 
   * is applied for all process objects
   */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  i_entity_refresh_config_apply (self, ENTITY(obj), &refconfig);

  /* Create process struct */
  swrun = l_snmp_swrun_create ();
  if (!swrun)
  { i_printf (1, "l_snmp_swrun_objfact_fab failed to create swrun item for object %s", obj->name_str); return -1; }
  obj->itemptr = swrun;
  swrun->obj = obj;
  swrun->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];
  if (swrun->index == 0)
  {
    /* Some SNMP implementations illegally use index 0.
     * Index 0 will NOT resolve against the MIB because the 
     * mib defines an index range of 1+. Hence, a blank object
     * is entered as a 'place holder'
     */
    l_snmp_swrun_free (swrun);
    obj->itemptr = NULL;
    return 0;
  }

  /* 
   * Metric Creation 
   */

  /* System Uptime */
  swrun->procname = l_snmp_metric_create (self, obj, "procname", "Process Name", METRIC_STRING, "hrSWRunName", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  swrun->path = l_snmp_metric_create (self, obj, "path", "Path", METRIC_STRING, "hrSWRunPath", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  swrun->params = l_snmp_metric_create (self, obj, "params", "Parameters", METRIC_STRING, "hrSWRunParameters", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  swrun->type = l_snmp_metric_create (self, obj, "type", "Type", METRIC_INTEGER, "hrSWRunType", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_metric_enumstr_add (swrun->type, 1, "Unknown");
  i_metric_enumstr_add (swrun->type, 2, "Operating System");
  i_metric_enumstr_add (swrun->type, 3, "Device Driver");
  i_metric_enumstr_add (swrun->type, 4, "Application");
  swrun->status = l_snmp_metric_create (self, obj, "status", "Status", METRIC_INTEGER, "hrSWRunStatus", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_metric_enumstr_add (swrun->status, 1, "Running");
  i_metric_enumstr_add (swrun->status, 1, "Runnable");
  i_metric_enumstr_add (swrun->status, 1, "Sleeping");
  i_metric_enumstr_add (swrun->status, 1, "Invalid");
  swrun->cpu_csec = l_snmp_metric_create (self, obj, "cpu_csec", "CPU Centi-seconds", METRIC_COUNT, "hrSWRunPerfCPU", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  swrun->cpu_pc = i_metric_acrate_create (self, obj, "cpu_pc", "CPU Utilization", "%", RECMETHOD_NONE, swrun->cpu_csec, 0);
  swrun->cpu_pc->record_defaultflag = 1;
  swrun->mem = l_snmp_metric_create (self, obj, "mem", "Memory Allocation", METRIC_GAUGE, "hrSWRunPerfMem", index_oidstr, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  swrun->mem->alloc_unit = 1024;
  swrun->mem->valstr_func = i_string_volume_metric;
  swrun->mem->unit_str = strdup ("bytes");
  swrun->mem->kbase = 1024;
  
  /*
   * End Metric Creation
   */

  /* Enqueue the swrun item */
  num = i_list_enqueue (cnt->item_list, swrun);
  if (num != 0)
  { i_printf (1, "l_snmp_swrun_objfact_fab failed to enqueue swrun for object %s", obj->name_str); l_snmp_swrun_free (swrun); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int l_snmp_swrun_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
{
  /* Check the result */
  if (result == SNMP_ERROR_NOERROR)
  {
    /* No errors, set item list state to NORMAL */
    cnt->item_list_state = ITEMLIST_STATE_NORMAL;
  }

  /* Refresh container */
  i_entity_refresh (self, ENTITY(cnt), REFFLAG_AUTO, NULL, NULL);

  return 0;
}

/* Object Factory Clean Func
 *
 * Called when an object is obsolete prior to it being deregistered and free
 */

int l_snmp_swrun_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  l_snmp_swrun *swrun = obj->itemptr;

  /* Remove from item list */
  num = i_list_search (cnt->item_list, swrun);
  if (num == 0) 
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;

  /* Free item */
  l_snmp_swrun_free (swrun);

  return 0;
}
