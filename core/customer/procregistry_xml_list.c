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

#include "procregistry.h"

/* List all registered procpros */

int xml_procregistry_list (i_resource *self, i_xml_request *req)
{
  i_xml *xml;
  xmlNodePtr root_node = NULL;

  /* Create XML */
  xml = i_xml_create ();
  if (!xml) 
  { i_printf (1, "xml_incident_list failed to create xml struct"); return -1; }

  /* Create/setup doc */
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "procpro_list");
  xmlDocSetRootElement(xml->doc, root_node);

  /* Iterate hashtable */
  i_list *proc_list = i_list_create ();
  i_hashtable *obj_table = l_procregistry_table_object ();
  i_hashtable_iterate (self, obj_table, l_procregistry_xml_list_iterate, proc_list);

  /* Vendor module list */
  l_procregistry_entry *proc;
  for (i_list_move_head(proc_list); (proc=i_list_restore(proc_list))!=NULL; i_list_move_next(proc_list))
  {
    xmlNodePtr proc_node = i_entity_descriptor_xml (proc->objent);
    xmlAddChild (root_node, proc_node);
  }
  i_list_free (proc_list);
    
  /* Finished */
  req->xml_out = xml;
  
  return 1;
}

void l_procregistry_xml_list_iterate (i_resource *self, i_hashtable *obj_table, void *data, void *passdata)
{
  i_list *proc_list = passdata;
  l_procregistry_entry *entry = data;

  i_list_enqueue (proc_list, entry);
}
