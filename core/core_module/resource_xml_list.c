#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/construct.h>
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
#include <induction/auth.h>
#include <induction/contact.h>
#include <induction/user.h>
#include <induction/configfile.h>
#include <induction/files.h>

extern i_hashtable* global_res_table;
extern struct timeval global_start_tv;
static i_list *res_list;

void l_resource_xml_list_iterate (i_resource *self, i_hashtable *res_table, void *data)
{ 
  i_list_enqueue (res_list, data);
} 

int xml_resource_list (i_resource *self, i_xml_request *req)
{
  i_xml *xml;
  xmlNodePtr root_node = NULL;
  char *str;

  /* Create XML */
  xml = i_xml_create ();
  if (!xml) 
  { i_printf (1, "xml_incident_list failed to create xml struct"); return -1; }

  /* Create/setup doc */
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "resource_list");
  xmlDocSetRootElement(xml->doc, root_node);

  /* Core Info */
  asprintf (&str, "%li", global_start_tv.tv_sec);
  xmlNewChild (root_node, NULL, BAD_CAST "starttime", BAD_CAST str);
  free (str);

  /* Create resource list */
  res_list = i_list_create (); 
  i_hashtable_iterate (self, global_res_table, l_resource_xml_list_iterate);
  
  /* User list */
  i_resource *res;
  for (i_list_move_head(res_list); (res=i_list_restore(res_list))!=NULL; i_list_move_next(res_list))
  {
    xmlNodePtr res_node = xmlNewNode (NULL, BAD_CAST "resource");

    if (res->ident_str) asprintf (&str, "%i:%i:%s", res->type, res->ident_int, res->ident_str);
    else asprintf (&str, "%i:%i", res->type, res->ident_int);
    xmlNewChild (res_node, NULL, BAD_CAST "id", BAD_CAST str);
    free (str);

    if (res->construct)
    {
      asprintf (&str, "%i", res->construct->pid);
      xmlNewChild (res_node, NULL, BAD_CAST "pid", BAD_CAST str);
      free (str);
    }
      
    asprintf (&str, "%i", res->restart_count);
    xmlNewChild (res_node, NULL, BAD_CAST "restart_count", BAD_CAST str);
    free (str);
    
    asprintf (&str, "%i", res->heartbeatfail_count);
    xmlNewChild (res_node, NULL, BAD_CAST "heartbeatfail_count", BAD_CAST str);
    free (str);
    
    xmlNewChild (res_node, NULL, BAD_CAST "type", BAD_CAST i_resource_typestr(res->type));
    
    xmlAddChild (root_node, res_node);
  }
    
  /* Finished */
  req->xml_out = xml;
  
  /* Socket Trickery */
  i_resource_address *client_handler_addr;
  char *res_addr_str;
  asprintf (&res_addr_str, "::%i:0:", RES_CLIENT_HANDLER_CORE);
  client_handler_addr = i_resource_address_string_to_struct (res_addr_str);
  free (res_addr_str);
  i_resource *dst_res = i_resource_local_get (global_res_table, client_handler_addr); /* Find the client handler */
  i_resource_free_address (client_handler_addr);

  /* Swapsies */
  i_socket *swapped_socket = self->core_socket;
  self->core_socket = dst_res->core_socket;
  i_xml_deliver (self, req);
  self->core_socket = swapped_socket;
  
  return 0;
}


