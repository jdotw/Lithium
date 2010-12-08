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
#include "netscan.h"

int l_ipregistry_xml_network_scancb (i_resource *self, l_netscan_proc *proc, void *passdata);

int xml_ipregistry_network_scan (i_resource *self, i_xml_request *req)
{
  char *network_str = NULL;
  char *mask_str = NULL;
  
  /* Interpret XML */
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

      if (!strcmp((char *)node->name, "address") && str) network_str = strdup(str);
      if (!strcmp((char *)node->name, "mask") && str) mask_str = strdup(str);

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_ipregistry_network_scan failed, no xml data received");
    return -1;
  }
  if (!network_str)
  { i_printf (1, "xml_ipregistry_network_scan failed, no network specified"); return -1; }

  /* Scan */
  l_netscan_scan (self, network_str, mask_str, l_ipregistry_xml_network_scancb, req);
  
  return 0;
}

int l_ipregistry_xml_network_scancb (i_resource *self, l_netscan_proc *proc, void *passdata)
{
  i_xml_request *req = passdata;
  
  xml_ipregistry_network_list (self, req);
  i_xml_deliver (self, req);

  return 0;
}
