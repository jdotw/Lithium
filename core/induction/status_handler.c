#include <stdlib.h>

#include <induction.h>

int i_status_get_int_handler (i_resource *self, i_socket *sock, i_message *msg, void *data)
{
  /* Handles incoming MSG_GET_STATUS_INT messages 
   *
   * Always return 0
   */

  int msgid;
  
  msgid = i_message_v2_send (self, RES_GET_STATUS_INT_V2, &static_highest_status_level, sizeof(int), msg->src, MSG_FLAG_RESP, msg->msgid);
  if (msgid == -1)
  { i_printf (1, "i_status_get_int_handler failed to send response"); }

  return 0;
}
