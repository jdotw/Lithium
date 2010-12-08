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

#include "lic.h"

/* License entitlements */

int xml_lic_entitlement (i_resource *self, i_xml_request *req)
{
  i_xml *xml;
  xmlNodePtr root_node = NULL;

  /* Create XML */
  xml = i_xml_create ();

  /* Create/setup doc */
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "entitlements");
  xmlDocSetRootElement(xml->doc, root_node);

  /* License Entitlements */
  char *str;
  l_lic_entitlement *licent = l_lic_static_entitlement();

  asprintf (&str, "%i", licent->customer_licensed);
  xmlNewChild (root_node, NULL, BAD_CAST "customer_licensed", BAD_CAST str);
  free (str);
  
  asprintf (&str, "%li", licent->devices_max);
  xmlNewChild (root_node, NULL, BAD_CAST "devices_max", BAD_CAST str);
  free (str);
  
  asprintf (&str, "%li", licent->devices_used);
  xmlNewChild (root_node, NULL, BAD_CAST "devices_used", BAD_CAST str);
  free (str);
  
  asprintf (&str, "%li", licent->devices_excess);
  xmlNewChild (root_node, NULL, BAD_CAST "devices_excess", BAD_CAST str);
  free (str);

  asprintf (&str, "%li", licent->expiry_sec);
  xmlNewChild (root_node, NULL, BAD_CAST "expiry", BAD_CAST str);
  free (str);

  xmlNewChild (root_node, NULL, BAD_CAST "type", BAD_CAST licent->type_str);

  asprintf (&str, "%i", licent->nfr);
  xmlNewChild (root_node, NULL, BAD_CAST "nfr", BAD_CAST str);
  free (str);

  asprintf (&str, "%i", licent->demo);
  xmlNewChild (root_node, NULL, BAD_CAST "demo", BAD_CAST str);
  free (str);

  asprintf (&str, "%i", licent->free);
  xmlNewChild (root_node, NULL, BAD_CAST "free", BAD_CAST str);
  free (str);

  asprintf (&str, "%i", licent->limited);
  xmlNewChild (root_node, NULL, BAD_CAST "limited", BAD_CAST str);
  free (str);

  /* Finished */
  req->xml_out = xml;
  
  return 1;
}


