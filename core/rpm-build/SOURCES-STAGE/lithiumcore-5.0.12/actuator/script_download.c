#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <errno.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/entity.h>
#include <induction/xml.h>

#include "shutdown.h"
#include "xml_render.h"
#include "script_download.h"
#include "errorcodes.h"

/* script_download functions */

int a_script_download (i_resource *self, int argc, char *argv[], int optind)
{
  char *entaddr_str;
  char *resaddr_str;
  char *type_str;
  char *name_str;
  i_entity_address *entaddr;
  i_resource_address *resaddr;
  /* Args/State check */
  
  if (argc < optind+4) 
  { printf ("Not enough arguments sent to a_download_script\n"); exit (ERROR_BADARGS); }

  if (!self->core_socket) 
  { printf ("Failed to download script. No connection to Lithium Client Handler present\n"); exit (ERROR_NOCONN); }

  resaddr_str = argv[optind + 2];
  entaddr_str = argv[optind + 3];
  type_str = argv[optind + 4];
  name_str = argv[optind + 5];

  /* Convert address */
  resaddr = i_resource_address_string_to_struct (resaddr_str);
  entaddr = i_entity_address_struct (entaddr_str);

  /* Read/Convert xmlout */
  i_xml *xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "script_download");
  xmlDocSetRootElement (xml->doc, root_node);
  xmlNewChild (root_node, NULL, BAD_CAST "type", BAD_CAST type_str);
  xmlNewChild (root_node, NULL, BAD_CAST "name", BAD_CAST name_str);

  /* Send XML */
  i_xml_request *xmlreq = i_xml_get (self, resaddr, entaddr, "script_download", 0, xml, a_script_download_cbfunc, NULL);
  i_resource_address_free (resaddr);
  i_entity_address_free (entaddr);
  if (xml) i_xml_free (xml);
  if (!xmlreq)
  { printf ("Failed to send request for xml\n"); exit (ERROR_SENDFAILED); }

  return 1;
}

int a_script_download_cbfunc (i_resource *self, i_xml *xml, int result, void *passdata)
{
  char *path_str = NULL;
  if (xml && result == XML_RESULT_OK)
  { 
    xmlNodePtr root_node = xmlDocGetRootElement (xml->doc);
    xmlNodePtr node;
    for (node = root_node->children; node; node = node->next)
    { 
      char *str;

      str = (char *) xmlNodeListGetString (xml->doc, node->xmlChildrenNode, 1);
      if (!strcmp((char *)node->name, "path") && str) path_str = strdup (str);

      xmlFree (str);
    }
  }

  fprintf (stdout, "%s", path_str);
  
  /* Check */
  if (!path_str)
  { i_printf (1, "xml_script_download path not specified in return xml"); exit (ERROR_INTERNAL); }

  /* Read and display */
  int fd;
  size_t datasize;
  char *data;
  fd = open (path_str, O_RDONLY);
  if (fd > 0)
  {
    /* Read from temp path */
    datasize = lseek (fd, 0, SEEK_END);
    data = malloc (datasize);
    memset (data, 0, datasize);
    lseek (fd, 0, SEEK_SET);
    int numread = read (fd, data, datasize);
    if (numread < 1) i_printf(1, "xml_script_download failed to read from file");
    close (fd);
    
    /* Write to stdout */
//    write (STDOUT_FILENO, data, datasize);
    free (data);
  }
  else
  {
    /* Error */
    fprintf (stdout, "Failed to retrieve xml (%s)", strerror(errno)); 
    a_shutdown (self);
    if (result == XML_RESULT_DENIED)
    { exit (ERROR_DENIED); }
    else
    { exit (ERROR_OPERROR); }
  }

  /* Terminate process */
  a_shutdown (self);
  exit (0);

  /* Finished */
  return 0;
}
