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

#include "lunregistry.h"

/* List all registered services */

static i_list* static_target_list = NULL;
static i_list* static_use_list = NULL;

xmlNodePtr l_lunregsitry_xml_lun (l_lunregistry_entry *lun);

int xml_lunregistry_list (i_resource *self, i_xml_request *req)
{
  i_xml *xml;
  xmlNodePtr root_node = NULL;

  /* Create XML */
  xml = i_xml_create ();
  if (!xml) 
  { i_printf (1, "xml_incident_list failed to create xml struct"); return -1; }

  /* Create/setup doc */
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "lun_list");
  xmlDocSetRootElement(xml->doc, root_node);

  /* Iterate hashtable */
  static_target_list = i_list_create ();
  static_use_list = i_list_create ();
  i_hashtable *obj_table = l_lunregistry_table_object ();
  i_hashtable_iterate (self, obj_table, l_lunregistry_xml_list_iterate, NULL);

  /* LUN list */
  l_lunregistry_entry *lun;
  for (i_list_move_head(static_target_list); (lun=i_list_restore(static_target_list))!=NULL; i_list_move_next(static_target_list))
  {
    xmlNodePtr lun_node = l_lunregsitry_xml_lun (lun);
    xmlAddChild (root_node, lun_node);
  }
  for (i_list_move_head(static_use_list); (lun=i_list_restore(static_use_list))!=NULL; i_list_move_next(static_use_list))
  {
    xmlNodePtr lun_node = l_lunregsitry_xml_lun (lun);
    xmlAddChild (root_node, lun_node);
  }
  i_list_free (static_target_list);
  i_list_free (static_use_list);
  static_target_list = NULL;
  static_use_list = NULL;
    
  /* Finished */
  req->xml_out = xml;
  
  return 1;
}

xmlNodePtr l_lunregsitry_xml_lun (l_lunregistry_entry *lun)
{
    xmlNodePtr lun_node = xmlNewNode (NULL, BAD_CAST "lun");

    xmlNewChild (lun_node, NULL, BAD_CAST "wwn", BAD_CAST lun->wwn_str);

    char *str;
    asprintf (&str, "%i", lun->lun);
    xmlNewChild (lun_node, NULL, BAD_CAST "lun", BAD_CAST str);
    free (str);

    asprintf (&str, "%i", lun->type);
    xmlNewChild (lun_node, NULL, BAD_CAST "lun_type", BAD_CAST str);
    free (str);

    xmlNodePtr ent_node = i_entity_descriptor_xml (lun->objent);
    xmlAddChild (lun_node, ent_node);

  return lun_node;
}

void l_lunregistry_xml_list_iterate (i_resource *self, i_hashtable *obj_table, void *data, void *passdata)
{
  l_lunregistry_entry *entry = data;

  if (entry->type == 1)
  { i_list_enqueue (static_target_list, entry); }
  else if (entry->type == 2)
  { i_list_enqueue (static_use_list, entry); }
}
