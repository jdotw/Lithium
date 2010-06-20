#include <stdio.h>
#include <stdlib.h>
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

/* Generic failure handling function for xml-related handlers */

int i_xml_handler_failed (i_resource *self, i_message *msg, i_xml_request *reqdata)
{
  if (msg) i_respond_failed (self, msg, 0);
  if (reqdata) i_xml_request_free (reqdata);
  return 0;
}

/* @} */
