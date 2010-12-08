#include <stdlib.h> 
#include <string.h> 
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/entity.h>
#include <induction/cement.h>
#include <induction/metric.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/entity_xml.h>
#include <induction/postgresql.h>
#include <induction/str.h>
#include <induction/xml.h>

#include "service.h"

/*
 * Service Triggerset Functions
 */

i_triggerset* l_service_triggerset_fromxml (i_resource *self, i_object *obj, i_xml *xml, xmlNodePtr tset_node)
{
  xmlNodePtr node;
  char *name_str = NULL;
  char *desc_str = NULL;
  char *metric_str = NULL;
  i_triggerset *tset = NULL;

  /* Parse tree */
  for (node = tset_node->children; node; node = node->next)
  {
    if (!strcmp((char *)node->name, "trigger"))
    {
      l_service_trigger_fromxml (self, tset, xml, node);
      continue;
    }
    
    char *str;
    str = (char *) xmlNodeListGetString (xml->doc, node->xmlChildrenNode, 1);
    if (!str) continue;

    /* Pre-creation variables */
    if (!strcmp((char *)node->name, "name") && str) name_str = strdup (str);
    else if (!strcmp((char *)node->name, "desc") && str) desc_str = strdup (str);
    else if (!strcmp((char *)node->name, "metric") && str) metric_str = strdup (str);
    
    /* Create tset */
    if (!tset && name_str && desc_str && metric_str)
    {
      tset = i_triggerset_create (name_str, desc_str, metric_str);
      if (!tset)
      { i_printf (1, "l_service_triggerset_fromxml failed to create tset"); break; }
      
      if (name_str) free (name_str);
      if (desc_str) free (desc_str);
      if (metric_str) free (metric_str);
      name_str = NULL;
      desc_str = NULL;
      metric_str = NULL;
    }

    xmlFree (str);
  }

  return tset;
} 

