#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/postgresql.h>
#include <induction/auth.h>
#include <induction/xml.h>
#include <induction/callback.h>

#include "case.h"
#include "case_xml.h"

int xml_case_get (i_resource *self, i_xml_request *req)
{
  char *caseid_str = NULL;
  i_callback *cb;

  /* Get case id */  
  if (req->xml_in)
  {
    /* Search criteria specified */
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
  if (!caseid_str)
  { i_printf (1, "xml_case_get no caseid specified"); return -1; }

  /* Load case */
  cb = l_case_sql_get (self, caseid_str, l_case_xml_get_casecb, req);
  free (caseid_str);
  if (!cb) return -1;

  return 0;
}

int l_case_xml_get_casecb (i_resource *self, l_case *cas, void *passdata)
{
  i_xml *xml;
  xmlNodePtr root_node;
  xmlNodePtr case_node;
  i_xml_request *req = passdata;

  /* Create XML */
  xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "case");
  xmlDocSetRootElement (xml->doc, root_node);

  /* Add case XML */
  case_node = l_case_xml (cas);
  xmlAddChild (root_node, case_node);

  /* Deliver */
  req->xml_out = xml;
  i_xml_deliver (self, req);

  return -1;  /* Dont keep case list */
}
