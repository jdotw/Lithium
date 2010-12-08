#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/callback.h>
#include <induction/list.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/xml.h>
#include <induction/entity_xml.h>
#include <induction/auth.h>

#include "document.h"

/*
 * Create a document
 */

int xml_document_create (i_resource *self, i_xml_request *req)
{
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_STAFF)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  l_document *doc = l_document_create ();
  if (req->xml_in)
  {
    /* Set variables  */
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;
    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;

      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
 
      if (!strcmp((char *)node->name, "type") && str) doc->type_str = strdup(str);
      else if (!strcmp((char *)node->name, "desc") && str) doc->desc_str = strdup(str);

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_document_create failed, no xml data received");
    return -1; 
  }
  if (doc->type_str == NULL)
  {
    i_printf (1, "xml_document_create failed, no document type specified"); 
    return -1;
  }

  /* Create outbound XML */
  i_xml *xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "document_create");
  xmlDocSetRootElement (xml->doc, root_node);
  req->xml_out = xml;

  /* Create blank document */
  int num = l_document_sql_insert (self, doc);
  if (num == 0)
  {
    char *str;
    asprintf (&str, "%i", doc->id);
    xmlNewChild (root_node, NULL, BAD_CAST "id", BAD_CAST str);
    free (str);
  }
  else
  {
    xmlNewChild (root_node, NULL, BAD_CAST "error", BAD_CAST "Failed to create document");
  }

  /* Clean up */
  l_document_free (doc);

  return 1;
}
