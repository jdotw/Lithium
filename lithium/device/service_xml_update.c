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

#include "service.h"

/* Add an Action*/

int xml_service_update (i_resource *self, i_xml_request *req)
{
  int num;
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;

  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  if (!req->xml_in) return -1;

  /* Interpret XML */
  l_service *service = l_service_create ();
  root_node = xmlDocGetRootElement (req->xml_in->doc);
  for (node = root_node->children; node; node = node->next)
  {
    char *str;

    str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
    if (!strcmp((char *)node->name, "id") && str) service->id = atoi (str);
    else if (!strcmp((char *)node->name, "desc") && str) service->desc_str = strdup (str);
    else if (!strcmp((char *)node->name, "enabled") && str) service->enabled = atoi (str);
    else if (!strcmp((char *)node->name, "script_file") && str) service->script_file = strdup (str);
    else if (!strcmp((char *)node->name, "config_variable"))
    { 
      l_service_configvar *var = l_service_configvar_fromxml (req->xml_in, node);
      if (var)
      {
        i_list_enqueue (service->configvar_list, var); 
      }
    }

    xmlFree (str);
  }

  /* Add/Update service (and assign id) */
  if (service->id == 0)
  {
    /* Insert */
    num = l_service_sql_insert (self, service);
    if (num != 0)
    { i_printf (1, "xml_service_update failed to insert new service"); return -1; }
    
    /* Create object */
    l_service_object_add (self, service);
    
    /* Add to list */
    i_list *service_list = l_service_list ();
    i_list_enqueue (service_list, service);
  }
  else
  {
    /* Update SQL */
    num = l_service_sql_update (self, service);
    if (num != 0)
    { i_printf (1, "xml_service_update failed to update existing"); return -1; }
    
    /* Update object */
    l_service_object_update (self, service);
  }

  /* Process configvars */
  if (service->configvar_list->size > 0)
  {
    /* Remove the old */
    l_service_sql_configvar_delete (self, service->id);

    /* Add new */
    l_service_configvar *var;
    for (i_list_move_head(service->configvar_list); (var=i_list_restore(service->configvar_list))!=NULL; i_list_move_next(service->configvar_list))
    {
      num = l_service_sql_configvar_insert (self, service->id, var);
      if (num != 0)
      { i_printf (1, "xml_service_update warning failed to add a configvar to service %li", service->id); }
    }
  }

  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "service_list");
  xmlDocSetRootElement (req->xml_out->doc, root_node);
  xmlNodePtr service_node = l_service_xml (service);
  xmlAddChild (root_node, service_node);

  return 1; 
}

int xml_service_add (i_resource *self, i_xml_request *req)
{
  return xml_service_update (self, req);
}
