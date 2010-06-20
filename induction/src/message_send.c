#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "message.h"
#include "auth.h"

/* Send */

long i_message_send (i_resource *self, int type, void *data, int datasize, i_resource_address *dst, int flags, long reqid)
{
  /* The i_message_send function is a function used to 
   * send a to other lithium processes. It assumes use of the 
   * self->core_socket as the transmit medium.
   *
   * Returns the message ID on success, -1 on error
   */

  int num;
  long msgid;
  i_message *msg;

  /* Create the msg */
  
  msg = i_message_create ();
  if (!msg)
  { i_printf (1, "i_message_send failed to create msg struct"); return -1; }
  msg->type = type;
  msg->datasize = datasize;
  msg->msgid = random ();
  msgid = msg->msgid;

  /* Authentication info */

  if (self && self->auth)
  {
    msg->auth = i_authentication_duplicate (self->auth);
    if (!msg->auth)
    { i_printf (1, "i_message_send failed to duplicate self->auth to msg->auth"); i_message_free (msg); return -1; }
  }
  else
  { i_printf (0, "i_message_send called with NULL self->auth"); }

  /* Message data */

  if (data && msg->datasize > 0)
  {
    msg->data = (char *) malloc (datasize);
    if (!msg->data)
    { i_printf (1, "i_message_send unable to malloc msg->data"); i_message_free (msg); return -1; }
    memcpy (msg->data, data, datasize);
  }

  /* Set the flags up */
  
  msg->flags = flags;
  msg->reqid = reqid;

  /* Address and send the msg */

  i_message_address_set (msg, RES_ADDR(self), dst);

  num = i_message_write_sockfd (self, self->core_socket, msg);
  i_message_free (msg);
  if (num != 0)
  { i_printf (1, "i_message_send failed to write message to self->core_socket"); return -1; }

  /* Finished */
  
  return msgid;
}

