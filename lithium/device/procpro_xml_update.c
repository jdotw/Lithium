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
#include <induction/auth.h>
#include <induction/xml.h>

#include "procpro.h"

/* Update a process profile */

xmlNodePtr l_procpro_xml (l_procpro *procpro);

int xml_procpro_update (i_resource *self, i_xml_request *req)
{
  int num;
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;

  if (!self->hierarchy->dev->swrun) return -1;

  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  if (!req->xml_in) return -1;

  /* Interpret XML */
  l_procpro *procpro = l_procpro_create ();
  root_node = xmlDocGetRootElement (req->xml_in->doc);
  for (node = root_node->children; node; node = node->next)
  {
    char *str;

    str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
    if (!strcmp((char *)node->name, "id") && str) procpro->id = atoi (str);
    if (!strcmp((char *)node->name, "desc") && str) procpro->desc_str = strdup (str);
    if (!strcmp((char *)node->name, "match") && str) procpro->match_str = strdup (str);
    if (!strcmp((char *)node->name, "argmatch") && str) procpro->argmatch_str = strdup (str);

    xmlFree (str);
  }

  /* Update procpro */
  num = l_procpro_sql_update (self, procpro);
  if (num != 0)
  { i_printf (1, "xml_procpro_update failed to insert new procpro"); return -1; }

  /* Update live object */
  l_procpro_object_update (self, procpro);

  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "procpro_update");
  xmlDocSetRootElement (req->xml_out->doc, root_node);
  char *str;
  asprintf (&str, "%i", procpro->id);
  xmlNewChild (root_node, NULL, BAD_CAST "id", BAD_CAST str);
  free (str);

  return 1; 
}


