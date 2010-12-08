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

#include "scene.h"

/* Scene Commit
 *
 * This function commits the scene document data to sql 
 * by deleting the existing and inserting the new
 */

int l_scene_commit (i_resource *self, int doc_id, i_xml *xml, xmlNodePtr data_node)
{
  /* Parse the XML to create scene struct */
  l_scene *scene = l_scene_create ();
  scene->doc_id = doc_id;
  xmlNodePtr node;
  for (node = data_node->children; node; node = node->next)
  {
    char *str = (char *) xmlNodeListGetString (xml->doc, node->xmlChildrenNode, 1);

    if (!strcmp((char *)node->name, "background_image") && str) scene->background_data = strdup(str);
    else if (!strcmp((char *)node->name, "overlay"))
    {
      /* Parse an overlay */
      l_scene_overlay *ol = l_scene_overlay_create ();
      if (str) { xmlFree (str); str = NULL; }
      xmlNodePtr ol_node;
      for (ol_node = node->children; ol_node; ol_node = ol_node->next)
      {
        char *str = (char *) xmlNodeListGetString (xml->doc, ol_node->xmlChildrenNode, 1);

        if (!strcmp((char *) ol_node->name, "uuid") && str) ol->uuid_str = strdup(str);
        else if (!strcmp((char *) ol_node->name, "x") && str) ol->x = atof (str);
        else if (!strcmp((char *) ol_node->name, "y") && str) ol->y = atof (str);
        else if (!strcmp((char *) ol_node->name, "width") && str) ol->width = atof (str);
        else if (!strcmp((char *) ol_node->name, "height") && str) ol->height = atof (str);
        else if (!strcmp((char *) ol_node->name, "autosize") && str) ol->autosize = atoi (str);
        else if (!strcmp((char *) ol_node->name, "entity_descriptor")) ol->entdesc = i_entity_descriptor_fromxml (xml, ol_node);

        if (str) { xmlFree (str); str = NULL; }
      }

      i_list_enqueue (scene->overlay_list, ol);
    }

    if (str) xmlFree(str);
  }

  /* Delete the old data */
  l_scene_sql_delete (self, doc_id);
  l_scene_sql_overlay_delete (self, doc_id);

  /* Commit the new */
  l_scene_sql_insert (self, scene);
  l_scene_overlay *ol;
  for (i_list_move_head(scene->overlay_list); (ol=i_list_restore(scene->overlay_list))!=NULL; i_list_move_next(scene->overlay_list))
  {
    l_scene_sql_overlay_insert (self, scene, ol);
  }

  return 0;
}
