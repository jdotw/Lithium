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
 * Cancel documents edited status
 */

int xml_document_edit_cancel (i_resource *self, i_xml_request *req)
{
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_CLIENT)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  int doc_id = 0;
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
 
      if (!strcmp((char *)node->name, "id") && str) doc_id = atoi(str);

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_document_edit_cancel failed, no xml data received");
    return -1; 
  }
  if (doc_id == 0)
  {
    i_printf (1, "xml_document_edit_cancel failed, no document type specified"); 
    return -1;
  }

  /* Create outbound XML */
  i_xml *xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "document_edit");
  xmlDocSetRootElement (xml->doc, root_node);
  req->xml_out = xml;

  /* Set documents edit status */
  int num = l_document_sql_update_editor (self, doc_id, 0, NULL);
  if (num == 0)
  {
    xmlNewChild (root_node, NULL, BAD_CAST "result", BAD_CAST "OK");
  }
  else
  {
    xmlNewChild (root_node, NULL, BAD_CAST "result", BAD_CAST "ERROR");
  }

  return 1;
}


