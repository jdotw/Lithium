#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/entity.h>
#include <induction/postgresql.h>
#include <induction/auth.h>
#include <induction/xml.h>
#include <induction/callback.h>
#include <induction/entity_xml.h>

#include "case.h"
#include "case_xml.h"

int xml_case_entity_list (i_resource *self, i_xml_request *req)
{
  i_callback *cb;
  char *caseid_str = NULL;
  
  if (req->xml_in)
  {
    /* Case ID Specifried */
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;

    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;
      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
      if (!strcmp((char *)node->name, "caseid") && str) caseid_str = strdup (str);
      xmlFree (str);
    }
  }
  if (!caseid_str) return -1; 

  /* Load log entries */
  cb = l_case_entity_sql_list (self, caseid_str, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, l_case_entity_xml_list_sqlcb, req);
  free (caseid_str);
  if (!cb) return -1;

  return 0;
}

int l_case_entity_xml_list_sqlcb (i_resource *self, i_list *list, void *passdata)
{
  i_xml *xml;
  l_case *cas;
  xmlNodePtr root_node;
  i_xml_request *req = passdata;

  /* Create XML */
  xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "entity_list");
  xmlDocSetRootElement (xml->doc, root_node);

  /* Add cases */
  for (i_list_move_head(list); (cas=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    xmlNodePtr case_node;
    case_node = l_case_xml (cas);
    xmlAddChild (root_node, case_node);
  }

  /* Deliver */
  req->xml_out = xml;
  i_xml_deliver (self, req);

  return -1;  /* Dont keep entity list */
}
