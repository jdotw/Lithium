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

#include "case.h"
#include "case_xml.h"

int xml_case_logentry_list (i_resource *self, i_xml_request *req)
{
  long caseid = -1;
  i_callback *cb;
  
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
      if (!strcmp((char *)node->name, "caseid") && str) caseid = atol (str);
      xmlFree (str);
    }
  }

  if (caseid == -1) return -1; 

  /* Load log entries */
  cb = l_case_logentry_sql_list (self, caseid, l_case_logentry_xml_list_logcb, req);
  if (!cb) return -1;

  return 0;
}

int l_case_logentry_xml_list_logcb (i_resource *self, i_list *list, void *passdata)
{
  i_xml *xml;
  xmlNodePtr root_node;
  i_xml_request *req = passdata;
  l_case_logentry *log;

  /* Create XML */
  xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "logentry_list");
  xmlDocSetRootElement (xml->doc, root_node);

  /* Add cases */
  for (i_list_move_head(list); (log=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    xmlNodePtr log_node;
    log_node = l_case_logentry_xml (log);
    xmlAddChild (root_node, log_node);
  }

  /* Deliver */
  req->xml_out = xml;
  i_xml_deliver (self, req);

  return -1;  /* Dont keep case list */
}
