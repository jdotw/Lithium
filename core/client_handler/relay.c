#include <stdlib.h>

#include <induction.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/hashtable.h>
#include <induction/respond.h>
#include <induction/auth.h>

#include "main.h"

int l_relay_to_client_core (i_resource *self, i_message *msg, i_socket *client_socket)
{
  int num;

  /* Srt the correct addressing */
  
  if (msg->src) i_resource_free_address (msg->src);                                                               /* Strip any address added by the client */
  msg->src = i_resource_address_duplicate (RES_ADDR(self));                                                       /* Change the src address to the client resource addr */
  if (!msg->dst) msg->dst = i_resource_address_string_to_struct ("::::");                                         /* If no destination is supplied, send it to the core */

  /* Set the correct auth */

  if (!self->auth)
  {
    i_printf (1, "l_relay_to_client_core called with NULL self->auth. failed for security reasons");
    module_shutdown (self);
    exit (1);
    return -1;
  }
  if (msg->auth) i_authentication_free (msg->auth);
  msg->auth = i_authentication_duplicate (self->auth);
  if (!msg->auth)
  {
    i_printf (1, "l_relay_to_client_core failed to duplicate self->auth credentials to msg->auth");
    module_shutdown (self);
    exit (1);
    return -1;
  }

  i_printf (2, "l_relay_msg_to_client_core routing message from %s:%s:%i:%i:%s to %s:%s:%i:%i:%s",
    msg->src->plexus, msg->src->node, msg->src->type, msg->src->ident_int, msg->src->ident_str,
    msg->dst->plexus, msg->dst->node, msg->dst->type, msg->dst->ident_int, msg->dst->ident_str);

  /* Relay the message */

  num = i_message_write_sockfd (self, self->core_socket, msg);   /* Relay the message to the client_core */
  if (num != 0)
  {
    i_message *msg_out;
    
    i_printf (1, "i_client_process_client unable to forward msg to client_handler_core");
    msg_out = i_message_create ();
    msg_out->type = MSG_FAIL;
    msg_out->flags = MSG_FLAG_RESP;
    msg_out->msgid = random ();
    msg_out->reqid = msg->msgid;
    i_message_address_set (msg_out, RES_ADDR(self), msg->src);
    i_message_write_sockfd (self, client_socket, msg_out);                                           /* Relay failed, send back a fail */
    i_message_free (msg_out);
    return 0;
  }

  return 0;
}

