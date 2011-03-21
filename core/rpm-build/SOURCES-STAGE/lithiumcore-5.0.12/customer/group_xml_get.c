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

#include "scene.h"
#include "vrack.h"
#include "group.h"

/*
 * Retrieve a group
 */

int l_group_xml_listcb (i_resource *self, i_list *group_list, i_list *entity_list, void *passdata);

int xml_group_list (i_resource *self, i_xml_request *req)
{
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_CLIENT)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  int use_group_id = 0;
  int group_id = 0;
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

      if (!strcmp((char *)node->name, "id") && str) 
      {
        group_id = atoi (str);
        use_group_id = 1;
      }

      xmlFree (str);
    }
  }

  /* Get group/entity list */
  i_callback *cb = l_group_sql_load_list (self, group_id, use_group_id, l_group_xml_listcb, req);
  if (!cb) return -1;

  return 0;
}

int l_group_xml_listcb (i_resource *self, i_list *group_list, i_list *entity_list, void *passdata)
{
  char *str;
  i_xml_request *req = passdata;

  /* Create outbound XML */
  i_xml *xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "groups");
  xmlDocSetRootElement (xml->doc, root_node);
  req->xml_out = xml;

  /* Add Groups */
  l_group *group;
  for (i_list_move_head(group_list); (group=i_list_restore(group_list))!=NULL; i_list_move_next(group_list))
  {
    xmlNodePtr group_node = xmlNewNode (NULL, BAD_CAST "group");

    asprintf (&str, "%i", group->id);
    xmlNewChild (group_node, NULL, BAD_CAST "id", BAD_CAST str);
    free (str);
    xmlNewChild (group_node, NULL, BAD_CAST "desc", BAD_CAST group->desc_str);
    asprintf (&str, "%i", group->parent_id);
    xmlNewChild (group_node, NULL, BAD_CAST "parent", BAD_CAST str);
    free (str);

    xmlAddChild (root_node, group_node); 
  }

  /* Add Entities */
  l_group_entity *g_ent;
  for (i_list_move_head(entity_list); (g_ent=i_list_restore(entity_list))!=NULL; i_list_move_next(entity_list))
  {
    xmlNodePtr entity_node = xmlNewNode (NULL, BAD_CAST "entity");

    asprintf (&str, "%i", g_ent->parent_id);
    xmlNewChild (entity_node, NULL, BAD_CAST "parent", BAD_CAST str);
    free (str);

    /* Since 5.0.9, there's no full entitiy tree kept in
     * the customer process, so the entdesc is used 
     * instead of doing a local entity look up
     */
    
    xmlAddChild (entity_node, i_entity_descriptor_xml (g_ent->entdesc));

    xmlAddChild (root_node, entity_node); 
  }

  /* Deliver */
  req->xml_out = xml;
  i_xml_deliver (self, req);

  return 0;
}
