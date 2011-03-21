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

/* Scene Retrieval
 *
 * This functio fires a callback and passes it an xmlNodePtr which is
 * the root node of the XML structure describing the overlay
 */

i_callback* l_scene_get (i_resource *self, int doc_id, int (*cbfunc) (), void *passdata)
{
  /* Create callback struct */
  i_callback *cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;
  
  /* Load Basic info */
  cb->data = l_scene_sql_get (self, doc_id, l_scene_get_sqlcb, cb);
  if (!cb->data)
  {
    i_printf (1, "l_scene_get failed to retrieve scene id %li from SQL, doc_id");
    i_callback_free (cb);
    return NULL;
  }

  return cb;
}

int l_scene_get_sqlcb (i_resource *self, l_scene *scene, void *passdata)
{
  char *str;
  i_callback *cb = (i_callback *) passdata;

  /* Create XML */
  i_xml *xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "scene");
  xmlDocSetRootElement (xml->doc, root_node);

  if (scene)
  {
    /* Add Scene Info */
    asprintf (&str, "%i", scene->doc_id);
    xmlNewChild (root_node, NULL, BAD_CAST "id", BAD_CAST str);
    free (str);
    xmlNewChild (root_node, NULL, BAD_CAST "background_image", BAD_CAST scene->background_data);

    /* Add Overlays */
    l_scene_overlay *overlay;
    for (i_list_move_head(scene->overlay_list); (overlay=i_list_restore(scene->overlay_list))!=NULL; i_list_move_next(scene->overlay_list))
    {
      xmlNodePtr overlay_node = xmlNewNode (NULL, BAD_CAST "overlay");

      xmlNewChild (overlay_node, NULL, BAD_CAST "uuid", BAD_CAST overlay->uuid_str);

      asprintf (&str, "%.2f", overlay->x);
      xmlNewChild (overlay_node, NULL, BAD_CAST "x", BAD_CAST str);
      free (str);
      asprintf (&str, "%.2f", overlay->y);
      xmlNewChild (overlay_node, NULL, BAD_CAST "y", BAD_CAST str);
      free (str);
      asprintf (&str, "%.2f", overlay->width);
      xmlNewChild (overlay_node, NULL, BAD_CAST "width", BAD_CAST str);
      free (str);
      asprintf (&str, "%.2f", overlay->height);
      xmlNewChild (overlay_node, NULL, BAD_CAST "height", BAD_CAST str);
      free (str);

      asprintf (&str, "%i", overlay->autosize);
      xmlNewChild (overlay_node, NULL, BAD_CAST "autosize", BAD_CAST str);
      free (str);

      if (overlay->entdesc)
      {
        xmlNodePtr entdesc_node = i_entity_descriptor_xml (overlay->entdesc);
        xmlAddChild (overlay_node, entdesc_node);
      }
    
      xmlAddChild (root_node, overlay_node);
    }
  }

  if (cb->func)
  {
    cb->func (self, xml, cb->passdata);
  }

  /* Clean up */
  i_callback_free (cb);

  return 0;   /* This will free the scene too */
}
