#include <stdlib.h> 
#include <string.h> 
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/entity.h>
#include <induction/cement.h>
#include <induction/metric.h>
#include <induction/object.h>
#include <induction/trigger.h>
#include <induction/entity_xml.h>
#include <induction/postgresql.h>
#include <induction/str.h>
#include <induction/xml.h>

#include "service.h"

/*
 * Service Metric Functions
 */

i_metric* l_service_metric_fromxml (i_resource *self, i_object *obj, i_xml *xml, xmlNodePtr met_node)
{
  xmlNodePtr node;
  char *name_str = NULL;
  char *desc_str = NULL;
  char *type_str = NULL;
  i_metric *met = NULL;

  /* Parse tree */
  for (node = met_node->children; node; node = node->next)
  {
    char *str;
    str = (char *) xmlNodeListGetString (xml->doc, node->xmlChildrenNode, 1);
    if (!str) continue;

    /* Pre-creation variables */
    if (!strcmp((char *)node->name, "name") && str) name_str = strdup (str);
    else if (!strcmp((char *)node->name, "desc") && str) desc_str = strdup (str);
    else if (!strcmp((char *)node->name, "type") && str) type_str = strdup (str);

    if (!met && name_str && desc_str && type_str)
    {
      /* Create metric */
      met = i_metric_create (name_str, desc_str, atoi (type_str));
      if (!met)
      { i_printf (1, "l_service_metric_fromxml failed to create metric"); break; }
      free (name_str);
      free (desc_str);
      free (type_str);
      name_str = NULL;
      desc_str = NULL;
      type_str = NULL;
    }

    /* Post-creation variables */
    if (!strcmp((char *)node->name, "alloc_unit") && str) met->alloc_unit = atol (str);
    else if (!strcmp((char *)node->name, "units") && str) met->unit_str = strdup (str);
    else if (!strcmp((char *)node->name, "volume") && str) met->valstr_func = i_string_volume_metric;
    else if (!strcmp((char *)node->name, "rate") && str) met->valstr_func = i_string_rate_metric;
    
    xmlFree (str);
  } 

  /* Register */
  i_entity_register (self, ENTITY(obj), ENTITY(met));

  /* Apply refresh config */
  i_entity_refresh_config refconfig;
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  return met;
} 

