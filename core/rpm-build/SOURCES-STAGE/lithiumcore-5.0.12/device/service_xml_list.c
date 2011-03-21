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

#include "service.h"

/* List all configured services
 *
 * This is a brief listing of just what's in 
 * the 'services' table. It does not go deeper
 * into the entity, histoty and configvars
 *
 */

int xml_service_list (i_resource *self, i_xml_request *req)
{
  i_callback *cb;
  char *id_str = NULL;
  char *site_str = NULL;
  char *dev_str = NULL;

  if (req->xml_in)
  {
    /* Interpret XML */
    xmlNodePtr node;
    xmlNodePtr root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;

      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
      if (!strcmp((char *)node->name, "id") && str) id_str = strdup (str);
      if (!strcmp((char *)node->name, "site") && str) site_str = strdup (str);
      if (!strcmp((char *)node->name, "device") && str) dev_str = strdup (str);

      xmlFree (str);
    }
  }
  
  /* Load Actions */
  cb = l_service_sql_load_list (self, id_str, site_str, dev_str, l_service_xml_list_sqlcb, req);
  if (id_str) free (id_str);
  if (site_str) free (site_str);
  if (dev_str) free (dev_str);
  if (!cb) return -1;

  return 0;   /* Not finished, waiting for SQL list */
}

int l_service_xml_list_sqlcb (i_resource *self, i_list *list, void *passdata)
{
  i_xml *xml;
  l_service *service;
  xmlNodePtr root_node;
  i_xml_request *req = passdata;

  /* Create XML */
  xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "service_list");
  xmlDocSetRootElement (xml->doc, root_node);

  /* Add services */
  for (i_list_move_head(list); (service=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    xmlNodePtr service_node;
    service_node = l_service_xml (service);
    if (service_node) xmlAddChild (root_node, service_node);
  }

  /* Deliver */
  req->xml_out = xml;
  i_xml_deliver (self, req);

  return -1;  /* Dont keep service list */
}
