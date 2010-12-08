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

/* Delete an Action*/

int xml_action_delete (i_resource *self, i_xml_request *req)
{
  int num;
  char *id_str = NULL;

  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  if (req->xml_in)
  {
    /* Action ID specified */
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;

    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;
      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
      if (!strcmp((char *)node->name, "id") && str) id_str = strdup (str);
      xmlFree (str);
    }
  }

  if (!id_str)
  { i_printf (1, "xml_action_update no action id specified"); return -1; }

  /* Delete */
  num = l_action_sql_delete (self, atol(id_str));
  if (num != 0)
  { i_printf (1, "xml_action_update failed to delete action %s", id_str); return -1; }

  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "action");
  xmlDocSetRootElement (req->xml_out->doc, root_node);
  xmlNewChild (root_node, NULL, BAD_CAST "id", BAD_CAST id_str);

  return 1;
}


