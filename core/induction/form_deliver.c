#include <stdlib.h>

#include "induction.h"
#include "form.h"
#include "message.h"

/** \addtogroup form Web Forms
 * @{
 */

/* Form delivery (deliverying a response to a get/send request */

int i_form_deliver (i_resource *self, i_form_reqdata *reqdata)
{
  /* This function is used by form_funct functions which are  called from
   * either the i_form_get_handler or i_form_send_handler to actually 
   * deliver the response to a form_get or form_send request.
   */

  long msgid;
  int form_datasize;
  char *form_data;

  if (!reqdata) return -1;
  
  /* Set form state */
  reqdata->form_out->state = FORMSTATE_DELIVERABLE;

  /* Check for any not-ready items */
  if (reqdata->form_out->notready_count > 0)
  {
    /* Some items are not ready. The forms
     * state has been set to deliverable, 
     * hence when all items have been set to
     * ready by i_form_item_ready, that func 
     * will call i_form_deliver again.
     */
    return 0;
  }
  
  if (!reqdata->form_out)
  { i_form_handler_failed (self, reqdata->msg_in, reqdata); return 0; }

  /* Compile form to data */

  form_data = i_form_form_to_data (reqdata->form_out, &form_datasize);       /* Compile the form into a data block */
  if (!form_data || form_datasize < 1)
  { i_printf (1, "i_form_deliver failed to convert reqdata->form to form_data"); i_form_handler_failed (self, reqdata->msg_in, reqdata); return -1; }

  /* Send message */

  msgid = i_message_send (self, MSG_FORM_GET, form_data, form_datasize, reqdata->msg_in->src, MSG_FLAG_RESP, reqdata->msg_in->msgid);
  free (form_data);
  if (msgid == -1)
  { i_printf (1, "i_form_deliver failed to send response message"); i_form_handler_failed (self, reqdata->msg_in, reqdata); return -1; }

  /* Free reqdata */

  i_form_reqdata_free (reqdata);

  return 0;
}

int i_form_deliver_denied (i_resource *self, i_form_reqdata *reqdata)
{
  reqdata->form_out = i_form_create_denied ();
  i_form_deliver (self, reqdata);
  return 0;
}

/* @} */
