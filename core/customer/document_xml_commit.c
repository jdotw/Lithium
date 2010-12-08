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
 * Commit the document data
 */

int xml_document_commit (i_resource *self, i_xml_request *req)
{
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_CLIENT)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  int doc_id = 0;
  char *type_str = NULL;
  char *desc_str = NULL;
  xmlNodePtr root_node = NULL;
  if (req->xml_in)
  {
    /* Set variables  */
    xmlNodePtr node;
    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;

      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
 
      if (!strcmp((char *)node->name, "id") && str) doc_id = atoi(str);
      else if (!strcmp((char *)node->name, "type") && str) type_str = strdup(str);
      else if (!strcmp((char *)node->name, "desc") && str) desc_str = strdup(str);

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_document_edit failed, no xml data received");
    return -1; 
  }
  if (doc_id == 0)
  {
    /* Create a new document */
    l_document *doc = l_document_create ();
    doc->type_str = strdup (type_str);
    doc->desc_str = strdup (desc_str);
    l_document_sql_insert (self, doc);
    doc_id = doc->id;
    l_document_free (doc);
  }

  /* Create outbound XML */
  i_xml *xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr return_root_node = xmlNewNode (NULL, BAD_CAST "document_commit");
  xmlDocSetRootElement (xml->doc, return_root_node);
  req->xml_out = xml;

  /* Find the type-specific commit function */
  int (*commit_func) () = NULL;
  if (strcmp(type_str, "scene") == 0) commit_func = l_scene_commit;
  else if (strcmp(type_str, "vrack") == 0) commit_func = l_vrack_commit;
  free (type_str);

  /* Mark document as not edited */
  l_document_sql_update_editor (self, doc_id, 0, NULL);

  /* Commit document */
  int num = commit_func (self, doc_id, req->xml_in, root_node);
  if (num == 0)
  {
    char *str;
    xmlNewChild (return_root_node, NULL, BAD_CAST "result", BAD_CAST "OK");
    asprintf (&str, "%i", doc_id);
    xmlNewChild (return_root_node, NULL, BAD_CAST "id", BAD_CAST str);
    free (str);
  }
  else
  {
    xmlNewChild (return_root_node, NULL, BAD_CAST "result", BAD_CAST "ERROR");
  }

  return 1;
}
