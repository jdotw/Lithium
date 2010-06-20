#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <libxml/parser.h>

#include "induction.h"
#include "entity.h"
#include "message.h"
#include "socket.h"
#include "data.h"
#include "msgproc.h"
#include "auth.h"
#include "xml.h"

/** \addtogroup xml XML Functions
 * @{
 */

#define XML_GET_TIMEOUT_SEC 60

/* Struct/Data conversions */

char* i_xml_data (i_xml *xml, int *datasizeptr)
{
  if (!xml->raw_data)
  {
    /* Init */
    xmlChar *data = NULL;
    int datasize = 0;
  
    /* Convert provided data buffer to XML struct */
    xmlDocDumpMemory(xml->doc, &data, &datasize);

    /* Set datasizeptr */
    *datasizeptr = datasize;
    
    return (char *) data;
  }
  else
  {
    /* The xml already exists as raw data */
    *datasizeptr = strlen (xml->raw_data)+1;
    return strdup (xml->raw_data); 
  }
}

i_xml* i_xml_struct (char *data, int datasize, char *xml_name)
{
  /* Convert provided XML struct to data buffer */
  i_xml *xml;
  char *base_str;

  /* Create struct */
  xml = i_xml_create ();
  xml->raw_data = (char *) malloc (datasize+1);
  memcpy (xml->raw_data, data, datasize);
  xml->raw_data[datasize] = '\0';

  /* Check if name is specified */
  if (xml_name)
  { asprintf (&base_str, "%s.xml", xml_name); }
  else
  { base_str = strdup ("generic.xml"); }

  xml->doc = xmlReadMemory (data, datasize, base_str, "UTF-8", 0);
  free (base_str);

  return xml;
}

/* @} */
