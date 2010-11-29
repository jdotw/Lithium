#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/ipregistry.h>
#include <induction/procregistry.h>
#include <induction/postgresql.h>
#include <induction/xml.h>
#include <induction/str.h>

#include "record.h"
#include "procpro.h"

/* Process Profile Object Manipulation */

int l_procpro_object_add (i_resource *self, l_procpro *procpro)
{
  /* Create and register a new object for the procpro */
  i_container *cnt = l_procpro_cnt();
  i_entity_refresh_config refconfig;

  /* Create obj */
  char *name_str;
  asprintf (&name_str, "%li", procpro->id);
  i_object *obj = i_object_create (name_str, procpro->desc_str);
  if (!obj)
  { i_printf (1, "l_procpro_object_add failed to create object"); return -1; }
  obj->itemptr = procpro;
  obj->refresh_func = l_procpro_refresh;

  /* Set refresh config */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  i_entity_refresh_config_loadapply (self, ENTITY(obj), &refconfig);

  /* Register */
  i_entity_register (self, ENTITY(cnt), ENTITY(obj));

  /* Enqueue item */
  i_list_enqueue (cnt->item_list, procpro);
  procpro->obj = obj;

  /* Setup refresh config */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  
  /* 
   * Create default metrics
   */

  procpro->status_met = i_metric_create ("status", "Status", METRIC_INTEGER);
  i_metric_enumstr_add (procpro->status_met, 0, "Not Running");
  i_metric_enumstr_add (procpro->status_met, 1, "Running");
  i_entity_register (self, ENTITY(obj), ENTITY(procpro->status_met));
  i_entity_refresh_config_apply (self, ENTITY(procpro->status_met), &refconfig);

  procpro->count_met = i_metric_create ("count", "Process Count", METRIC_GAUGE);
  procpro->count_met->record_method = RECMETHOD_RRD;
  procpro->count_met->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(procpro->count_met));
  i_entity_refresh_config_apply (self, ENTITY(procpro->count_met), &refconfig);

  procpro->highest_count_met = i_metric_create ("highest_count", "Highest Process Count", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(procpro->highest_count_met));
  i_entity_refresh_config_apply (self, ENTITY(procpro->highest_count_met), &refconfig);

  procpro->mem_total_met = i_metric_create ("mem_total", "Total Memory Usage", METRIC_FLOAT);
  procpro->mem_total_met->record_method = RECMETHOD_RRD;
  procpro->mem_total_met->record_defaultflag = 1;
  procpro->mem_total_met->unit_str = strdup ("bytes");
  procpro->mem_total_met->kbase = 1024;
  procpro->mem_total_met->valstr_func = i_string_volume_metric;
  i_entity_register (self, ENTITY(obj), ENTITY(procpro->mem_total_met));
  i_entity_refresh_config_apply (self, ENTITY(procpro->mem_total_met), &refconfig);

  procpro->mem_maxsingle_met = i_metric_create ("mem_maxsingle", "Max. Single Process Memory Usage", METRIC_FLOAT);
  procpro->mem_maxsingle_met->record_method = RECMETHOD_RRD;
  procpro->mem_maxsingle_met->record_defaultflag = 1;
  procpro->mem_maxsingle_met->unit_str = strdup ("bytes");
  procpro->mem_maxsingle_met->kbase = 1024;
  procpro->mem_maxsingle_met->valstr_func = i_string_volume_metric;
  i_entity_register (self, ENTITY(obj), ENTITY(procpro->mem_maxsingle_met));
  i_entity_refresh_config_apply (self, ENTITY(procpro->mem_maxsingle_met), &refconfig);

  procpro->cpu_total_met = i_metric_create ("cpu_total", "Total CPU Usage", METRIC_FLOAT);
  procpro->cpu_total_met->record_method = RECMETHOD_RRD;
  procpro->cpu_total_met->record_defaultflag = 1;
  procpro->cpu_total_met->unit_str = strdup ("%");
  i_entity_register (self, ENTITY(obj), ENTITY(procpro->cpu_total_met));
  i_entity_refresh_config_apply (self, ENTITY(procpro->cpu_total_met), &refconfig);

  procpro->cpu_maxsingle_met = i_metric_create ("cpu_maxsingle", "Max. Single Process CPU Usage", METRIC_FLOAT);
  procpro->cpu_maxsingle_met->record_method = RECMETHOD_RRD;
  procpro->cpu_maxsingle_met->record_defaultflag = 1;
  procpro->cpu_maxsingle_met->unit_str = strdup ("%");
  i_entity_register (self, ENTITY(obj), ENTITY(procpro->cpu_maxsingle_met));
  i_entity_refresh_config_apply (self, ENTITY(procpro->cpu_maxsingle_met), &refconfig);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  /* Register */
  long msgid = i_procregistry_register (self, obj);
  if (msgid == -1)
  { i_printf (1, "l_procpro_object_add_scriptcb warning, failed to register procpro object %s", obj->name_str); }
  
  /* DO NOT Attempt to do a refresh of the procpro, 
   * you may clash with the swrun refresh or create a 
   * situation where the version of the metrics is = to the
   * version they get when refreshed through swrun but are 
   * infact different values/states 
   */
  
  return 0;
}

int l_procpro_object_update (i_resource *self, l_procpro *procpro)
{
  /* Update an existing procpro object using the info from
   * the procpro specified
   */

  /* Search for procpro object */
  char *id_str;
  asprintf (&id_str, "%li", procpro->id);
  i_container *cnt = l_procpro_cnt();
  i_object *obj = (i_object *) i_entity_child_get (ENTITY(cnt), id_str);
  if (obj)
  {
    /* Update */
    l_procpro *existing = (l_procpro *) obj->itemptr;
    
    /* Deregister with old values */
    long msgid = i_procregistry_deregister (self, self->hierarchy->cust_addr, self->hierarchy->dev_addr, obj);
    if (msgid == -1)
    { i_printf (1, "l_procpro_object_update warning, failed to deregister procpro object %s", obj->name_str); }

    /* Update procpro struct */
    if (existing->desc_str) free (existing->desc_str);
    existing->desc_str = procpro->desc_str ? strdup (procpro->desc_str) : NULL;
    if (existing->match_str) free (existing->match_str);
    existing->match_str = procpro->match_str ? strdup (procpro->match_str) : NULL;
    if (existing->argmatch_str) free (existing->argmatch_str);
    existing->argmatch_str = procpro->argmatch_str ? strdup (procpro->argmatch_str) : NULL;

    /* Update obj struct */
    if (obj->desc_str) free (obj->desc_str);
    obj->desc_str = procpro->desc_str ? strdup (procpro->desc_str) : NULL;
    
    /* Re-register with new */
    msgid = i_procregistry_register (self, obj);
    if (msgid == -1)
    { i_printf (1, "l_procpro_object_update warning, failed to register procpro object %s", obj->name_str); }

    /* DO NOT Attempt to do a refresh of the procpro, 
     * you may clash with the swrun refresh or create a 
     * situation where the version of the metrics is = to the
     * version they get when refreshed through swrun but are 
     * infact different values/states 
     */
  }
  else
  {
    /* Not found */
    i_printf (1, "l_procpro_object_update failed, procpro not found"); 
    return -1; 
  }

  return 0;  
}

int l_procpro_object_remove (i_resource *self, unsigned long id)
{
  /* De-register and free the object belonging to the procpro */

  /* Search for procpro */
  char *id_str;
  asprintf (&id_str, "%li", id);
  i_container *cnt = l_procpro_cnt();
  i_object *obj = (i_object *) i_entity_child_get (ENTITY(cnt), id_str);
  if (obj)
  {
    /* Deregister */
    long msgid = i_procregistry_deregister (self, self->hierarchy->cust_addr, self->hierarchy->dev_addr, obj);
    if (msgid == -1)
    { i_printf (1, "l_procpro_object_remove warning, failed to deregister procpro object %s", obj->name_str); }
    if (obj->itemptr)
    {
      l_procpro *procpro = (l_procpro *) obj->itemptr;
      procpro->obj = NULL;
      l_procpro_free (procpro);
    }
    i_entity_deregister (self, ENTITY(obj));
    i_entity_free (ENTITY(obj));
  }
  else
  {
    i_printf (1, "l_procpro_object_remove procpro %li not found", id); 
    return -1;
  }

  return 0;
}

