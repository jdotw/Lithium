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

#include "svcregistry.h"

/* List all registered services */

int xml_svcregistry_list (i_resource *self, i_xml_request *req)
{
  i_xml *xml;
  xmlNodePtr root_node = NULL;

  /* Create XML */
  xml = i_xml_create ();
  if (!xml) 
  { i_printf (1, "xml_incident_list failed to create xml struct"); return -1; }

  /* Create/setup doc */
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "service_list");
  xmlDocSetRootElement(xml->doc, root_node);

  /* Iterate hashtable */
  i_list *svc_list = i_list_create ();
  i_hashtable *obj_table = l_svcregistry_table_object ();
  i_hashtable_iterate (self, obj_table, l_svcregistry_xml_list_iterate, svc_list);

  /* Vendor module list */
  l_svcregistry_entry *svc;
  for (i_list_move_head(svc_list); (svc=i_list_restore(svc_list))!=NULL; i_list_move_next(svc_list))
  {
    xmlNodePtr svc_node = i_entity_descriptor_xml (svc->objent);
    xmlAddChild (root_node, svc_node);
  }
  i_list_free (svc_list);
    
  /* Finished */
  req->xml_out = xml;
  
  return 1;
}

void l_svcregistry_xml_list_iterate (i_resource *self, i_hashtable *obj_table, void *data, void *passdata)
{
  i_list *svc_list = passdata;
  l_svcregistry_entry *entry = data;

  i_list_enqueue (svc_list, entry);
}
