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
#include "xml_get.h"
#include "errorcodes.h"

/* xml_get functions */

int a_xml_get (i_resource *self, int argc, char *argv[], int optind)
{
  int num;
  char *entaddr_str;
  char *resaddr_str;
  char *xmlname_str;
  char *xmlout_str;
  time_t ref_sec;
  i_entity_address *entaddr;
  i_resource_address *resaddr;
  i_xml *xml_out = NULL;
  i_xml_request *xmlreq;

  /* Args/State check */
  
  if (argc < optind+5) 
  { i_printf (1, "ERROR: Not enough arguments sent to a_xml_get\n"); exit (ERROR_BADARGS); }

  if (!self->core_socket) 
  { 
    i_printf (1, "ERROR: No connection to Lithium Client Handler present\n"); 
    exit (ERROR_NOCONN);
  }

  resaddr_str = argv[optind + 2];
  entaddr_str = argv[optind + 3];
  xmlname_str = argv[optind + 4];
  ref_sec = (time_t) atol (argv[optind + 5]);
  xmlout_str = argv[optind + 6];

  /* Convert address */
  resaddr = i_resource_address_string_to_struct (resaddr_str);
  entaddr = i_entity_address_struct (entaddr_str);

  /* Read/Convert xmlout */
  if (xmlout_str && strlen(xmlout_str) > 0)
  {
    int fd;
    int xmlout_datasize;
    char *xmlout_data;

    fd = open (xmlout_str, O_RDONLY);
    if (fd > 0)
    {
      xmlout_datasize = lseek (fd, 0, SEEK_END);
      xmlout_data = malloc (xmlout_datasize);
      memset (xmlout_data, 0, xmlout_datasize);
      lseek (fd, 0, SEEK_SET);
      num = read (fd, xmlout_data, xmlout_datasize);
      close (fd);

      xml_out = i_xml_struct (xmlout_data, xmlout_datasize, NULL);
      free (xmlout_data);
    }
  }

  /* Get XML */
  xmlreq = i_xml_get (self, resaddr, entaddr, xmlname_str, ref_sec, xml_out, a_xml_get_cbfunc, NULL);
  i_resource_address_free (resaddr);
  i_entity_address_free (entaddr);
  if (xml_out) i_xml_free (xml_out);
  if (!xmlreq)
  {
    i_printf (1, "ERROR: Failed to send XML request to Core");  
    exit (ERROR_SENDFAILED);
  }

  return 1;
}

int a_xml_get_cbfunc (i_resource *self, i_xml *xml, int result, void *passdata)
{
  if (xml && result == XML_RESULT_OK)
  { 
    /* Render the XML */
    a_xml_render (self, xml);
  }
  else
  {
    /* Error */
    a_shutdown (self);
    if (result == XML_RESULT_DENIED)
    { 
      i_printf (2, "ERROR: Authentication/Permission denied"); 
      exit (ERROR_DENIED); 
    }
    else
    { 
      i_printf (2, "ERROR: Non-Auth Error occurred retrieving XML"); 
      exit (ERROR_OPERROR); 
    }
  }

  /* Terminate process */
  a_shutdown (self);
  exit (0);

  /* Finished */
  return 0;
}
