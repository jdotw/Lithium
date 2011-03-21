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

/* List of all license keys */

int xml_lic_key_list (i_resource *self, i_xml_request *req)
{
  i_xml *xml;
  xmlNodePtr root_node = NULL;

  /* Create XML */
  xml = i_xml_create ();
  if (!xml) 
  { i_printf (1, "xml_lic_key_list failed to create xml struct"); return -1; }

  /* Create/setup doc */
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "vendor_list");
  xmlDocSetRootElement(xml->doc, root_node);

  /* License list */
  l_lic_key *lic;
  i_list *key_list;
  key_list = l_lic_loadkeys (self);
  for (i_list_move_head(key_list); (lic=i_list_restore(key_list))!=NULL; i_list_move_next(key_list))
  {
    char *str;
    
    xmlNodePtr lic_node = xmlNewNode (NULL, BAD_CAST "license");

    asprintf (&str, "%li", lic->id);
    xmlNewChild (lic_node, NULL, BAD_CAST "id", BAD_CAST str);
    free (str);
    
    asprintf (&str, "%i", lic->status);
    xmlNewChild (lic_node, NULL, BAD_CAST "status", BAD_CAST str);
    free (str);
    
    xmlNewChild (lic_node, NULL, BAD_CAST "enc", BAD_CAST lic->enc_str);
    
    asprintf (&str, "%li", lic->serial);
    xmlNewChild (lic_node, NULL, BAD_CAST "serial", BAD_CAST str);
    free (str);
    
    xmlNewChild (lic_node, NULL, BAD_CAST "type", BAD_CAST lic->type_str);
    xmlNewChild (lic_node, NULL, BAD_CAST "product", BAD_CAST lic->product_str);
    
    asprintf (&str, "%li", lic->volume);
    xmlNewChild (lic_node, NULL, BAD_CAST "volume", BAD_CAST str);
    free (str);
    
    asprintf (&str, "%li", lic->flags);
    xmlNewChild (lic_node, NULL, BAD_CAST "flags", BAD_CAST str);
    free (str);
    
    asprintf (&str, "%li", lic->expiry_sec);
    xmlNewChild (lic_node, NULL, BAD_CAST "expiry", BAD_CAST str);
    free (str);
    
    xmlAddChild (root_node, lic_node);
  }
  i_list_free (key_list);
    
  /* Finished */
  req->xml_out = xml;
  
  return 1;
}


