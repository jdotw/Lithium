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
#include <induction/construct.h>
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

#include "script.h"

/* Delete a script */

int xml_script_delete (i_resource *self, i_xml_request *req)
{
  int num;
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;

  if (!req->xml_in) return -1;

  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

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
  { i_printf (1, "xml_script_upload file and/or type not specified"); return -1; }

  /* Check type */
  int modb = 0;
  if (strcmp(type_str, "modb") == 0) 
  { modb = 1; }

  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "script_upload");
  xmlDocSetRootElement (req->xml_out->doc, root_node);

  /* Check name */
  if (strstr(name_str, ".."))
  {
    xmlNewChild (root_node, NULL, BAD_CAST "error", BAD_CAST "Illegal filename specified");
    return 1;
  }

  /* Create script path */
  char *script_path;
  if (modb)
  {
    char *base_path = i_path_glue (self->construct->config_path, "module_builder");
    script_path = i_path_glue (base_path, name_str);
    free (base_path);
  }
  else
  {
    char *type_path;
    asprintf (&type_path, "%s_scripts", type_str);
    char *dir_path = i_path_glue (self->root, type_path);
    free (type_path);
    script_path = i_path_glue (dir_path, name_str);
    free (dir_path);
  }

  /* Unlink */
  num = unlink (script_path);
  free (script_path);
  if (num == 0)
  {
    xmlNewChild (root_node, NULL, BAD_CAST "message", BAD_CAST "Script deleted");
  }
  else
  {
    xmlNewChild (root_node, NULL, BAD_CAST "error", BAD_CAST "Failed to delete script"); 
  }

  return 1; 
}

