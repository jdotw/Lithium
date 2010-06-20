#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/entity.h>
#include <induction/xml.h>

#include "shutdown.h"
#include "xml_render.h"
#include "script_upload.h"
#include "errorcodes.h"

/* script_upload functions */

int a_script_upload (i_resource *self, int argc, char *argv[], int optind)
{
  char *entaddr_str;
  char *resaddr_str;
  char *type_str;
  char *path_str;
  char *name_str;
  i_entity_address *entaddr;
  i_resource_address *resaddr;
  /* Args/State check */
  
  if (argc < optind+5) 
  { printf ("Not enough arguments sent to a_upload_script\n"); exit (ERROR_BADARGS); }

  if (!self->core_socket) 
  { printf ("Failed to upload script. No connection to Lithium Client Handler present\n"); exit (ERROR_NOCONN); }

  resaddr_str = argv[optind + 2];
  entaddr_str = argv[optind + 3];
  type_str = argv[optind + 4];
  name_str = argv[optind + 5];
  path_str = argv[optind + 6];

  /* Convert address */
  resaddr = i_resource_address_string_to_struct (resaddr_str);
  entaddr = i_entity_address_struct (entaddr_str);

  /* Read/Convert xmlout */
  i_xml *xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "script_upload");
  xmlDocSetRootElement (xml->doc, root_node);
  xmlNewChild (root_node, NULL, BAD_CAST "type", BAD_CAST type_str);
  xmlNewChild (root_node, NULL, BAD_CAST "path", BAD_CAST path_str);
  xmlNewChild (root_node, NULL, BAD_CAST "name", BAD_CAST name_str);

  /* Send XML */
  i_xml_request *xmlreq = i_xml_get (self, resaddr, entaddr, "script_upload", 0, xml, a_script_upload_cbfunc, NULL);
  i_resource_address_free (resaddr);
  i_entity_address_free (entaddr);
  if (xml) i_xml_free (xml);
  if (!xmlreq)
  { printf ("Failed to send request for xml\n"); exit (ERROR_SENDFAILED); }

  return 1;
}

int a_script_upload_cbfunc (i_resource *self, i_xml *xml, int result, void *passdata)
{
  if (xml && result == XML_RESULT_OK)
  { 
    /* Render the XML */
    a_xml_render (self, xml);
  }
  else
  {
    /* Error */
    i_printf (1, "Failed to retrieve xml"); 
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
