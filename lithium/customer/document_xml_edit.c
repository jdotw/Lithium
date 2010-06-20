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
 * Mark a document as being edited 
 */

int l_document_xml_edit_sqlcb (i_resource *self, i_list *list, void *passdata);

int xml_document_edit (i_resource *self, i_xml_request *req)
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
    i_printf (1, "xml_document_edit failed, no xml data received");
    return -1; 
  }
  if (doc_id == 0)
  {
    i_printf (1, "xml_document_edit failed, no document type specified"); 
    return -1;
  }

  /* Get the document */
  i_callback *cb = l_document_sql_load_list (self, NULL, doc_id, l_document_xml_edit_sqlcb, req);
  if (!cb)
  { i_printf (1, "xml_document_edit failed to load document from SQL"); return -1; }

  return 0;
}

int l_document_xml_edit_sqlcb (i_resource *self, i_list *list, void *passdata)
{
  i_xml_request *req = (i_xml_request *) passdata;

  /* Check for editing flags */
  int force = 0;
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;
  root_node = xmlDocGetRootElement (req->xml_in->doc);
  for (node = root_node->children; node; node = node->next)
  {
    char *str;
    str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
    if (!strcmp((char *)node->name, "force") && str) force = atoi(str);
    xmlFree (str);
  }

  /* Create outbound XML */
  i_xml *xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "document_edit");
  xmlDocSetRootElement (xml->doc, root_node);
  req->xml_out = xml;

  /* Check document status */
  i_list_move_head (list);
  l_document *doc = i_list_restore (list);
  if (doc)
  {
    /* Check if doc is edited */
    if (doc->state == 1 && force == 0)
    {
      /* Return information about current editor */

      xmlNewChild (root_node, NULL, BAD_CAST "result", BAD_CAST "LOCKED");
      xmlNewChild (root_node, NULL, BAD_CAST "username", BAD_CAST doc->editor_str);
    }
    else
    {
      /* Not edited, begin edit session */
      int num = l_document_sql_update_editor (self, doc->id, 1, req->auth->username);
      if (num == 0)
      {
        xmlNewChild (root_node, NULL, BAD_CAST "result", BAD_CAST "OK");
      }
      else
      {
        xmlNewChild (root_node, NULL, BAD_CAST "result", BAD_CAST "ERROR");
        xmlNewChild (root_node, NULL, BAD_CAST "message", BAD_CAST "Failed to lock document record");
      }
    }

    /* Add basic info about document */
    char *str;
    asprintf (&str, "%i", doc->state);
    xmlNewChild (root_node, NULL, BAD_CAST "state", BAD_CAST str);
    free (str);
    xmlNewChild (root_node, NULL, BAD_CAST "editor", BAD_CAST doc->editor_str);
    asprintf (&str, "%i", doc->version);
    xmlNewChild (root_node, NULL, BAD_CAST "version", BAD_CAST str);
    free (str);
  }
  else
  {
    /* Document not found */
    xmlNewChild (root_node, NULL, BAD_CAST "result", BAD_CAST "ERROR");
    xmlNewChild (root_node, NULL, BAD_CAST "message", BAD_CAST "Document not found");
  }

  /* Deliver */
  i_xml_deliver (self, req);

  return 0; /* Frees list */
}


