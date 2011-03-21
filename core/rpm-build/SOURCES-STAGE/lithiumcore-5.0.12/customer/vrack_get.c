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

/* VRack Retrieval
 *
 * This function fires a callback and passes it an xmlNodePtr which is
 * the root node of the XML structure describing the vrack
 */

i_callback* l_vrack_get (i_resource *self, int doc_id, int (*cbfunc) (), void *passdata)
{
  /* Create callback struct */
  i_callback *cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;
  
  /* Load Basic info */
  cb->data = l_vrack_sql_get (self, doc_id, l_vrack_get_sqlcb, cb);
  if (!cb->data)
  {
    i_printf (1, "l_vrack_get failed to retrieve vrack id %li from SQL", doc_id);
    i_callback_free (cb);
    return NULL;
  }

  return cb;
}

int l_vrack_get_sqlcb (i_resource *self, l_vrack *vrack, void *passdata)
{
  char *str;
  i_callback *cb = (i_callback *) passdata;

  /* Create XML */
  i_xml *xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "vrack");
  xmlDocSetRootElement (xml->doc, root_node);

  if (vrack)
  {
    /* Add rack Info */
    asprintf (&str, "%i", vrack->doc_id);
    xmlNewChild (root_node, NULL, BAD_CAST "id", BAD_CAST str);
    free (str);
    asprintf (&str, "%i", vrack->ru_count);
    xmlNewChild (root_node, NULL, BAD_CAST "ru_count", BAD_CAST str);
    free (str);
    xmlNewChild (root_node, NULL, BAD_CAST "thumbnail_image", BAD_CAST vrack->thumbnail_data);

    /* Add Devices */
    l_vrack_device *device;
    for (i_list_move_head(vrack->device_list); (device=i_list_restore(vrack->device_list))!=NULL; i_list_move_next(vrack->device_list))
    {
      xmlNodePtr device_node = xmlNewNode (NULL, BAD_CAST "device");

      xmlNewChild (device_node, NULL, BAD_CAST "uuid", BAD_CAST device->uuid_str);

      asprintf (&str, "%.2f", device->color_red);
      xmlNewChild (device_node, NULL, BAD_CAST "color_red", BAD_CAST str);
      free (str);
      asprintf (&str, "%.2f", device->color_green);
      xmlNewChild (device_node, NULL, BAD_CAST "color_green", BAD_CAST str);
      free (str);
      asprintf (&str, "%.2f", device->color_blue);
      xmlNewChild (device_node, NULL, BAD_CAST "color_blue", BAD_CAST str);
      free (str);

      asprintf (&str, "%i", device->ru_index);
      xmlNewChild (device_node, NULL, BAD_CAST "ru_index", BAD_CAST str);
      free (str);
      asprintf (&str, "%i", device->size);
      xmlNewChild (device_node, NULL, BAD_CAST "size", BAD_CAST str);
      free (str);

      if (device->entdesc)
      {
        xmlNodePtr entdesc_node = i_entity_descriptor_xml (device->entdesc);
        xmlAddChild (device_node, entdesc_node);
      }
    
      xmlAddChild (root_node, device_node);
    }
    
    /* Add Cables */
    l_vrack_cable *cable;
    for (i_list_move_head(vrack->cable_list); (cable=i_list_restore(vrack->cable_list))!=NULL; i_list_move_next(vrack->cable_list))
    {
      xmlNodePtr cable_node = xmlNewNode (NULL, BAD_CAST "cable");

      xmlNewChild (cable_node, NULL, BAD_CAST "uuid", BAD_CAST cable->uuid_str);

      asprintf (&str, "%.2f", cable->color_red);
      xmlNewChild (cable_node, NULL, BAD_CAST "color_red", BAD_CAST str);
      free (str);
      asprintf (&str, "%.2f", cable->color_green);
      xmlNewChild (cable_node, NULL, BAD_CAST "color_green", BAD_CAST str);
      free (str);
      asprintf (&str, "%.2f", cable->color_blue);
      xmlNewChild (cable_node, NULL, BAD_CAST "color_blue", BAD_CAST str);
      free (str);

      xmlNewChild (cable_node, NULL, BAD_CAST "vlans", BAD_CAST cable->vlans_str);
      xmlNewChild (cable_node, NULL, BAD_CAST "notes", BAD_CAST cable->notes_str);
      
      asprintf (&str, "%i", cable->locked);
      xmlNewChild (cable_node, NULL, BAD_CAST "locked", BAD_CAST str);
      free (str);

      if (cable->a_int_entdesc)
      {
        xmlNodePtr end_node = xmlNewNode (NULL, BAD_CAST "a_end");
        xmlNodePtr entdesc_node = i_entity_descriptor_xml (cable->a_int_entdesc);
        xmlAddChild (end_node, entdesc_node);
        xmlAddChild (cable_node, end_node);
      }
      if (cable->b_int_entdesc)
      {
        xmlNodePtr end_node = xmlNewNode (NULL, BAD_CAST "b_end");
        xmlNodePtr entdesc_node = i_entity_descriptor_xml (cable->b_int_entdesc);
        xmlAddChild (end_node, entdesc_node);
        xmlAddChild (cable_node, end_node);
      }
    
      xmlAddChild (root_node, cable_node);
    }

    /* Add Cable Groups */
    l_vrack_cablegroup *group;
    for (i_list_move_head(vrack->cablegroup_list); (group=i_list_restore(vrack->cablegroup_list))!=NULL; i_list_move_next(vrack->cablegroup_list))
    {
      xmlNodePtr group_node = xmlNewNode (NULL, BAD_CAST "cablegroup");

      xmlNewChild (group_node, NULL, BAD_CAST "uuid", BAD_CAST group->uuid_str);
      xmlNewChild (group_node, NULL, BAD_CAST "desc", BAD_CAST group->desc_str);

      xmlAddChild (root_node, group_node);
    }
  }

  if (cb->func)
  {
    cb->func (self, xml, cb->passdata);
  }

  /* Clean up */
  i_callback_free (cb);

  return 0;   /* This will free the vrack too */
}
