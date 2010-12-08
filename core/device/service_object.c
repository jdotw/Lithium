#include <stdlib.h>
#include <string.h>
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
#include <induction/svcregistry.h>
#include <induction/postgresql.h>
#include <induction/xml.h>

#include "record.h"
#include "service.h"

/* Service Object Manipulation */

int l_service_object_add (i_resource *self, l_service *service)
{
  /* Create and register a new object for the service */

  /* Query service script for its object structure */
  l_service_proc *proc = l_service_exec (self, service->script_file, "object", NULL, l_service_object_add_scriptcb, service);
  if (!proc)
  { i_printf (1, "l_service_object_add failed to query script for object structure"); return -1; }

  return 0;
}

int l_service_object_add_scriptcb (i_resource *self, l_service_proc *proc, void *passdata)
{
  l_service *service = (l_service *) passdata;
  i_container *cnt = l_service_cnt();
  i_entity_refresh_config refconfig;

  /* Create obj */
  char *name_str;
  asprintf (&name_str, "%li", service->id);
  i_object *obj = i_object_create (name_str, service->desc_str);
  if (!obj)
  { i_printf (1, "l_service_object_add failed to create object"); return -1; }
  obj->itemptr = service;
  obj->refresh_func = l_service_refresh;

  /* Triggerset List */
  obj->tset_list = i_list_create ();
  if (!obj->tset_list)
  { i_printf (1, "l_service_object_add failed to create triggerset list"); return -1; }

  /* Triggerset Hashtable */
  obj->tset_ht = i_hashtable_create (30);
  if (!obj->tset_ht)
  { i_printf (1, "l_service_object_add failed to create triggerset hashtable"); return -1; }

  /* Set refresh config */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  i_entity_refresh_config_loadapply (self, ENTITY(obj), &refconfig);

  /* Register */
  i_entity_register (self, ENTITY(cnt), ENTITY(obj));

  /* Enqueue item */
  i_list_enqueue (cnt->item_list, service);
  service->obj = obj;

  /* Setup refresh config */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  
  /* 
   * Create default metrics
   */

  service->status_met = i_metric_create ("status", "Status", METRIC_INTEGER);
  i_metric_enumstr_add (service->status_met, 0, "Unknown");
  i_metric_enumstr_add (service->status_met, 1, "Normal");
  i_metric_enumstr_add (service->status_met, 2, "Connection Refused");
  i_metric_enumstr_add (service->status_met, 3, "Connection Timeout");
  i_metric_enumstr_add (service->status_met, 4, "Protocol Timeout");
  i_metric_enumstr_add (service->status_met, 5, "Protocol Error");
  i_metric_enumstr_add (service->status_met, 6, "Script Error");
  i_entity_register (self, ENTITY(obj), ENTITY(service->status_met));
  i_entity_refresh_config_apply (self, ENTITY(service->status_met), &refconfig);

  service->resptime_met = i_metric_create ("resptime", "Response Time", METRIC_FLOAT);
  service->resptime_met->record_method = RECMETHOD_RRD;
  service->resptime_met->record_defaultflag = 1;
  service->resptime_met->unit_str = strdup ("s");
  i_entity_register (self, ENTITY(obj), ENTITY(service->resptime_met));
  i_entity_refresh_config_apply (self, ENTITY(service->resptime_met), &refconfig);

  service->transtime_met = i_metric_create ("transtime", "Transaction Time", METRIC_FLOAT);
  service->transtime_met->record_method = RECMETHOD_RRD;
  service->transtime_met->record_defaultflag = 1;
  service->transtime_met->unit_str = strdup ("s");
  i_entity_register (self, ENTITY(obj), ENTITY(service->transtime_met));
  i_entity_refresh_config_apply (self, ENTITY(service->transtime_met), &refconfig);

  service->version_met = i_metric_create ("version", "Server Version", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(service->version_met));
  i_entity_refresh_config_apply (self, ENTITY(service->version_met), &refconfig);

  service->message_met = i_metric_create ("message", "Message", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(service->message_met));
  i_entity_refresh_config_apply (self, ENTITY(service->message_met), &refconfig);

  /* Check for xml */
  if (proc->output_str && strstr(proc->output_str, "<?xml"))
  {
    /* Break out the parser */
    i_xml *xml = i_xml_struct (proc->output_str, strlen(proc->output_str), "service_script");
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;
    root_node = xmlDocGetRootElement (xml->doc);
    if (root_node)
    {
      /* Iterate through XML nodes */
      for (node = root_node->children; node; node = node->next)
      {
        if (!strcmp((char *)node->name, "metric"))
        {
          /* Process Metric */
          i_metric *met = l_service_metric_fromxml (xml, node);
          if (met) 
          { 
            i_entity_register (self, ENTITY(obj), ENTITY(met)); 
            i_entity_refresh_config_loadapply (self, ENTITY(met), &refconfig);
          }
          continue;
        }
        else if (!strcmp((char *)node->name, "triggerset"))
        {
          /* Triggert Set */
//          i_triggerset *tset = l_service_triggerset_fromxml (xml, node);
        }
      }
    }
    else
    { i_printf (1, "l_service_object_add_scriptcb warning, failed to parse object structure XML"); }
    if (xml) i_xml_free (xml);
  }
  else
  { i_printf (1, "l_service_object_add_scriptcb warning, script did not return valid object structure xml"); }

  /* 
   * Add default triggersets 
   */

  /* Operational State */
  i_triggerset *tset = i_triggerset_create ("status", "Status", "status");
  i_triggerset_addtrg (self, tset, "connrefused", "Connection Refused", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_FATAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "conntimeout", "Connection Timeout", VALTYPE_INTEGER, TRGTYPE_EQUAL, 3, NULL, 0, NULL, 0, ENTSTATE_FATAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "prototimeout", "Protocol Timeout", VALTYPE_INTEGER, TRGTYPE_EQUAL, 4, NULL, 0, NULL, 0, ENTSTATE_FATAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "protoerror", "Protocol Error", VALTYPE_INTEGER, TRGTYPE_EQUAL, 5, NULL, 0, NULL, 0, ENTSTATE_FATAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "checkerror", "Script Error", VALTYPE_INTEGER, TRGTYPE_EQUAL, 6, NULL, 0, NULL, 0, ENTSTATE_FATAL, TSET_FLAG_VALAPPLY);
  i_triggerset_assign_obj (self, obj, tset);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  /* Register */
  long msgid = i_svcregistry_register (self, obj);
  if (msgid == -1)
  { i_printf (1, "l_service_object_add_scriptcb warning, failed to register service object %s", obj->name_str); }
  
  return 0;
}

int l_service_object_update (i_resource *self, l_service *service)
{
  /* Update an existing service object using the info from
   * the service specified
   */

  /* Search for service object */
  char *id_str;
  asprintf (&id_str, "%li", service->id);
  i_object *obj;
  i_container *cnt = l_service_cnt();
  for (i_list_move_head(cnt->obj_list); (obj=i_list_restore(cnt->obj_list))!=NULL; i_list_move_next(cnt->obj_list))
  {
    if (!strcmp(obj->name_str, id_str)) break;
  }

  if (obj)
  {
    long msgid;

    /* Deregister with old values */
    msgid = i_svcregistry_deregister (self, self->hierarchy->cust_addr, self->hierarchy->dev_addr, obj);
    if (msgid == -1)
    { i_printf (1, "l_service_object_update warning, failed to deregister service object %s", obj->name_str); }
    
    /* Update */
    l_service *existing = obj->itemptr;
    if (existing->desc_str) { free (existing->desc_str); existing->desc_str = NULL; }
    if (service->desc_str) existing->desc_str = strdup (service->desc_str);
    if (obj->desc_str) { free (obj->desc_str); obj->desc_str = NULL; }
    if (service->desc_str) obj->desc_str = strdup (service->desc_str);
    if (existing->script_file) { free (existing->script_file); existing->script_file = NULL; }
    if (service->script_file) existing->script_file = strdup (service->script_file);
    existing->enabled = service->enabled;

    /* Re-register with new */
    msgid = i_svcregistry_register (self, obj);
    if (msgid == -1)
    { i_printf (1, "l_service_object_update warning, failed to register service object %s", obj->name_str); }
  }
  else
  {
    /* Not fouind */
    i_printf (1, "l_service_object_update failed, service not found"); 
    return -1; 
  }

  return 0;  
}

int l_service_object_remove (i_resource *self, unsigned long id)
{
  /* De-register and free the object belonging to the service */

  /* Search for service */
  char *id_str;
  asprintf (&id_str, "%li", id);
  i_object *obj;
  i_container *cnt = l_service_cnt();
  for (i_list_move_head(cnt->obj_list); (obj=i_list_restore(cnt->obj_list))!=NULL; i_list_move_next(cnt->obj_list))
  {
    if (!strcmp(obj->name_str, id_str)) break;
  }

  /* Remove */
  if (obj)
  {
    /* Deregister */
    long msgid = i_svcregistry_deregister (self, self->hierarchy->cust_addr, self->hierarchy->dev_addr, obj);
    if (msgid == -1)
    { i_printf (1, "l_service_object_remove warning, failed to deregister service object %s", obj->name_str); }
    
    if (obj->itemptr)
    {
      l_service *service = (l_service *) obj->itemptr;
      service->obj = NULL;
      l_service_free (service);
    }
    i_entity_deregister (self, ENTITY(obj));
    i_entity_free (ENTITY(obj));
  }
  else
  {
    i_printf (1, "l_service_object_remove service %li not found", id); 
    return -1;
  }

  return 0;
}

