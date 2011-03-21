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

int xml_case_update (i_resource *self, i_xml_request *req)
{
  i_callback *cb;
  char *id = NULL;
  
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_USER)
  { req->xml_out = i_xml_denied (); return 1; }

  if (req->xml_in)
  {
    /* Case ID specified */
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;

    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;
      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
      if (!strcmp((char *)node->name, "caseid") && str) id = strdup (str);
      xmlFree (str);
    }
  }

  if (!id)
  { return -1; }
  
  /* Load case */
  cb = l_case_sql_list (self, id, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, l_case_xml_update_casecb, req);
  free (id);
  if (!cb) return -1;

  return 0;
}

int l_case_xml_update_casecb (i_resource *self, i_list *list, void *passdata)
{
  int num;
  char *str;
  l_case *cas;
  xmlNodePtr root_node;
  i_xml_request *req = passdata;

  /* Retrieve case */
  i_list_move_head (list);
  cas = i_list_restore (list);
  if (!cas)
  { i_xml_deliver (self, req); return -1; }
  
  /* Interpret fields */
  if (req->xml_in)
  {
    xmlNodePtr node;
    xmlNodePtr root_node;

    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;
      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
      if (!strcmp((char *)node->name, "hline") && str) 
      { if (cas->hline_str) free (cas->hline_str); cas->hline_str = strdup (str); }
      if (!strcmp((char *)node->name, "owner") && str) 
      { if (cas->owner_str) free (cas->owner_str); cas->owner_str = strdup (str); }
      xmlFree (str);
    }
  }

  /* Update */
  num = l_case_sql_update (self, cas);
  if (num != 0)
  { i_xml_deliver (self, req); return -1; }

  /* Call xml_case_list */
  if (req->xml_in) i_xml_free (req->xml_in);
  req->xml_in = i_xml_create ();
  req->xml_in->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "criteria");
  xmlDocSetRootElement (req->xml_in->doc, root_node);
  asprintf (&str, "%li", cas->id);
  xmlNewChild (root_node, NULL, BAD_CAST "id", BAD_CAST str);
  free (str);
  num = xml_case_list (self, req);
  if (num != 0)
  { i_xml_deliver (self, req); }
  
  return -1;  /* Dont keep case list */
}
