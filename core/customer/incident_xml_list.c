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
#include <induction/xml.h>
#include <induction/incident.h>
#include <induction/incident_xml.h>

#include "device.h"
#include "navtree.h"
#include "site.h"
#include "case.h"
#include "action.h"
#include "incident.h"

/* List all active incidents */

int xml_incident_list (i_resource *self, i_xml_request *req)
{
  i_xml *xml;
  i_incident *inc;
  i_list *inc_list;
  xmlNodePtr root_node = NULL;

  if (req->xml_in)
  {
    /* XML Document provided, criteria specified.
     * Perform SQL list of incidents matching
     * the criteria provided
     */
    char *id_str = NULL;
    char *state_str = NULL;
    char *startupper_str = NULL;
    char *startlower_str = NULL;
    char *endupper_str = NULL;
    char *endlower_str = NULL;
    char *type_str = NULL;        /* Incident type, NOT entity type */
    char *cust_name = NULL;
    char *site_name = NULL;
    char *dev_name = NULL;
    char *cnt_name = NULL;
    char *obj_name = NULL;
    char *met_name = NULL;
    char *trg_name = NULL;
    char *adminstate_str = NULL;
    char *opstate_str = NULL;
    char *caseid_str = NULL;
    int max_count = 200;
    xmlNodePtr node;
    i_callback *cb;
    i_entity_descriptor *entdesc = NULL;

    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;

      /* Check for entity descriptor */
      if (!strcmp((char *)node->name, "entity_descriptor"))
      {
        entdesc = i_entity_descriptor_fromxml (req->xml_in, node);
        if (entdesc)
        {
          cust_name = entdesc->cust_name;
          site_name = entdesc->site_name;
          dev_name = entdesc->dev_name;
          cnt_name = entdesc->cnt_name;
          obj_name = entdesc->obj_name;
          met_name = entdesc->met_name;
          trg_name = entdesc->trg_name;
        }
        continue;
      }

      /* Get and check item string */
      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
      if (!str) continue;

      /* Interpret item */
      if (!strcmp((char *)node->name, "id") && str) id_str = strdup(str);
      else if (!strcmp((char *)node->name, "state") && str) state_str = strdup(str);
      else if (!strcmp((char *)node->name, "startupper") && str) startupper_str = strdup(str);
      else if (!strcmp((char *)node->name, "startlower") && str) startlower_str = strdup(str);
      else if (!strcmp((char *)node->name, "endupper") && str) endupper_str = strdup(str);
      else if (!strcmp((char *)node->name, "endlower") && str) endlower_str = strdup(str);
      else if (!strcmp((char *)node->name, "type") && str) type_str = strdup(str);
      else if (!strcmp((char *)node->name, "caseid") && str) caseid_str = strdup(str);
      else if (!strcmp((char *)node->name, "adminstate_num") && str) adminstate_str = strdup(str);
      else if (!strcmp((char *)node->name, "opstate_num") && str) opstate_str = strdup(str);
      else if (!strcmp((char *)node->name, "max_count") && str) max_count = atoi (str);

      /* Free string */
      xmlFree (str);
    }

    /* Load Incidents */
    cb = l_incident_sql_list (self, id_str, state_str, startupper_str, startlower_str, endupper_str, endlower_str, type_str, cust_name, site_name, dev_name, cnt_name, obj_name, met_name, trg_name, adminstate_str, opstate_str, caseid_str, max_count, l_incident_xml_list_sqlcb, req);
    if (entdesc) i_entity_descriptor_free (entdesc);
    if (id_str) free (id_str);
    if (state_str) free (state_str);
    if (startupper_str) free (startupper_str);
    if (startlower_str) free (startlower_str);
    if (endupper_str) free (endupper_str);
    if (endlower_str) free (endlower_str);
    if (type_str) free (type_str);
    if (caseid_str) free (caseid_str);
    if (adminstate_str) free (adminstate_str);
    if (opstate_str) free (opstate_str);
    
    if (!cb) return -1;
  }
  else
  {
    /* No XML doc provided, no criterial specified.
     * List all active incidents
     */
  
    /* Create XML */
    xml = i_xml_create ();
    if (!xml) 
    { i_printf (1, "xml_incident_list failed to create xml struct"); return -1; }

    /* Create/setup doc */
    xml->doc = xmlNewDoc (BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "incident_list");
    xmlDocSetRootElement(xml->doc, root_node);

    /* Get active incident list */
    inc_list = l_incident_list ();
    
    /* Incident List Version */
    char *str;
    asprintf(&str, "%lu", l_incident_list_version());
    xmlNewChild (root_node, NULL, BAD_CAST "version", BAD_CAST str);
    free (str);

    /* Incident Count */
    asprintf(&str, "%lu", inc_list ? inc_list->size : 0);
    xmlNewChild (root_node, NULL, BAD_CAST "count", BAD_CAST str);
    free (str);

    /* Loops through incident list to add each to the xml */
    for (i_list_move_head(inc_list); (inc=i_list_restore(inc_list)) != NULL; i_list_move_next(inc_list))
    {
      xmlNodePtr inc_node = NULL;

      inc_node = i_incident_xml (inc);
      if (inc_node) 
      {
        /* Add node */
        xmlAddChild (root_node, inc_node);

        /* Append actions */
        l_action *action;
        for (i_list_move_head(inc->action_list); (action=i_list_restore(inc->action_list))!=NULL; i_list_move_next(inc->action_list))
        {
          xmlNodePtr action_node = l_action_xml (action);
          if (action_node)
          { xmlAddChild (inc_node, action_node); }
        }
      }
    }
  
    /* Finished */
    req->xml_out = xml;
  
    return 1;
  }

  return 0;   /* Not finished, waiting for SQL list */
}

int l_incident_xml_list_sqlcb (i_resource *self, i_list *list, void *passdata)
{
  i_xml *xml;
  i_incident *inc;
  xmlNodePtr root_node;
  i_xml_request *req = passdata;

  /* Create XML */
  xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "incident_list");
  xmlDocSetRootElement (xml->doc, root_node);

  /* Add incidents */
  for (i_list_move_head(list); (inc=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    xmlNodePtr inc_node;
    inc_node = i_incident_xml (inc);
    xmlAddChild (root_node, inc_node);
  }

  /* Deliver */
  req->xml_out = xml;
  i_xml_deliver (self, req);

  return -1;  /* Dont keep incident list */
}
