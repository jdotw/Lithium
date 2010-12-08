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
#include <induction/incident.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/timer.h>
#include <induction/xml.h>

#include "case.h"
#include "incident.h"
#include "action.h"

/* Update a device's minimum action severity (WITHOUT RESTARTING IT) */

int xml_action_device_minimum_severity_update (i_resource *self, i_xml_request *req)
{
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;
  int minimum_action_severity = 0;

  if (!req->xml_in) return -1;

  /* Interpret XML */
  root_node = xmlDocGetRootElement (req->xml_in->doc);
  for (node = root_node->children; node; node = node->next)
  {
    char *str;

    str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
    if (!strcmp((char *)node->name, "minimum_action_severity") && str) minimum_action_severity = atoi(str);

    if (str) xmlFree(str);
  }

  /* Update device */
  i_device *dev = (i_device *) i_entity_local_get(self, req->entaddr);
  if (dev)
  {
    /* Update local value */
    dev->minimum_action_severity = minimum_action_severity;

    /* Open conn */
    i_pg_async_conn *conn = i_pg_async_conn_open_customer (self);

    /* Create query */
    char uuid_str[37];
    uuid_unparse_lower (dev->uuid, uuid_str);
    char *query;
    asprintf (&query, "UPDATE devices SET minimum_action_severity='%i' WHERE uuid='%s'", minimum_action_severity, uuid_str);

    /* Execute query */
    int num = i_pg_async_query_exec (self, conn, query, 0, NULL, NULL);
    free (query);
    if (num != 0)
    { i_printf (1, "xml_action_device_minimum_severity_update failed to execute UPDATE"); return -1; }
  }
  else
  {
    i_printf (1, "xml_action_device_minimum_severity_update failed to find specified device entity");
  }

  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "update");
  xmlDocSetRootElement (req->xml_out->doc, root_node);

  return 1;
}

