#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/callback.h>
#include <induction/list.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/xml.h>
#include <induction/entity_xml.h>
#include <induction/auth.h>

#include "group.h"

/*
 * Group Entity Manipulation
 */

int xml_group_entity_add (i_resource *self, i_xml_request *req)
{
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_STAFF)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  l_group_entity *g_ent = l_group_entity_create ();
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
 
      if (!strcmp((char *)node->name, "parent") && str) g_ent->parent_id = atoi (str);
      else if (!strcmp((char *)node->name, "entity_descriptor"))
      {
        g_ent->entdesc = i_entity_descriptor_fromxml (req->xml_in, node);
      }

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_group_entity_add failed, no xml data received");
    return -1; 
  }

  /* Insert Entity */
  l_group_sql_entity_insert (self, g_ent);

  /* Clean up */
  l_group_entity_free (g_ent);

  return 1;
}

int xml_group_entity_remove (i_resource *self, i_xml_request *req)
{
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_STAFF)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  l_group_entity *g_ent = l_group_entity_create ();
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

      if (!strcmp((char *)node->name, "parent") && str) g_ent->parent_id = atoi (str);
      else if (!strcmp((char *)node->name, "entity_descriptor"))
      {
        g_ent->entdesc = i_entity_descriptor_fromxml (req->xml_in, node);
      } 

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_group_entity_add failed, no xml data received");
    return -1; 
  }

  /* Insert Entity */
  l_group_sql_entity_delete (self, g_ent);

  /* Clean up */
  l_group_entity_free (g_ent);

  return 1;
}

int xml_group_entity_move (i_resource *self, i_xml_request *req)
{
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_STAFF)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  l_group_entity *g_ent = l_group_entity_create ();
  int prev_parent = 0;
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

      if (!strcmp((char *)node->name, "parent") && str) g_ent->parent_id = atoi (str);
      if (!strcmp((char *)node->name, "prev_parent") && str) prev_parent = atoi (str);
      else if (!strcmp((char *)node->name, "entity_descriptor"))
      {
        g_ent->entdesc = i_entity_descriptor_fromxml (req->xml_in, node);
      }

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_group_entity_add failed, no xml data received");
    return -1;
  }

  /* Insert Entity */
  l_group_sql_entity_move (self, prev_parent, g_ent);

  /* Clean up */
  l_group_entity_free (g_ent);

  return 1;
}

