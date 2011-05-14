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
#include <induction/user.h>
#include <induction/auth.h>

#include "user.h"

/* Add/Edit a User */

int xml_user_remove (i_resource *self, i_xml_request *req)
{
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;

  if (!req->xml_in) return -1;

  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  char *username_str = NULL;
  root_node = xmlDocGetRootElement (req->xml_in->doc);
  for (node = root_node->children; node; node = node->next)
  {
    char *str;

    str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
    if (!strcmp((char *)node->name, "username") && str) username_str = strdup (str);

    xmlFree (str);
  }

  /* Remove old user */
  if (username_str)
  {
    i_user_sql_delete (self, username_str);
    free (username_str);
    username_str = NULL;

    /* Create return xml */
    req->xml_out = i_xml_create ();
    req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
    root_node = xmlNewNode (NULL, BAD_CAST "user_remove");
    xmlDocSetRootElement (req->xml_out->doc, root_node);

    return 1; 
  }
  else
  {
    i_printf(1, "xml_user_remove no user specified");
    return -1;
  }
}

int xml_user_delete (i_resource *self, i_xml_request *req)
{
  return xml_user_remove (self, req);
}
