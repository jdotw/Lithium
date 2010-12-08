#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/path.h>
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

#include "script.h"

/* Repair a script */

int xml_script_repair (i_resource *self, i_xml_request *req)
{
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;

  if (!req->xml_in) return -1;

  /* Interpret XML */
  char *type_str = NULL;
  char *name_str = NULL;
  root_node = xmlDocGetRootElement (req->xml_in->doc);
  for (node = root_node->children; node; node = node->next)
  {
    char *str;

    str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
    if (!strcmp((char *)node->name, "type") && str) type_str = strdup (str);
    if (!strcmp((char *)node->name, "name") && str) name_str = strdup (str);

    xmlFree (str);
  }

  /* Check */
  if (!type_str || !name_str)
  { i_printf (1, "xml_script_repair file and/or type not specified"); return -1; }

  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "script_repair");
  xmlDocSetRootElement (req->xml_out->doc, root_node);

  /* Create script path */
  char *type_path;
  asprintf (&type_path, "%s_scripts", type_str);
  char *dir_path = i_path_glue (self->root, type_path);
  free (type_path);
  free (dir_path);

  /* Repair the script to fix any deps */
  char *repair_path = i_path_glue (self->root, "../scripts/script_repair.pl");
  char *command_str;
  asprintf (&command_str, "%s", repair_path);
//  l_script_proc *proc = l_script_exec (self, command_str, script_path, l_script_repair_repaircb, req);
//  if (!proc)
//  { xmlNewChild (root_node, NULL, BAD_CAST "error", BAD_CAST "Failed to run repair script."); return 1; }
  
  xmlNewChild (root_node, NULL, BAD_CAST "message", BAD_CAST "Script repair operation completed.");

  /* Wait for repair script */
  //  return 0; 
  //
  return 1;
}

int l_script_repair_repaircb (i_resource *self, l_script_proc *proc, void *passdata)
{
  i_xml_request *req = passdata;

  /* Add Output */
  xmlNodePtr root_node = xmlDocGetRootElement (req->xml_out->doc);
  if (proc->output_str)
  {
    xmlNewChild (root_node, NULL, BAD_CAST "script_output", BAD_CAST proc->output_str);
  }
  
  /* Deliver XML */
  i_xml_deliver (self, req);

  return 0;
}
