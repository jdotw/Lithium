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
#include <induction/auth.h>

#include "action.h"

/* Add/Update an Action*/

int xml_action_update (i_resource *self, i_xml_request *req)
{
  int num;
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;

  if (!req->xml_in) return -1;

  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  l_action *action = l_action_create ();
  root_node = xmlDocGetRootElement (req->xml_in->doc);
  for (node = root_node->children; node; node = node->next)
  {
    char *str;

    str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
    if (!strcmp((char *)node->name, "id") && str) action->id = atoi (str);
    else if (!strcmp((char *)node->name, "desc") && str) action->desc_str = strdup (str);
    else if (!strcmp((char *)node->name, "enabled") && str) action->enabled = atoi (str);
    else if (!strcmp((char *)node->name, "activation") && str) action->activation = atoi (str);
    else if (!strcmp((char *)node->name, "delay") && str) action->delay = atol (str);
    else if (!strcmp((char *)node->name, "rerun") && str) action->rerun = atoi (str);
    else if (!strcmp((char *)node->name, "rerun_delay") && str) action->rerun_delay = atol (str);
    else if (!strcmp((char *)node->name, "time_filter") && str) action->time_filter = atoi (str);
    else if (!strcmp((char *)node->name, "day_mask") && str) action->day_mask = atoi (str);
    else if (!strcmp((char *)node->name, "start_hour") && str) action->start_hour = atoi (str);
    else if (!strcmp((char *)node->name, "end_hour") && str) action->end_hour = atoi (str);
    else if (!strcmp((char *)node->name, "script_file") && str) action->script_file = strdup (str);
    else if (!strcmp((char *)node->name, "log_output") && str) action->log_output = atoi (str);
    else if (!strcmp((char *)node->name, "config_variable"))
    { 
      l_action_configvar *var = l_action_configvar_fromxml (req->xml_in, node);
      if (var)
      {
        i_list_enqueue (action->configvar_list, var); 
      }
    }
    else if (!strcmp((char *)node->name, "entity_descriptor"))
    {
      i_entity_descriptor *entdesc = i_entity_descriptor_fromxml (req->xml_in, node);
      if (entdesc)
      { i_list_enqueue (action->entity_list, entdesc); }
    }

    xmlFree (str);
  }

  /* Add action (and assign id) */
  if (action->id == 0)
  {
    num = l_action_sql_insert (self, action);
    if (num != 0)
    { i_printf (1, "xml_action_update failed to insert new action"); return -1; }
  }
  else
  {
    num = l_action_sql_update (self, action);
    if (num != 0)
    { i_printf (1, "xml_action_update failed to update existing action"); return -1; }
  }

  /* Process Config Variables */
  if (action->configvar_list->size > 0)
  { 
    /* New variables received, remove the old */
    l_action_sql_configvar_delete (self, action->id);

    /* Insert the new */
    l_action_configvar *var;
    for (i_list_move_head(action->configvar_list); (var=i_list_restore(action->configvar_list))!=NULL; i_list_move_next(action->configvar_list))
    {
      num = l_action_sql_configvar_insert (self, action->id, var);
      if (num != 0)
      { i_printf (1, "xml_action_update warning failed to add a configvar to action %li", action->id); }
    }
  }

  /* Process entities */
  if (action->entity_list->size > 0)
  {
    /* New entities received, remove the old */
    l_action_sql_entity_delete (self, action->id);

    /* Insert new */
    i_entity_descriptor *entdesc;
    for (i_list_move_head(action->entity_list); (entdesc=i_list_restore(action->entity_list))!=NULL; i_list_move_next(action->entity_list))
    {
      num = l_action_sql_entity_insert (self, action->id, entdesc);
      if (num != 0)
      { i_printf (1, "xml_action_update warning failed to add an entity to action %li", action->id); }
    }
  }

  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "action_list");
  xmlDocSetRootElement (req->xml_out->doc, root_node);
  xmlNodePtr action_node = l_action_xml (action);
  xmlAddChild (root_node, action_node);

  /* Clean up */
  l_action_free (action);

  return 1; 
}

int xml_action_add (i_resource *self, i_xml_request *req)
{
  return xml_action_update (self, req);
}


