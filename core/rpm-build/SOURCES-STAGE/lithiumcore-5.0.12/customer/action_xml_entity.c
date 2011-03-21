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

/* List Action Entities */

int xml_action_entity_list (i_resource *self, i_xml_request *req)
{
  if (!req->xml_in) return -1;

  /* Interpret XML */
  long actionid = 0;
  xmlNodePtr node;
  xmlNodePtr root_node = xmlDocGetRootElement (req->xml_in->doc);
  for (node = root_node->children; node; node = node->next)
  {
    char *str;

    str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
    if (!strcmp((char *)node->name, "actionid") && str) actionid = atol (str);

    xmlFree (str);
  }

  /* Retrieve entity list */
  i_callback *cb = l_action_sql_entity_load_list (self, actionid, l_action_xml_entity_list_sqlcb, req);
  if (!cb)
  {
    i_printf (1, "xml_action_entity_list failed to load config variable list");
    return -1;
  }

  return 0;
}

int l_action_xml_entity_list_sqlcb (i_resource *self, i_list *list, void *passdata)
{
  i_xml_request *req = passdata;

  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "action_entity_list");
  xmlDocSetRootElement (req->xml_out->doc, root_node);

  /* Add config variables */
  i_entity_descriptor *entdesc;
  for (i_list_move_head(list); (entdesc=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    xmlNodePtr entity_node = i_entity_descriptor_xml (entdesc);
    if (entity_node) xmlAddChild (root_node, entity_node);
  }

  i_xml_deliver (self, req);

  return -1;    /* Dont keep list */
}


