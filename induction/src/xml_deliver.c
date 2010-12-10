#include <stdlib.h>
#include <libxml/parser.h>

#include "induction.h"
#include "cement.h"
#include "entity.h"
#include "construct.h"
#include "message.h"
#include "xml.h"

/** \addtogroup xml XML Functions
 * @{
 */

/* 
 * XML delivery (deliverying a response to a get/send request 
 */

int i_xml_deliver (i_resource *self, i_xml_request *reqdata)
{
  /* This function is used by xml_func functions which are called from
   * either the i_xml_get_handler or i_xml_send_handler to actually 
   * deliver the response to a xml_get or xml_send request.
   */

  long msgid;
  int xml_datasize;
  char *xml_data;

  if (!reqdata) return -1;

  if (!reqdata->xml_out)
  { i_xml_handler_failed (self, reqdata->msg_in, reqdata); return 0; }

  /* Set xml state */
  reqdata->xml_out->state = XMLSTATE_DELIVERABLE;

  /* Check for any not-ready items */
  if (reqdata->xml_out->notready_count > 0)
  {
    /* Some items are not ready. The xmls
     * state has been set to deliverable, 
     * hence when all items have been set to
     * ready by i_xml_item_ready, that func 
     * will call i_xml_deliver again.
     */
    return 0;
  }
  
  /* Performance logging */
  char *perf_resaddr_str = reqdata->resaddr ? i_resource_address_struct_to_string (reqdata->resaddr) : NULL;
  char *perf_entaddr_str = reqdata->entaddr ? i_entity_address_string (NULL, reqdata->entaddr) : NULL;
  char *perf_srcaddr_str = reqdata->msg_in->src ? i_resource_address_struct_to_string (reqdata->msg_in->src) : NULL;
  struct timeval perf_start;
  gettimeofday (&perf_start, NULL);
  if (self->perflog) i_printf (0, "PERF: i_xml_deliver called to deliver %s from %s for %s to %s", reqdata->xml_name, perf_resaddr_str, perf_entaddr_str, perf_srcaddr_str);

  /* Compile xml to data */
  xml_data = i_xml_data (reqdata->xml_out, &xml_datasize);

  /* Performance logging */
  struct timeval perf_now;
  gettimeofday (&perf_now, NULL);
  if (self->perflog) i_printf (0, "PERF: i_xml_deliver (%s:%s) data compilation took %li seconds to produce %i bytes", reqdata->xml_name, perf_entaddr_str, perf_now.tv_sec - perf_start.tv_sec, xml_datasize);

  /* Send message */
  msgid = i_message_send (self, MSG_FORM_GET, xml_data, xml_datasize, reqdata->msg_in->src, MSG_FLAG_RESP, reqdata->msg_in->msgid);
  free (xml_data);
  if (msgid == -1)
  { i_printf (1, "i_xml_deliver failed to send response message"); i_xml_handler_failed (self, reqdata->msg_in, reqdata); return -1; }

  /* Performance logging */
  gettimeofday (&perf_now, NULL);
  if (perf_resaddr_str) free (perf_resaddr_str);
  if (perf_entaddr_str) free (perf_entaddr_str);
  if (perf_srcaddr_str) free (perf_srcaddr_str);

  /* Free reqdata */
  i_xml_request_free (reqdata);

  return 0;
}

/* @} */
