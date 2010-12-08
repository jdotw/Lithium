#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <libxml/parser.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

#include <induction.h>
#include <induction/callback.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/hashtable.h>
#include <induction/list.h>
#include <induction/postgresql.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/entity.h>
#include <induction/entity_xml.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/incident.h>
#include <induction/xml.h>

#include "ipregistry.h"

void l_ipregistry_xml_network_list_iterate (i_resource *self, i_hashtable *network_table, void *data, void *passdata);

int xml_ipregistry_network_list (i_resource *self, i_xml_request *req)
{
  char *str;
  
  /* Create return XML */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode ( NULL, BAD_CAST "network_list");
  xmlDocSetRootElement (req->xml_out->doc, root_node);

  /* Get network table */
  i_hashtable *network_table = l_ipregistry_table_network ();
  i_list *list = i_list_create ();
  i_hashtable_iterate (self, network_table, l_ipregistry_xml_network_list_iterate, list);

  /* Loop through networks */
  l_ipregistry_network *network;
  for (i_list_move_head(list); (network=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    /* Add XML for network */
    xmlNodePtr network_node = xmlNewNode ( NULL, BAD_CAST "network_entry");    
    if (~network->maskaddr.s_addr == 0)
    { asprintf (&str, "%s/32", network->network_str); }
    else
    { asprintf (&str, "%s/%.0f", network->network_str, 32 - (log(ntohl(~network->maskaddr.s_addr)) / log(2))); }
    xmlNewChild (network_node, NULL, BAD_CAST "address", BAD_CAST str);

    /* Loop through entries */
    l_ipregistry_entry *entry;
    for (i_list_move_head(network->entry_list); (entry=i_list_restore(network->entry_list))!=NULL; i_list_move_next(network->entry_list))
    {
      /* Add XML for IP address */
      xmlNodePtr entry_node = xmlNewNode ( NULL, BAD_CAST "ip_entry");
      xmlNewChild (entry_node, NULL, BAD_CAST "ip", BAD_CAST entry->ip_str);
      xmlNewChild (entry_node, NULL, BAD_CAST "network", BAD_CAST entry->network_str);
      xmlNewChild (entry_node, NULL, BAD_CAST "mask", BAD_CAST entry->mask_str);
      xmlNewChild (entry_node, NULL, BAD_CAST "dns", BAD_CAST entry->dns_str);
      asprintf (&str, "%i", entry->bound);
      xmlNewChild (entry_node, NULL, BAD_CAST "bound", BAD_CAST str);
      free (str);

      /* Iterate through devices */
      i_list *dev_list = i_list_create ();
      i_hashtable_iterate (self, entry->device_table, l_ipregistry_xml_network_list_iterate, dev_list);
      l_ipregistry_entry_device *dev;
      for (i_list_move_head(dev_list); (dev=i_list_restore(dev_list))!=NULL; i_list_move_next(dev_list))
      {
        /* Add device entry */
        xmlNodePtr device_node = xmlNewNode (NULL, BAD_CAST "device_entry");
        if (dev->ipent)
        {
          xmlNodePtr ipent_node = xmlNewNode (NULL, BAD_CAST "ip_entity");
          xmlNodePtr desc_node = i_entity_descriptor_xml (dev->ipent);
          xmlAddChild (ipent_node, desc_node);
          xmlAddChild (device_node, ipent_node);
        }
        if (dev->iface)
        {
          xmlNodePtr iface_node = xmlNewNode (NULL, BAD_CAST "iface_entity");
          xmlNodePtr desc_node = i_entity_descriptor_xml (dev->iface);
          xmlAddChild (iface_node, desc_node);
          xmlAddChild (device_node, iface_node);
        }
        xmlAddChild (entry_node, device_node);
      }

      xmlAddChild (network_node, entry_node);
    }
    
    xmlAddChild (root_node, network_node);
  }
  
  return 1;
}

void l_ipregistry_xml_network_list_iterate (i_resource *self, i_hashtable *network_table, void *data, void *passdata)
{
  i_list *list = passdata;
  i_list_enqueue (list, data);
}
