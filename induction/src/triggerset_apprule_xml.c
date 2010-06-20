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

xmlNodePtr i_triggerset_apprule_xml (i_triggerset_apprule *rule)
{
  char *str;
  xmlNodePtr rule_node;

  /* Create root node */
  rule_node = xmlNewNode (NULL, BAD_CAST "apprule");

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

  /* Flag */
  asprintf (&str, "%i", rule->applyflag);
  xmlNewChild (rule_node, NULL, BAD_CAST "applyflag", BAD_CAST str);
  free (str);

  return rule_node;
}

/* @} */
