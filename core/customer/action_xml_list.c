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

#include "action.h"

/* List all configured actions
 *
 * This is a brief listing of just what's in 
 * the 'actions' table. It does not go deeper
 * into the entity, histoty and configvars
 *
 */

int xml_action_list (i_resource *self, i_xml_request *req)
{
  i_callback *cb;

  /* Load Actions */
  cb = l_action_sql_load_list (self, NULL, l_action_xml_list_sqlcb, req);
  if (!cb) return -1;

  return 0;   /* Not finished, waiting for SQL list */
}

int l_action_xml_list_sqlcb (i_resource *self, i_list *list, void *passdata)
{
  i_xml *xml;
  l_action *action;
  xmlNodePtr root_node;
  i_xml_request *req = passdata;

  /* Create XML */
  xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "action_list");
  xmlDocSetRootElement (xml->doc, root_node);

  /* Add actions */
  for (i_list_move_head(list); (action=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    xmlNodePtr action_node;
    action_node = l_action_xml (action);
    if (action_node) xmlAddChild (root_node, action_node);
  }

  /* Deliver */
  req->xml_out = xml;
  i_xml_deliver (self, req);

  return -1;  /* Dont keep action list */
}
