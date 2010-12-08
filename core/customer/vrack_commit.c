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

#include "vrack.h"

/* VRack Commit
 *
 * This function commits the vrack document data to sql 
 * by deleting the existing and inserting the new
 */

int l_vrack_commit (i_resource *self, int doc_id, i_xml *xml, xmlNodePtr data_node)
{
  /* Parse the XML to create vrack struct */
  l_vrack *vrack = l_vrack_create ();
  vrack->doc_id = doc_id;
  xmlNodePtr node;
  for (node = data_node->children; node; node = node->next)
  {
    char *str = (char *) xmlNodeListGetString (xml->doc, node->xmlChildrenNode, 1);

    /* VRack Properties */
    if (!strcmp((char *)node->name, "ru_count") && str) vrack->ru_count = atoi (str);
    else if (!strcmp((char *)node->name, "thumbnail_image") && str) vrack->thumbnail_data = strdup (str);

    /* VRack Device */
    else if (!strcmp((char *)node->name, "device"))
    {
      l_vrack_device *dev = l_vrack_device_create ();
      if (str) { xmlFree (str); str = NULL; }
      xmlNodePtr dev_node;
      for (dev_node = node->children; dev_node; dev_node = dev_node->next)
      {
        char *str = (char *) xmlNodeListGetString (xml->doc, dev_node->xmlChildrenNode, 1);

        if (!strcmp((char *) dev_node->name, "uuid") && str) dev->uuid_str = strdup(str);
        else if (!strcmp((char *) dev_node->name, "color_red") && str) dev->color_red = atof (str);
        else if (!strcmp((char *) dev_node->name, "color_green") && str) dev->color_green = atof (str);
        else if (!strcmp((char *) dev_node->name, "color_blue") && str) dev->color_blue = atof (str);
        else if (!strcmp((char *) dev_node->name, "ru_index") && str) dev->ru_index = atoi (str);
        else if (!strcmp((char *) dev_node->name, "size") && str) dev->size = atoi (str);
        else if (!strcmp((char *) dev_node->name, "entity_descriptor")) dev->entdesc = i_entity_descriptor_fromxml (xml, dev_node);

        if (str) { xmlFree (str); str = NULL; }
      }

      i_list_enqueue (vrack->device_list, dev);
    }

    /* VRack Cable */
    else if (!strcmp((char *)node->name, "cable"))
    {
      l_vrack_cable *cable = l_vrack_cable_create ();
      if (str) { xmlFree (str); str = NULL; }
      xmlNodePtr cable_node;
      for (cable_node = node->children; cable_node; cable_node = cable_node->next)
      {
        char *str = (char *) xmlNodeListGetString (xml->doc, cable_node->xmlChildrenNode, 1);

        if (!strcmp((char *) cable_node->name, "uuid") && str) cable->uuid_str = strdup(str);
        else if (!strcmp((char *) cable_node->name, "color_red") && str) cable->color_red = atof (str);
        else if (!strcmp((char *) cable_node->name, "color_green") && str) cable->color_green = atof (str);
        else if (!strcmp((char *) cable_node->name, "color_blue") && str) cable->color_blue = atof (str);
        else if (!strcmp((char *) cable_node->name, "vlans") && str) cable->vlans_str = strdup (str);
        else if (!strcmp((char *) cable_node->name, "notes") && str) cable->notes_str = strdup (str);
        else if (!strcmp((char *) cable_node->name, "a_end"))
        {
          xmlNodePtr end_node;
          for (end_node=cable_node->children; end_node; end_node = end_node->next)
          {
            if (!strcmp((char *)end_node->name, "entity_descriptor"))
            { 
              cable->a_int_entdesc = i_entity_descriptor_fromxml (xml, end_node); 
            }
          }
        }
        else if (!strcmp((char *) cable_node->name, "b_end")) 
        {
          xmlNodePtr end_node;
          for (end_node=cable_node->children; end_node; end_node = end_node->next)
          {
            if (!strcmp((char *)end_node->name, "entity_descriptor"))
            { 
              cable->b_int_entdesc = i_entity_descriptor_fromxml (xml, end_node); 
            }
          }
        }
        else if (!strcmp((char *) cable_node->name, "locked") && str) cable->locked = atoi (str);

        if (str) { xmlFree (str); str = NULL; }
      }

      i_list_enqueue (vrack->cable_list, cable);
    }

    /* VRack Cable Group */
    else if (!strcmp((char *)node->name, "cablegroup"))
    {
      l_vrack_cablegroup *group = l_vrack_cablegroup_create ();
      if (str) { xmlFree (str); str = NULL; }
      xmlNodePtr group_node;
      for (group_node = node->children; group_node; group_node = group_node->next)
      {
        char *str = (char *) xmlNodeListGetString (xml->doc, group_node->xmlChildrenNode, 1);

        if (!strcmp((char *) group_node->name, "uuid") && str) group->uuid_str = strdup(str);
        else if (!strcmp((char *) group_node->name, "desc") && str) group->desc_str = strdup (str);

        if (str) { xmlFree (str); str = NULL; }
      }

      i_list_enqueue (vrack->cablegroup_list, group);
    }

    /* Free String */
    if (str) xmlFree(str);
  }

  /* Delete the old data */
  l_vrack_sql_delete (self, doc_id);
  l_vrack_sql_device_delete (self, doc_id);
  l_vrack_sql_cable_delete (self, doc_id);
  l_vrack_sql_cablegroup_delete (self, doc_id);

  /* Commit the new */
  l_vrack_sql_insert (self, vrack);
  l_vrack_device *device;
  for (i_list_move_head(vrack->device_list); (device=i_list_restore(vrack->device_list))!=NULL; i_list_move_next(vrack->device_list))
  {
    l_vrack_sql_device_insert (self, vrack, device);
  }
  l_vrack_cable *cable;
  for (i_list_move_head(vrack->cable_list); (cable=i_list_restore(vrack->cable_list))!=NULL; i_list_move_next(vrack->cable_list))
  {
    l_vrack_sql_cable_insert (self, vrack, cable);
  }
  l_vrack_cablegroup *cablegroup;
  for (i_list_move_head(vrack->cablegroup_list); (cablegroup=i_list_restore(vrack->cablegroup_list))!=NULL; i_list_move_next(vrack->cablegroup_list))
  {
    l_vrack_sql_cablegroup_insert (self, vrack, cablegroup);
  }

  return 0;
}
