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
#include <induction/incident.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/timer.h>
#include <induction/xml.h>

#include "case.h"
#include "incident.h"
#include "action.h"

/* Execute an action immediately */

int xml_action_execute (i_resource *self, i_xml_request *req)
{
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;

  if (!req->xml_in) return -1;

  /* Interpret XML */
  unsigned long incid = 0;
  unsigned long actionid = 0;
  root_node = xmlDocGetRootElement (req->xml_in->doc);
  for (node = root_node->children; node; node = node->next)
  {
    char *str;

    str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
    if (!strcmp((char *)node->name, "incid") && str) incid = atol (str);
    if (!strcmp((char *)node->name, "actionid") && str) actionid = atol (str);

    xmlFree (str);
  }

  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "action_execute");
  xmlDocSetRootElement (req->xml_out->doc, root_node);

  /* Retrieve incident */
  i_hashtable *inc_table = l_incident_table ();
  i_hashtable_key *key = i_hashtable_create_key_long (incid, inc_table->size);
  i_incident *inc = i_hashtable_get (inc_table, key);
  if (!inc)
  {
    xmlNewChild (root_node, NULL, BAD_CAST "error", BAD_CAST "Specified incident not found.");
    return 1;
  }
  i_hashtable_key_free (key);

  /* Find action */
  l_action *action;
  for (i_list_move_head(inc->action_list); (action=i_list_restore(inc->action_list))!=NULL; i_list_move_next(inc->action_list))
  {
    /* Check ID */
    if (action->id == actionid)
    { break; }
  }
  if (!action)
  {
    xmlNewChild (root_node, NULL, BAD_CAST "error", BAD_CAST "Specified action not found.");
    return 1; 
  }

  /* Execute */
  l_action_incident_run_action (self, action);

  /* Set message */
  xmlNewChild (root_node, NULL, BAD_CAST "message", BAD_CAST "Action Script executed.");
  
  return 1;
}

