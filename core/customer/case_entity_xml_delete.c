#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/entity.h>
#include <induction/postgresql.h>
#include <induction/auth.h>
#include <induction/xml.h>
#include <induction/entity_xml.h>

#include "case.h"
#include "case_xml.h"

int xml_case_entity_delete (i_resource *self, i_xml_request *req)
{
  int num;
  char *str;
  long caseid = -1;
  i_entity_descriptor *entdesc;
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;

  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_USER)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Create log */
  entdesc = i_entity_descriptor_create ();

  /* Interpret fields */
  if (req->xml_in)
  {
    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      /* Handle entity descriptor */
      if (!strcmp((char *)node->name, "entity_descriptor"))
      { 
        entdesc = i_entity_descriptor_fromxml (req->xml_in, node); 
        continue;
      }

      /* Get/check string */
      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
      if (!str) continue;

      /* Interpret item */
      if (!strcmp((char *)node->name, "caseid") && str) caseid = atol(str);

      /* Free string */
      xmlFree (str);
    }
  }
  if (!entdesc) return -1;
  if (caseid == -1) { if (entdesc) i_entity_descriptor_free (entdesc); return -1; }

  /* Remove entry */
  num = l_case_entity_remove (self, caseid, entdesc);
  i_entity_descriptor_free (entdesc);
  if (num != 0) return -1;

  /* Prepare to call xml_case_entity_list */
  if (req->xml_in) i_xml_free (req->xml_in);
  req->xml_in = i_xml_create ();
  req->xml_in->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "criteria");
  xmlDocSetRootElement (req->xml_in->doc, root_node);
  asprintf (&str, "%li", caseid);
  xmlNewChild (root_node, NULL, BAD_CAST "caseid", BAD_CAST str);
  free (str);

  return xml_case_entity_list (self, req);
}

