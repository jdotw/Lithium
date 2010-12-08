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
#include <induction/vendor.h>

#include "xsanregistry.h"

/* List all registered services */

int xml_xsanregistry_list (i_resource *self, i_xml_request *req)
{
  i_xml *xml;
  xmlNodePtr root_node = NULL;

  /* Create XML */
  xml = i_xml_create ();
  if (!xml) 
  { i_printf (1, "xml_incident_list failed to create xml struct"); return -1; }

  /* Create/setup doc */
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "xsan_device_list");
  xmlDocSetRootElement(xml->doc, root_node);

  /* Iterate hashtable */
  i_list *device_list = i_list_create ();
  i_hashtable *device_table = l_xsanregistry_table_device ();
  i_hashtable_iterate (self, device_table, l_xsanregistry_xml_list_iterate, device_list);

  /* Device list */
  l_xsanregistry_device *dev;
  for (i_list_move_head(device_list); (dev=i_list_restore(device_list))!=NULL; i_list_move_next(device_list))
  {
    xmlNodePtr dev_node = xmlNewNode (NULL, BAD_CAST "computer");

    /* Type */
    char *str;
    asprintf (&str, "%i", dev->type);
    xmlNewChild (dev_node, NULL, BAD_CAST "type", BAD_CAST str);
    free (str);

    /* Device Entity */
    xmlNodePtr ent_node = i_entity_descriptor_xml (dev->devent);
    xmlAddChild (dev_node, ent_node);

    /* Loop through volumes */
    l_xsanregistry_entry *vol;
    for (i_list_move_head(dev->entry_list); (vol=i_list_restore(dev->entry_list))!=NULL; i_list_move_next(dev->entry_list))
    {
      xmlNodePtr vol_node = xmlNewNode (NULL, BAD_CAST "volume");

      /* Volume Entity */
      ent_node = i_entity_descriptor_xml (vol->objent);
      xmlAddChild (vol_node, ent_node);
      
      xmlAddChild (dev_node, vol_node);
    }
    
    /* Add dev node to root */ 
    xmlAddChild (root_node, dev_node);
  }
  i_list_free (device_list);
    
  /* Finished */
  req->xml_out = xml;
  
  return 1;
}

void l_xsanregistry_xml_list_iterate (i_resource *self, i_hashtable *obj_table, void *data, void *passdata)
{
  i_list *device_list = passdata;
  l_xsanregistry_device *device = data;

  i_list_enqueue (device_list, device);
}
