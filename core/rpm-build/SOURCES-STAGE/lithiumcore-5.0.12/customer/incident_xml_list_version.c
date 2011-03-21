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
#include <induction/incident.h>
#include <induction/incident_xml.h>

#include "device.h"
#include "navtree.h"
#include "site.h"
#include "case.h"
#include "action.h"
#include "incident.h"

/* Returns the incident list version and count of incidents */

int xml_incident_list_version (i_resource *self, i_xml_request *req)
{
  i_list *inc_list = l_incident_list();

  /* Create XML */
  i_xml *xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "incident_list");
  xmlDocSetRootElement(xml->doc, root_node);

  /* Incident List Version */
  char *str;
  asprintf(&str, "%lu", l_incident_list_version());
  xmlNewChild (root_node, NULL, BAD_CAST "version", BAD_CAST str);
  free (str);

  /* Incident Count */
  asprintf(&str, "%lu", inc_list ? inc_list->size : 0);
  xmlNewChild (root_node, NULL, BAD_CAST "count", BAD_CAST str);
  free (str);

  /* Finished */
  req->xml_out = xml;
  
  return 1;
}

