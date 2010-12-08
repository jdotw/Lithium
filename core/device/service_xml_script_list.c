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
#include <induction/xml.h>

#include "../customer/script.h"
#include "service.h"

/* List of all scripts present on the system */

typedef struct
{
  i_xml_request *xmlreq;
  int script_count;
  int waiting;
  i_list *script_list;
} l_service_script_list_req;

int xml_service_script_list (i_resource *self, i_xml_request *req)
{
  /* Walks the services directory, queries each script
   * for its description and then returns the xml
   */

  l_service_script_list_req *scriptreq;

  /* Create out requestd ata */
  scriptreq = (l_service_script_list_req *) malloc (sizeof(l_service_script_list_req));
  memset (scriptreq, 0, sizeof(l_service_script_list_req));
  scriptreq->xmlreq = req;

  /* Create list */
  scriptreq->script_list = i_list_create ();
  i_list_set_destructor (scriptreq->script_list, l_service_script_free);

  /* Open dir */
  char *dirname = i_path_glue (self->root, "../../service_scripts");
  DIR *dir = opendir (dirname);
  if (!dir)
  { i_printf (1, "xml_service_script_list failed to open directory %s", dirname); free(dirname); return -1; }

  /* Traverse */
  struct dirent *dirent;
  while ((dirent=readdir(dir))!=NULL)
  {
    if (dirent->d_name[0] == '.') continue;

    l_service_proc *proc = l_service_exec (self, dirent->d_name, "info", NULL, l_service_xml_script_list_scriptcb, scriptreq);
    if (proc)
    {
      scriptreq->waiting++;
      scriptreq->script_count++;
    }
  }

  if (scriptreq->waiting == 0)
  {
    /* No script to wait for... create and send XML */
    i_xml *xml = i_xml_create ();
    xml->doc = xmlNewDoc (BAD_CAST "1.0");
    xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "script_list");
    xmlDocSetRootElement (xml->doc, root_node);
    req->xml_out = xml;
    free (scriptreq);

    return 1;
  }    
  
  return 0;
}

int l_service_xml_script_list_scriptcb (i_resource *self, l_service_proc *proc, void *passdata)
{
  /* Called when a script has reported back */
  l_service_script_list_req *req = passdata;

  /* Decrement counter */
  req->waiting--;

  /* Create script */
  l_service_script *script = l_service_script_create ();
  script->name_str = strdup (proc->script_file);

  /* Check for xml */
  if (proc->output_str && strstr(proc->output_str, "<?xml"))
  {
    /* Break out the parser */
    i_xml *xml = i_xml_struct (proc->output_str, strlen(proc->output_str), "service_script");
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;
    root_node = xmlDocGetRootElement (xml->doc);
    if (root_node)
    {
      for (node = root_node->children; node; node = node->next)
      {
        char *str;

        if (!strcmp((char *)node->name, "config_variable"))
        {
          /* Config variable */
          l_service_configvar *var = l_service_configvar_fromxml (xml, node);
          if (var) { i_list_enqueue (script->configvar_list, var); }
          continue;
        }

        str = (char *) xmlNodeListGetString (xml->doc, node->xmlChildrenNode, 1);

        if (!strcmp((char *)node->name, "desc") && str) script->desc_str = strdup(str);
        if (!strcmp((char *)node->name, "info") && str) script->info_str = strdup(str);
        if (!strcmp((char *)node->name, "version") && str) script->version_str = strdup(str);
        if (!strcmp((char *)node->name, "proto") && str) script->protocol_str = strdup(str);
        if (!strcmp((char *)node->name, "transport") && str) script->transport_str = strdup(str);
        if (!strcmp((char *)node->name, "port") && str) script->port_str = strdup(str);

        xmlFree (str);
      }
      script->status = SCRIPT_OK;
    }
  }
  else if (proc->output_str && strlen(proc->output_str) > 0)
  {
    /* Plain Text */
    { script->desc_str = strdup (proc->output_str); }
    script->status = SCRIPT_OK;
  }
  else
  {
    /* No output or empty output... script has a problem */
    script->status = SCRIPT_ERROR;
  }

  if (!script->desc_str)
  { script->desc_str = strdup (proc->script_file); }
  
  i_printf (0, "l_service_xml_script_list_scriptcb called script is %s/%s", script->name_str, script->desc_str);
  
  /* Enqueue */
  i_list_enqueue (req->script_list, script);

  /* Check to see if we're still waiting */
  if (req->waiting < 1)
  {
    i_printf (0, "l_service_xml_script_list_scriptcb delivering XML!!!"); 
    /* Create and send XML */
    i_xml *xml = i_xml_create ();
    xml->doc = xmlNewDoc (BAD_CAST "1.0");
    xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "script_list");
    xmlDocSetRootElement (xml->doc, root_node);

    /* Add scripts */
    l_service_script *script;
    for (i_list_move_head(req->script_list); (script=i_list_restore(req->script_list))!=NULL; i_list_move_next(req->script_list))
    {      
      xmlNodePtr script_node = l_service_script_xml (script);
      if (script_node) xmlAddChild (root_node, script_node);
    }

    /* Deliver */
    req->xmlreq->xml_out = xml;
    i_xml_deliver (self, req->xmlreq);

    /* Cleanup */
    i_list_free (req->script_list);
    free (req);
  }

  return 0;
}

