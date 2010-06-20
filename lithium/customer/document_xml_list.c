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

int xml_document_list (i_resource *self, i_xml_request *req)
{
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_CLIENT)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
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
 
      if (!strcmp((char *)node->name, "type") && str) type_str = strdup(str);

      xmlFree (str);
    }
  }

  /* Request list from SQL */
  i_callback *sqlcb = l_document_sql_load_list (self, type_str, 0, l_document_xml_list_sqlcb, req);
  if (!sqlcb)
  { i_printf (1, "xml_document_list failed to query SQL for document list"); return 1; }

  /* Clean up */
  if (type_str) free (type_str);
  
  return 0;
}

int l_document_xml_list_sqlcb (i_resource *self, i_list *list, void *passdata)
{
  i_xml_request *req = passdata;

  /* Create outbound XML */
  i_xml *xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "document_list");
  xmlDocSetRootElement (xml->doc, root_node);

  /* Add documents */
  l_document *doc;
  for (i_list_move_head(list); (doc=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    char *str;
    
    xmlNodePtr doc_node = xmlNewNode (NULL, BAD_CAST "document");

    asprintf (&str, "%i", doc->id);
    xmlNewChild (doc_node, NULL, BAD_CAST "id", BAD_CAST str);
    free (str);

    xmlNewChild (doc_node, NULL, BAD_CAST "type", BAD_CAST doc->type_str);
    xmlNewChild (doc_node, NULL, BAD_CAST "desc", BAD_CAST doc->desc_str);

    asprintf (&str, "%i", doc->state);
    xmlNewChild (doc_node, NULL, BAD_CAST "state", BAD_CAST str);
    free (str);

    xmlNewChild (doc_node, NULL, BAD_CAST "editor", BAD_CAST doc->editor_str);

    asprintf (&str, "%li", doc->version);
    xmlNewChild (doc_node, NULL, BAD_CAST "version", BAD_CAST str);
    free (str);

    xmlAddChild (root_node, doc_node);
  }

  /* Deliver */
  req->xml_out = xml;
  i_xml_deliver (self, req);

  return 0;     /* Return 0 to destroy the list */
}
