#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/socket.h>
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
#include <induction/postgresql.h>
#include <induction/path.h>
#include <induction/xml.h>

#include "service.h"

/*
 * Service script-based refreshing
 */

int l_service_refresh (i_resource *self, i_object *obj, int opcode)
{
  l_service_proc *proc;
  l_service *service = obj->itemptr;
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh process */
      proc = l_service_exec (self, service->script_file, "check", service, l_service_refresh_procb, obj);
      if (!proc) { i_printf (1, "l_service_refresh failed to commence service check for %s @ %s", obj->desc_str, self->hierarchy->dev->name_str); return 1; }    
      break;

    case REFOP_COLLISION:   /* Handle collision */
      break;

    case REFOP_TERMINATE:   /* Terminate the refresh */
      break;

    case REFOP_CLEANDATA:   /* Cleanup persistent refresh data */
      break;
  }

  return 0;
}

int l_service_refresh_procb (i_resource *self, l_service_proc *proc, void *passdata)
{
  /* Called with the output from the 'check' operation.
   *
   * This will be the xml struct with relevant metric values
   */
  i_object *obj = passdata;

  /* Check for xml */
  if (!proc->output_str || !strstr(proc->output_str, "<?xml"))
  { 
    i_printf (1, "l_service_refresh_procb no xml header found in script output"); 

    /* Invalidate metrics */
    l_service_refresh_invalidate_all (self, obj); 
    obj->refresh_result = REFRESULT_TOTAL_FAIL;

    /* Set Status */
    i_metric *met = (i_metric *) i_entity_child_get (ENTITY(obj), "status");
    if (met)
    {
      i_metric_value *val = i_metric_value_create ();
      i_metric_valstr_set (met, val, "6");
      i_metric_value_enqueue (self, met, val);
      met->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(met));
    }
    
    /* Set Message */
    met = (i_metric *) i_entity_child_get (ENTITY(obj), "message");
    if (met)
    {
      i_metric_value *val = i_metric_value_create ();
      i_metric_valstr_set (met, val, "Script did not return XML");
      i_metric_value_enqueue (self, met, val);
      met->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(met));
    }
    
    /* Terminate refresh */
    i_entity_refresh_terminate (ENTITY(obj));
    
    return -1; 
  }

  /* Break out the parser */
  i_xml *xml = i_xml_struct (proc->output_str, strlen(proc->output_str), "service_script");
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;
  root_node = xmlDocGetRootElement (xml->doc);
  if (!root_node)
  { 
    i_printf (1, "l_service_refresh_procb failed to parse XML"); 

    /* Invalidate metrics */
    l_service_refresh_invalidate_all (self, obj); 
    obj->refresh_result = REFRESULT_TOTAL_FAIL;

    /* Set Status */
    i_metric *met = (i_metric *) i_entity_child_get (ENTITY(obj), "status");
    if (met)
    {
      i_metric_value *val = i_metric_value_create ();
      i_metric_valstr_set (met, val, "6");
      i_metric_value_enqueue (self, met, val);
      met->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(met));
    }
    
    /* Set Message */
    met = (i_metric *) i_entity_child_get (ENTITY(obj), "message");
    if (met)
    {
      i_metric_value *val = i_metric_value_create ();
      i_metric_valstr_set (met, val, "Failed to parse script XML output");
      i_metric_value_enqueue (self, met, val);
      met->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(met));
    }
    
    /* Terminate refresh */
    i_entity_refresh_terminate (ENTITY(obj));
    
    return -1; 
  }

  obj->refresh_result = REFRESULT_OK;

  /* Iterate through XML nodes */
  for (node = root_node->children; node; node = node->next)
  {
    if (!strcmp((char *)node->name, "metric"))
    {
      xmlNodePtr met_node;
      i_metric *met = NULL;
      for (met_node = node->children; met_node; met_node = met_node->next)
      {
        char *str = (char *) xmlNodeListGetString (xml->doc, met_node->xmlChildrenNode, 1);
        if (!strcmp((char *)met_node->name, "name") && str)
        {
          /* Locate metric */
          met = (i_metric *) i_entity_child_get (ENTITY(obj), str);
          if (!met)
          { 
            i_printf (1, "l_service_refresh_procb failed to find metric %s", str); 
            xmlFree (str);
            break; 
          }
        }
        else if (!strcmp((char *)met_node->name, "value") && str)
        {
          /* Enqueue new value record */
          i_metric_value *val = i_metric_value_create ();
          i_metric_valstr_set (met, val, str);
          i_metric_value_enqueue (self, met, val); 
          met->refresh_result = REFRESULT_OK;
          i_entity_refresh_terminate (ENTITY(met));
        }
        if (str) xmlFree (str);
      }
    }
  }

  i_xml_free (xml);
  
  i_entity_refresh_terminate (ENTITY(obj));

  return 0;
}


int l_service_refresh_invalidate_all (i_resource *self, i_object *obj)
{
  i_metric *met;
  for (i_list_move_head(obj->met_list); (met=i_list_restore(obj->met_list))!=NULL; i_list_move_next(obj->met_list))
  {
    if (strcmp(met->name_str, "status") != 0 && strcmp(met->name_str, "message") != 0)
    {
      met->refresh_result = REFRESULT_TOTAL_FAIL;
      i_entity_refresh_terminate (ENTITY(met));
    }
  }
  return 0;
}
