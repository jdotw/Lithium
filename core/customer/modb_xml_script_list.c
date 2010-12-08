#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <sys/types.h>
#include <dirent.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/path.h>
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
#include <induction/vendor.h>
#include <induction/xml.h>

#include "script.h"
#include "action.h"

/* List of all modb scripts present on the system */

int xml_modb_script_list (i_resource *self, i_xml_request *req)
{
  /* Walks the vendor module directory and produces an
   * XML list of module builder modules
   */

  /* Create XML */
  i_xml *xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "script_list");
  xmlDocSetRootElement (xml->doc, root_node);
  req->xml_out = xml;

  /* Get Vendor List */
  i_list *list = i_vendor_list (self);

  /* Add scripts */
  i_vendor *vendor;
  for (i_list_move_head(list); (vendor=i_list_restore(list))!=NULL; i_list_move_next(list))
  {      
    if (!strstr(vendor->module_str, ".xml")) continue;
    
    xmlNodePtr script_node = xmlNewNode (NULL, BAD_CAST "script");

    xmlNewChild (script_node, NULL, BAD_CAST "name", BAD_CAST vendor->name_str);
    xmlNewChild (script_node, NULL, BAD_CAST "desc", BAD_CAST vendor->desc_str);
    xmlNewChild (script_node, NULL, BAD_CAST "info", BAD_CAST vendor->desc_str);
    xmlNewChild (script_node, NULL, BAD_CAST "status", BAD_CAST "1");
    
    xmlAddChild (root_node, script_node);
  }

  /* Cleanup */
  i_list_free (list);

  return 1;
}

