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

#include "device.h"
#include "navtree.h"
#include "site.h"
#include "case.h"

/* List all vendors */

int xml_vendor_list (i_resource *self, i_xml_request *req)
{
  i_xml *xml;
  i_vendor *vendor;
  i_list *vendor_list;
  xmlNodePtr root_node = NULL;

  /* Create XML */
  xml = i_xml_create ();
  if (!xml) 
  { i_printf (1, "xml_incident_list failed to create xml struct"); return -1; }

  /* Create/setup doc */
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "vendor_list");
  xmlDocSetRootElement(xml->doc, root_node);

  /* Vendor module list */
  vendor_list = i_vendor_list (self);
  for (i_list_move_head(vendor_list); (vendor=i_list_restore(vendor_list))!=NULL; i_list_move_next(vendor_list))
  {
    xmlNodePtr vendor_node;
    
    vendor_node = xmlNewNode (NULL, BAD_CAST "vendor_module");
    xmlNewChild (vendor_node, NULL, BAD_CAST "name", BAD_CAST vendor->name_str);
    xmlNewChild (vendor_node, NULL, BAD_CAST "desc", BAD_CAST vendor->desc_str);
    
    xmlAddChild (root_node, vendor_node);
  }
    
  /* Finished */
  if (vendor_list) i_list_free (vendor_list);
  req->xml_out = xml;
  
  return 1;
}

