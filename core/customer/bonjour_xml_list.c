#include "config.h"
#ifdef HAVE_DNS_SD
#include <netdb.h>
#include <stdio.h>
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
#include <dns_sd.h>

#include "bonjour.h"

/* List all bonjour-discovered services */

int xml_bonjour_list (i_resource *self, i_xml_request *req)
{
  i_xml *xml;
  xmlNodePtr root_node = NULL;

  /* Create XML */
  xml = i_xml_create ();
  if (!xml) 
  { i_printf (1, "xml_bonjour_list failed to create xml struct"); return -1; }

  /* Create/setup doc */
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "bonjour_list");
  xmlDocSetRootElement(xml->doc, root_node);

  /* Bonjour service list */
  i_list *browsers = l_bonjour_browser_list (self);
  l_bonjour_browser *browser;
  for (i_list_move_head(browsers); (browser=i_list_restore(browsers))!=NULL; i_list_move_next(browsers))
  {
    xmlNodePtr browser_node;
    
    browser_node = xmlNewNode (NULL, BAD_CAST "browser");
    xmlNewChild (browser_node, NULL, BAD_CAST "servicename", BAD_CAST browser->service);
    xmlNewChild (browser_node, NULL, BAD_CAST "desc", BAD_CAST browser->desc);

    l_bonjour_service *service;
    for (i_list_move_head(browser->service_list); (service=i_list_restore(browser->service_list))!=NULL; i_list_move_next(browser->service_list))
    {
      xmlNodePtr service_node = xmlNewNode (NULL, BAD_CAST "service");
      xmlNewChild (service_node, NULL, BAD_CAST "name", BAD_CAST service->name);
      xmlNewChild (service_node, NULL, BAD_CAST "regtype", BAD_CAST service->regtype);
      xmlNewChild (service_node, NULL, BAD_CAST "replydomain", BAD_CAST service->replydomain);
      xmlNewChild (service_node, NULL, BAD_CAST "fullname", BAD_CAST service->fullname);
      xmlNewChild (service_node, NULL, BAD_CAST "hosttarget", BAD_CAST service->hosttarget);
      char *str;
      asprintf (&str, "%i", (int) service->port);
      xmlNewChild (service_node, NULL, BAD_CAST "port", BAD_CAST str);
      free (str);
      asprintf (&str, "%i", service->current);
      xmlNewChild (service_node, NULL, BAD_CAST "current", BAD_CAST str);
      free (str);
      xmlNewChild (service_node, NULL, BAD_CAST "ip", BAD_CAST service->ip);
      xmlAddChild (browser_node, service_node);
    }
    
    xmlAddChild (root_node, browser_node);
  }
    
  /* Finished */
  req->xml_out = xml;
  
  return 1;
}

#endif
