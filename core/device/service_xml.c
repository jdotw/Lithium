#include <stdlib.h> 
#include <string.h> 
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/entity.h>
#include <induction/entity_xml.h>
#include <induction/postgresql.h>
#include <induction/xml.h>

#include "service.h"

/*
 * Action XML functions
 */

xmlNodePtr l_service_xml (l_service *service)
{
  char *str;
  xmlNodePtr service_node;

  /* Create service node */
  service_node = xmlNewNode (NULL, BAD_CAST "service");

  /* service info */
  asprintf (&str, "%li", service->id);
  xmlNewChild (service_node, NULL, BAD_CAST "id", BAD_CAST str);
  free (str);
  if (service->desc_str) xmlNewChild (service_node, NULL, BAD_CAST "desc", BAD_CAST service->desc_str);
  asprintf (&str, "%i", service->enabled);
  xmlNewChild (service_node, NULL, BAD_CAST "enabled", BAD_CAST str);
  free (str);
  if (service->script_file) xmlNewChild (service_node, NULL, BAD_CAST "script_file", BAD_CAST service->script_file);

  if (service->obj)
  {
    xmlNodePtr ent_node = i_entity_descriptor_xml (i_entity_descriptor_struct_static(ENTITY(service->obj)));
    if (ent_node) xmlAddChild (service_node, ent_node);
  }

  return service_node;
}

xmlNodePtr l_service_configvar_xml (l_service_configvar *var)
{
  char *str;
  xmlNodePtr var_node;

  /* Create service node */
  var_node = xmlNewNode (NULL, BAD_CAST "config_variable");

  /* Config variable info */
  asprintf (&str, "%li", var->id);
  xmlNewChild (var_node, NULL, BAD_CAST "id", BAD_CAST str);
  free (str);
  if (var->name_str) xmlNewChild (var_node, NULL, BAD_CAST "name", BAD_CAST var->name_str);
  if (var->desc_str) xmlNewChild (var_node, NULL, BAD_CAST "desc", BAD_CAST var->desc_str);
  if (var->value_str) xmlNewChild (var_node, NULL, BAD_CAST "value", BAD_CAST var->value_str);
  asprintf (&str, "%i", var->required);
  xmlNewChild (var_node, NULL, BAD_CAST "required", BAD_CAST str);
  free (str);
  
  return var_node;
}

l_service_configvar* l_service_configvar_fromxml (i_xml *xml, xmlNodePtr var_node)
{
  l_service_configvar *var = l_service_configvar_create ();
  xmlNodePtr node;

  /* Parse tree */
  for (node = var_node->children; node; node = node->next)
  {
    char *str;
    str = (char *) xmlNodeListGetString (xml->doc, node->xmlChildrenNode, 1);
    if (!str) continue;

    if (!strcmp((char *)node->name, "id") && str) var->id = atol (str);
    else if (!strcmp((char *)node->name, "name") && str) var->name_str = strdup (str);
    else if (!strcmp((char *)node->name, "desc") && str) var->desc_str = strdup (str);
    else if (!strcmp((char *)node->name, "value") && str) var->value_str = strdup (str);
    else if (!strcmp((char *)node->name, "required") && str) var->required = atoi (str);

    xmlFree (str);
  } 

  return var;
} 

xmlNodePtr l_service_script_xml (l_service_script *script)
{
  char *str;
  l_service_configvar *var;
  xmlNodePtr script_node;

  /* Create script node */
  script_node = xmlNewNode (NULL, BAD_CAST "script");

  /* Config variable info */
  if (script->name_str) xmlNewChild (script_node, NULL, BAD_CAST "name", BAD_CAST script->name_str);
  if (script->desc_str) xmlNewChild (script_node, NULL, BAD_CAST "desc", BAD_CAST script->desc_str);
  if (script->version_str) xmlNewChild (script_node, NULL, BAD_CAST "installed_version", BAD_CAST script->version_str);
  if (script->info_str) xmlNewChild (script_node, NULL, BAD_CAST "info", BAD_CAST script->info_str);
  if (script->port_str) xmlNewChild (script_node, NULL, BAD_CAST "port", BAD_CAST script->port_str);
  if (script->protocol_str) xmlNewChild (script_node, NULL, BAD_CAST "protocol", BAD_CAST script->protocol_str);
  if (script->transport_str) xmlNewChild (script_node, NULL, BAD_CAST "transport", BAD_CAST script->transport_str);
  for (i_list_move_head(script->configvar_list); (var=i_list_restore(script->configvar_list))!=NULL; i_list_move_next(script->configvar_list))
  {
    xmlNodePtr var_node = l_service_configvar_xml (var);
    if (var_node) xmlAddChild (script_node, var_node);
  }
  asprintf (&str, "%i", script->status);
  xmlNewChild (script_node, NULL, BAD_CAST "status", BAD_CAST str);
  free (str);

  return script_node;
}

xmlNodePtr l_service_log_xml (l_service_log *log)
{
  char *str;
  xmlNodePtr log_node;

  /* Create script node */
  log_node = xmlNewNode (NULL, BAD_CAST "log");

  /* Config variable info */
  asprintf (&str, "%li", log->id);
  xmlNewChild (log_node, NULL, BAD_CAST "id", BAD_CAST str);
  free (str);
  asprintf (&str, "%li", log->timestamp);
  xmlNewChild (log_node, NULL, BAD_CAST "timestamp", BAD_CAST str);
  free (str);
  if (log->comment_str) xmlNewChild (log_node, NULL, BAD_CAST "comment", BAD_CAST log->comment_str);

  return log_node;
}



