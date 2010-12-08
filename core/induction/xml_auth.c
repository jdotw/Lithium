#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <libxml/parser.h>

#include "induction.h"
#include "entity.h"
#include "entity_xml.h"
#include "message.h"
#include "socket.h"
#include "data.h"
#include "msgproc.h"
#include "auth.h"
#include "list.h"
#include "respond.h"
#include "incident.h"
#include "device.h"
#include "resource_xml.h"
#include "incident_xml.h"
#include "xml.h"

/** \addtogroup xml XML Functions
 * @{
 */

/*
 * Device-related XML functions
 */

/* XML Functions */

i_xml* i_xml_denied ()
{
  /* Create return xml */
  i_xml *xml_out;
  xml_out = i_xml_create ();
  xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "error");
  xmlDocSetRootElement (xml_out->doc, root_node);
  xmlNewChild (root_node, NULL, BAD_CAST "message", BAD_CAST "Permission Denied.");
 
  return xml_out;
}


/* @} */
