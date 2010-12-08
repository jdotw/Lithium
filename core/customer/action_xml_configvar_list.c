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

/* List Actions Config Variable */

int xml_action_configvar_list (i_resource *self, i_xml_request *req)
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

  /* Retrieve configvar list */
  i_callback *cb = l_action_sql_configvar_load_list (self, actionid, l_action_xml_configvar_list_sqlcb, req);
  if (!cb)
  {
    i_printf (1, "xml_action_configvar_list failed to load config variable list"); 
    return -1; 
  }

  return 0;
}

int l_action_xml_configvar_list_sqlcb (i_resource *self, i_list *variable_list, void *passdata)
{
  i_xml_request *req = passdata;

  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "action_configvar_list");
  xmlDocSetRootElement (req->xml_out->doc, root_node);

  /* Add config variables */
  l_action_configvar *var;
  for (i_list_move_head(variable_list); (var=i_list_restore(variable_list))!=NULL; i_list_move_next(variable_list))
  {
    xmlNewChild (root_node, NULL, BAD_CAST var->name_str, BAD_CAST var->value_str);
  }

  i_xml_deliver (self, req);

  return -1;    /* Dont keep list */ 
}


