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
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/timer.h>
#include <induction/xml.h>
#include <induction/entity_xml.h>
#include <induction/auth.h>

#include "device.h"

int xml_device_move (i_resource *self, i_xml_request *req)
{
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  if (req->xml_in)
  {
    /* Create return XML */
    req->xml_out = i_xml_create ();
    req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
    xmlNodePtr return_root_node = xmlNewNode(NULL, BAD_CAST "device_move_results");
    xmlDocSetRootElement(req->xml_out->doc, return_root_node);

    /* Parse XML */
    xmlNodePtr dev_node;
    xmlNodePtr root_node = NULL;
    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (dev_node = root_node->children; dev_node; dev_node = dev_node->next)
    {
      if (!strcmp((char *)dev_node->name, "device"))
      {
        i_device *device = NULL;
        i_site *dest_site = NULL;

        /* Interpret site+device pair */
        xmlNodePtr node;
        for (node = dev_node->children; node; node = node->next)
        {
          char *str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
 
          if (!strcmp((char *)node->name, "dest_site_entaddr") && str)
          {
            i_entity_address *entaddr = i_entity_address_struct (str);
            if (!entaddr) 
            { i_printf (1, "xml_device_move received bad site entity address"); return -1; }

            dest_site = (i_site *) i_entity_local_get (self, entaddr);
            i_entity_address_free (entaddr);
            if (!device)
            { i_printf (1, "xml_device_move specified site not found"); return -1; }
          }
          else if (!strcmp((char *)node->name, "dev_entaddr") && str) 
          {
            i_entity_address *entaddr = i_entity_address_struct (str);
            if (!entaddr) 
            { i_printf (1, "xml_device_move received bad device entity address"); return -1; }

            device = (i_device *) i_entity_local_get (self, entaddr);
            i_entity_address_free (entaddr);
            if (!device)
            { i_printf (1, "xml_device_move specified device not found"); return -1; }
          }

          xmlFree (str);
        }

        /* Check for site and device */
        if (!dest_site || !device)
        {
          i_printf (1, "xml_device_move warning, destination site or device missing from atleast one device"); 
          continue;
        }

        /* Perform the move */
        int result = l_device_move (self, device, dest_site);
        char *dev_entaddr_str = i_entity_address_string (ENTITY(device), NULL);
        char *result_str;
        asprintf (&result_str, "%i", result);
        xmlNewChild (root_node, NULL, BAD_CAST dev_entaddr_str, BAD_CAST result_str);
        free (result_str);
        free (dev_entaddr_str);
      }
    }
  }
  else
  {
    i_printf (1, "xml_device_update, no xml data received");
    return -1; 
  }

  return 1;
}

