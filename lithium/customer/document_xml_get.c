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
 * Retrieve a document
 */

int l_document_xml_getcb (i_resource *self, i_xml *xml, void *passdata);

int xml_document_get (i_resource *self, i_xml_request *req)
{
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_CLIENT)
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
    i_printf (1, "xml_document_commit failed, no xml data received");
    return -1; 
  }
  if (doc_id == 0)
  {
    i_printf (1, "xml_document_commit failed, no document type specified"); 
    return -1;
  }

  /* Find the type-specific commit function */
  i_callback* (*get_func) () = NULL;
  if (strcmp(type_str, "scene") == 0) get_func = l_scene_get;
  if (strcmp(type_str, "vrack") == 0) get_func = l_vrack_get;

  /* Commit document */
  i_callback *cb = get_func (self, doc_id, l_document_xml_getcb, req);
  if (!cb)
  {
    return -1;
  }

  return 0;
}

int l_document_xml_getcb (i_resource *self, i_xml *xml, void *passdata)
{
  i_xml_request *req = passdata;

  /* Deliver */
  req->xml_out = xml;
  i_xml_deliver (self, req);

  return 0;
}
