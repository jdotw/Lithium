#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
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
#include <induction/construct.h>
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

/* Upload a script */

int xml_script_upload (i_resource *self, i_xml_request *req)
{
  int num;
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;

  if (!req->xml_in) return -1;

  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_USER)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  char *type_str = NULL;
  char *path_str = NULL;
  char *name_str = NULL;
  root_node = xmlDocGetRootElement (req->xml_in->doc);
  for (node = root_node->children; node; node = node->next)
  {
    char *str;

    str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
    if (!strcmp((char *)node->name, "type") && str) type_str = strdup (str);
    if (!strcmp((char *)node->name, "path") && str) path_str = strdup (str);
    if (!strcmp((char *)node->name, "name") && str) name_str = strdup (str);

    xmlFree (str);
  }

  /* Check */
  if (!type_str || !path_str || !name_str)
  { i_printf (1, "xml_script_upload file and/or type not specified"); return -1; }

  /* Check type */
  int modb = 0;
  if (strcmp(type_str, "modb") == 0) 
  { modb = 1; }

  /* Copy */
  int fd;
  size_t datasize;
  char *data;

  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "script_upload");
  xmlDocSetRootElement (req->xml_out->doc, root_node);

  fd = open (path_str, O_RDONLY);
  if (fd > 0)
  {
    /* Read from temp path */
    datasize = lseek (fd, 0, SEEK_END);
    data = malloc (datasize);
    memset (data, 0, datasize);
    lseek (fd, 0, SEEK_SET);
    num = read (fd, data, datasize);
    close (fd);

    /* Create script path */
    char *script_path;
    if (modb)
    {
      /* Module builder upload */
      char *base_path = i_path_glue (self->construct->config_path, "module_builder");
      script_path = i_path_glue (base_path, name_str); 
      free (base_path);
    }
    else
    {
      /* Action/Service Script Upload */
      char *type_path;
      asprintf (&type_path, "%s_scripts", type_str);
      char *dir_path = i_path_glue (self->root, type_path);
      free (type_path);
      script_path = i_path_glue (dir_path, name_str); 
      free (dir_path);
    }

    /* Write to script file */
    fd = open (script_path, O_WRONLY|O_CREAT|O_TRUNC, 0775); 
    if (fd == -1)
    {
      /* Failed to open file */
      if (errno == EEXIST)
      {
        xmlNewChild (root_node, NULL, BAD_CAST "error", BAD_CAST "File already exists");
        return 1;
      }
      else
      {
        xmlNewChild (root_node, NULL, BAD_CAST "error", BAD_CAST "Failed to open file for writing.");
        return 1;
      }
    }
    else
    {
      /* File is open, write to it */
      size_t bytes_written = write (fd, data, datasize);
      close (fd);
      free (data);
      if (bytes_written != datasize)
      { xmlNewChild (root_node, NULL, BAD_CAST "error", BAD_CAST "Failed to write entire file."); return 1; }
      else
      { xmlNewChild (root_node, NULL, BAD_CAST "message", BAD_CAST "Script uploaded successfully."); }
    }
    
    /* Repair the script to fix any deps */
    if (!modb)
    {
//      char *repair_path = i_path_glue (self->root, "../scripts/script_repair.pl");
//      char *command_str;
//      asprintf (&command_str, "%s", repair_path);
//      l_script_proc *proc = l_script_exec (self, command_str, script_path, l_script_upload_repaircb, req);
//      if (!proc)
//      { xmlNewChild (root_node, NULL, BAD_CAST "error", BAD_CAST "Failed to run repair script."); return 1; }
    }
  }
  else
  { xmlNewChild (root_node, NULL, BAD_CAST "error", BAD_CAST "Failed to open uploaded script for reading."); return 1; }

//  if (modb)
//  {
//    /* Finished upload */
//    return 1;
//  }
//  else
//  {
//    /* Wait for repair screipt */
//    return 0; 
//  }
//
  return 1;

}

int l_script_upload_repaircb (i_resource *self, l_script_proc *proc, void *passdata)
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
