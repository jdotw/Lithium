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
#include "respond.h"
#include "xml.h"

/** \addtogroup xml XML Functions
 * @{
 */

#define XML_GET_TIMEOUT_SEC 60

/* 
 * Induction XML engine 
 */

/* Struct manipulation */

i_xml* i_xml_create ()
{
  i_xml *xml;

  xml = (i_xml *) malloc (sizeof(i_xml));
  if (!xml)
  { i_printf (1, "i_xml_create failed to malloc i_xml struct"); return NULL; }
  memset (xml, 0, sizeof(i_xml));

  return xml;
}

void i_xml_free (void *xmlptr)
{
  i_xml *xml = xmlptr;

  if (!xml) return;

  if (xml->doc) xmlFreeDoc(xml->doc);
  if (xml->raw_data) free (xml->raw_data);

  free (xml);
}

/* @} */
