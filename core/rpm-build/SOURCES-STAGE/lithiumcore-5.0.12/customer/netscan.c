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
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/incident.h>
#include <induction/xml.h>

#include "ipregistry.h"
#include "netscan.h"

/* nmap XML Parsing */

int l_netscan_parse (i_resource *self, char *network_str, char *mask_str, char *xml_str)
{
  if (!xml_str || strlen(xml_str) < 1) return -1;
  xmlDocPtr xml = xmlReadMemory (xml_str, strlen(xml_str), "nmap", "UTF-8", 0);
  if (!xml) return -1;
  xmlNodePtr root_node = xmlDocGetRootElement (xml);
  if (!root_node) return -1;
  xmlNodePtr node;
  for (node=root_node->children; node; node = node->next)
  {
    if (strcmp((char *)node->name, "host") == 0)
    {
      /* Loops for each "host" entry */
      xmlNodePtr hostnode;
      l_ipregistry_entry *entry = NULL;
      for (hostnode = node->children; hostnode; hostnode = hostnode->next)
      {
        /* Address */
        if (strcmp((char *)hostnode->name, "address") == 0 && 
          strcmp(l_netscan_attr((char *)hostnode, "addrtype"), "ipv4") == 0)
        {
          /* Got IPv4 Address, find/create entry */
          char *ip_str = l_netscan_attr(hostnode,"addr");
          entry = l_ipregistry_get_ip_str (self, ip_str);
          if (!entry)
          {
            entry = l_ipregistry_add (self, ip_str, mask_str);
            if (!entry) continue;
          }
        }

        /* Hostnames */
        if (strcmp((char *)hostnode->name, "hostnames") == 0)
        {
          xmlNodePtr namenode;
          for (namenode = hostnode->children; namenode; namenode=namenode->next)
          {
            if (strcmp((char *)namenode->name, "hostname") != 0) continue;
            char *hostname = l_netscan_attr (namenode, "name");
            if (hostname) 
            { l_ipregistry_set_dns (self, entry, hostname); }
          }
        }
      }
    }
  }

  return 0;
}

char* l_netscan_attr (xmlNodePtr node, char *name)
{
  /* Loop through the elements properties */
  xmlAttrPtr attr;
  for (attr=node->properties; attr; attr = attr->next)
  {
    if (strcmp((char *)attr->name, name) == 0)
    {
      char *str = (char *) xmlNodeListGetString (node->doc, attr->xmlChildrenNode, 1);
      return str;
    }
  }

  return NULL;
}

