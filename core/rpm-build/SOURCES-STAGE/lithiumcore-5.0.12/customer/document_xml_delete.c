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

#include "scene.h"
#include "vrack.h"
#include "document.h"

/*
 * Delete a document
 */

int xml_document_delete (i_resource *self, i_xml_request *req)
{
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_STAFF)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  int doc_id = 0;
  char *type_str = NULL;
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
      else if (!strcmp((char *)node->name, "type") && str) type_str = strdup(str);

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_document_delete failed, no xml data received");
    return -1; 
  }
  if (doc_id == 0)
  {
    i_printf (1, "xml_document_delete failed, no document type specified"); 
    return -1;
  }

  /* Create outbound XML */
  i_xml *xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "document_delete");
  xmlDocSetRootElement (xml->doc, root_node);
  req->xml_out = xml;

  /* Delete from documents table */
  int num = l_document_sql_delete (self, doc_id);
  if (num != 0)
  {
    xmlNewChild (root_node, NULL, BAD_CAST "result", BAD_CAST "Failed to delete from documents table");
    return 1;
  }

  /* Find the type-specific delete function */
  int (*delete_func) () = NULL;
  if (strcmp(type_str, "scene") == 0) delete_func = l_scene_delete;
  else if (strcmp(type_str, "vrack") == 0) delete_func = l_vrack_delete;

  /* Delete document */
  num = delete_func (self, doc_id);
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
