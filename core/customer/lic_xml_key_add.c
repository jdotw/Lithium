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

#include "lic.h"

/* Add a license key */

int xml_lic_key_add (i_resource *self, i_xml_request *req)
{
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;

  if (!req->xml_in) return -1;

  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  char *key_str = NULL;
  root_node = xmlDocGetRootElement (req->xml_in->doc);
  for (node = root_node->children; node; node = node->next)
  {
    char *str;

    str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
    if (!strcmp((char *)node->name, "key") && str) 
    { key_str = strdup (str); }

    xmlFree (str);
  }

  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "result_list");
  xmlDocSetRootElement (req->xml_out->doc, root_node);

  xmlNodePtr lic_node = xmlNewNode (NULL, BAD_CAST "license");
  xmlNewChild (lic_node, NULL, BAD_CAST "key", BAD_CAST key_str);
      
  /* Validate */
  l_lic_key *key = l_lic_validate_key (self, key_str);
  if (key && key->status == KEY_VALID)
  {
    /* Valid key */
    xmlNewChild (lic_node, NULL, BAD_CAST "result", BAD_CAST "1");
    xmlNewChild (lic_node, NULL, BAD_CAST "message", BAD_CAST "License key validated and added.");
    l_lic_key_add (self, key_str); 
  }
  else
  {
    /* Invalid key */
    xmlNewChild (lic_node, NULL, BAD_CAST "result", BAD_CAST "2");
    xmlNewChild (lic_node, NULL, BAD_CAST "message", BAD_CAST "Invalid License Key.");
  }
  xmlAddChild (root_node, lic_node);
  
  return 1; 
}


