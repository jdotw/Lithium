#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <libxml/parser.h>

#include "induction.h"
#include "list.h"
#include "cement.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "trigger.h"
#include "entity_xml.h"
#include "message.h"
#include "socket.h"
#include "data.h"
#include "msgproc.h"
#include "auth.h"
#include "respond.h"
#include "triggerset.h"
#include "xml.h"

/** \addtogroup triggerset Trigger Sets
 * @ingroup trigger
 * @{
 */

/*
 * Triggerset-related XML functions
 */

xmlNodePtr i_triggerset_valrule_xml (i_triggerset_valrule *rule)
{
  char *str;
  xmlNodePtr rule_node;

  /* Create node */
  rule_node = xmlNewNode (NULL, BAD_CAST "valrule");

  /* Basic Info */
  asprintf (&str, "%li", rule->id);
  xmlNewChild (rule_node, NULL, BAD_CAST "id", BAD_CAST str);
  free (str);

  /* Identifiers */
  if (rule->site_name) xmlNewChild (rule_node, NULL, BAD_CAST "site_name", BAD_CAST rule->site_name);
  if (rule->site_desc) xmlNewChild (rule_node, NULL, BAD_CAST "site_desc", BAD_CAST rule->site_desc);
  if (rule->dev_name) xmlNewChild (rule_node, NULL, BAD_CAST "dev_name", BAD_CAST rule->dev_name);
  if (rule->dev_desc) xmlNewChild (rule_node, NULL, BAD_CAST "dev_desc", BAD_CAST rule->dev_desc);
  if (rule->obj_name) xmlNewChild (rule_node, NULL, BAD_CAST "obj_name", BAD_CAST rule->obj_name);
  if (rule->obj_desc) xmlNewChild (rule_node, NULL, BAD_CAST "obj_desc", BAD_CAST rule->obj_desc);
  if (rule->trg_name) xmlNewChild (rule_node, NULL, BAD_CAST "trg_name", BAD_CAST rule->trg_name);
  if (rule->trg_desc) xmlNewChild (rule_node, NULL, BAD_CAST "trg_desc", BAD_CAST rule->trg_desc);

  /* Value */
  if (rule->xval_str) xmlNewChild (rule_node, NULL, BAD_CAST "xval", BAD_CAST rule->xval_str);
  if (rule->yval_str) xmlNewChild (rule_node, NULL, BAD_CAST "yval", BAD_CAST rule->yval_str);

  /* Duration */
  asprintf (&str, "%li", rule->duration_sec);
  xmlNewChild (rule_node, NULL, BAD_CAST "duration", BAD_CAST str);
  free (str);
  
  /* Flags */
  asprintf (&str, "%i", rule->trg_type);
  xmlNewChild (rule_node, NULL, BAD_CAST "trg_type_num", BAD_CAST str);
  free (str);
  xmlNewChild (rule_node, NULL, BAD_CAST "trg_type", BAD_CAST i_trigger_typestr(rule->trg_type));
  
  asprintf (&str, "%i", rule->adminstate);
  xmlNewChild (rule_node, NULL, BAD_CAST "adminstate_num", BAD_CAST str);
  free (str);
  xmlNewChild (rule_node, NULL, BAD_CAST "adminstate", BAD_CAST i_entity_adminstatestr(rule->adminstate));

  return rule_node;
}

/* @} */
