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

/* Request Struct manipulation */

i_xml_request* i_xml_request_create ()
{
  i_xml_request *req;

  req = (i_xml_request *) malloc (sizeof(i_xml_request));
  if (!req)
  { i_printf (1, "i_xml_request_create failed to malloc i_xml_request struct"); return NULL; }
  memset (req, 0, sizeof(i_xml_request));

  return req;
}

void i_xml_request_free (void *reqptr)
{
  i_xml_request *req = reqptr;

  if (!req) return;

  if (req->xml_name) free (req->xml_name);
  if (req->resaddr) i_resource_address_free (req->resaddr);
  if (req->entaddr) i_entity_address_free (req->entaddr);
  if (req->msg_in) i_message_free (req->msg_in);
  if (req->auth) i_authentication_free (req->auth);
  if (req->xml_in) i_xml_free (req->xml_in);
  if (req->xml_out) i_xml_free (req->xml_out);

  free (req);
}


/** \addtogroup xml XML Functions
 * @{
 */

