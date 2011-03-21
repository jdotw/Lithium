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
#include <induction/construct.h>
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

/* Download a script */

int xml_script_download (i_resource *self, i_xml_request *req)
{
  int num;
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
  { i_printf (1, "xml_script_download file and/or type not specified"); return -1; }

  /* Check type */
  int modb = 0;
  if (strcmp(type_str, "modb") == 0) 
  { modb = 1; }

  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "script_upload");
  xmlDocSetRootElement (req->xml_out->doc, root_node);

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

  /* Read script to memory */
  int fd = open (script_path, O_RDONLY);
  if (fd > 0)
  {
    /* Read */
    size_t datasize = lseek (fd, 0, SEEK_END);
    char *data = malloc (datasize);
    memset (data, 0, datasize);
    lseek (fd, 0, SEEK_SET);
    num = read (fd, data, datasize);
    close (fd);

    /* Set temp filename */
    char *temp_config_file = "";
    struct timeval now;
    gettimeofday (&now, NULL);
    char *prefix;
    asprintf (&prefix, "script-%li-%i-", now.tv_sec, (int)now.tv_usec);
    temp_config_file = tempnam(NULL, prefix);

    /* Write to temp file */
    fd = open (temp_config_file, O_WRONLY|O_CREAT, 0664);
    if (fd > 0)
    {
      int numwritten = write (fd, data, datasize);
      if (numwritten < 1) i_printf(1, "xml_script_download failed to write");
      close (fd);
    
      xmlNewChild (root_node, NULL, BAD_CAST "path", BAD_CAST temp_config_file);
    }
    free (data);
  }

  return 1; 
}

