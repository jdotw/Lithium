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
#include <induction/auth.h>
#include <induction/xml.h>

#include "procpro.h"

/* Delete an Action*/

int xml_procpro_delete (i_resource *self, i_xml_request *req)
{
  int num;
  char *id_str = NULL;

  if (!self->hierarchy->dev->swrun) return -1;

  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  if (req->xml_in)
  {
    /* Action ID specified */
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;

    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;
      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
      if (!strcmp((char *)node->name, "id") && str) id_str = strdup (str);
      xmlFree (str);
    }
  }

  if (!id_str)
  { i_printf (1, "xml_procpro_update no procpro id specified"); return -1; }

  /* Delete */
  num = l_procpro_sql_delete (self, atol(id_str));
  if (num != 0)
  { i_printf (1, "xml_procpro_update failed to delete procpro %s", id_str); return -1; }

  /* Remove from list */
  i_list *procpro_list = l_procpro_list ();
  l_procpro *list_proc;
  for (i_list_move_head(procpro_list); (list_proc=i_list_restore(procpro_list))!=NULL; i_list_move_next(procpro_list))
  {
    if (list_proc->id == (unsigned long) atol(id_str))
    { i_list_delete (procpro_list); }
  }
  
  /* Create object */
  l_procpro_object_remove (self, atol(id_str));
  
  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "procpro");
  xmlDocSetRootElement (req->xml_out->doc, root_node);
  xmlNewChild (root_node, NULL, BAD_CAST "id", BAD_CAST id_str);

  return 1;
}


