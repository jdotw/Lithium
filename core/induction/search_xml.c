#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include "induction.h"
#include "hashtable.h"
#include "callback.h"
#include "list.h"
#include "cement.h"
#include "entity.h"
#include "customer.h"
#include "site.h"
#include "device.h"
#include "metric.h"
#include "metric_xml.h"
#include "hierarchy.h"
#include "xml.h"
#include "entity_xml.h"
#include "auth.h"
#include "search.h"

/*
 * Server-side search
 */

int xml_search (i_resource *self, i_xml_request *req)
{
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_RO)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  int max_type = 6;
  int flags = 0;
  i_list *keyword_list = i_list_create ();
  i_list_set_destructor (keyword_list, free);
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
 
      if (!strcmp((char *)node->name, "keyword") && str) i_list_enqueue (keyword_list, strdup(str));
      else if (!strcmp((char *)node->name, "operator") && str)
      {
        if (strcmp(str, "OR")) flags = flags & SEARCH_ANY;
        else if (strcmp(str, "AND")) flags = flags & SEARCH_ALL;
      }
      else if (!strcmp((char *)node->name, "regex") && str) 
      {
        if (atoi(str) == 1) flags = flags & SEARCH_REGEX;
      }
      else if (!strcmp((char *)node->name, "max_type") && str) max_type = atoi (str);

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_search failed, no xml data received");
    return -1; 
  }
  if (keyword_list->size < 1)
  {
    i_printf (1, "xml_search no keywords found!");
  }

  /* Perform search */
  i_list *entity_list = i_search_cache_query (self, keyword_list, max_type, flags);
  i_list_free (keyword_list);

  /* Create XML */
  i_xml *xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "entity_list");
  xmlDocSetRootElement (xml->doc, root_node);
  req->xml_out = xml;

  /* Add documents */
  i_entity *entity;
  for (i_list_move_head(entity_list); (entity=i_list_restore(entity_list))!=NULL; i_list_move_next(entity_list))
  {
    xmlNodePtr result_node = xmlNewNode (NULL, BAD_CAST "result");

    i_entity_descriptor *entdesc = i_entity_descriptor_struct_static (entity);
    xmlNodePtr entity_node = i_entity_descriptor_xml (entdesc);
    if (entity_node) xmlAddChild (result_node, entity_node);

    if (entity->ent_type == 6)
    {
      /* Metric */
      i_metric *met = (i_metric *)entity;
      i_metric_value *val = i_metric_curval (met);
      if (val)
      {
        xmlNodePtr value_node = i_metric_value_xml (met, val);
        if (value_node) xmlAddChild (result_node, value_node);
      }
    }
    
    xmlAddChild (root_node, result_node);
  }

  /* Cleanup */
  i_list_free (entity_list);

  return 1;
}

