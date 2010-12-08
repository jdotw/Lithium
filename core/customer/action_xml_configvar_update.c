#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/cement.h>
#include <induction/callback.h>
#include <induction/entity.h>
#include <induction/entity_xml.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/timer.h>
#include <induction/xml.h>
#include <induction/auth.h>

#include "action.h"

/* Update an Actions Config Variable */

int xml_action_configvar_update (i_resource *self, i_xml_request *req)
{
  int num;
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;

  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  if (!req->xml_in) return -1;

  /* Interpret XML */
  long actionid = 0;
  l_action_configvar *var = NULL;
  root_node = xmlDocGetRootElement (req->xml_in->doc);
  for (node = root_node->children; node; node = node->next)
  {
    char *str;

    str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
    if (!strcmp((char *)node->name, "actionid") && str) actionid = atol (str);
    if (!strcmp((char *)node->name, "config_variable")) var = l_action_configvar_fromxml (req->xml_in, node);

    xmlFree (str);
  }

  /* Attempt to update/insert configvar */
  num = l_action_sql_configvar_update_insert (self, actionid, var, 0);

  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "action_configvar");
  xmlDocSetRootElement (req->xml_out->doc, root_node);
  xmlNodePtr action_node = l_action_configvar_xml (var);
  xmlAddChild (root_node, action_node);

  return 1; 
}


