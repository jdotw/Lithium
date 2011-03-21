#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include <induction.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/hashtable.h>
#include <induction/respond.h>
#include <induction/auth.h>
#include <induction/customer.h>

#include "msgproc_child.h"
#include "main.h"
#include "relay.h"
#include "reqtime.h"

static i_socket *static_client_socket = NULL;
extern i_hashtable *global_client_table;

int l_msgproc_child_default_from_client (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* The default function called for both unhandled requests and unhandled
   * responses that have come from a client to the client_handler_child
   */
  
  int num;
  i_socket *client_socket = passdata;

  /* Update reqtime */
//  l_reqtime_reqrecvd ();
  i_printf (2, "l_msgproc_child_default_from_client message recvd type %i", msg->type);

  /* Print message info */
  if (msg->dst)
  {
    i_printf (2, "l_msgproc_child_default_from_client processing message type %i from client %s:%s:%i:%i:%s to %s:%s:%i:%i:%s",
      msg->type, self->plexus, self->node, self->type, self->ident_int, self->ident_str,
      msg->dst->plexus, msg->dst->node, msg->dst->type, msg->dst->ident_int, msg->dst->ident_str);
  }
  else
  {
    i_printf (2, "l_msgproc_child_default_from_client processing message type %i from client %s:%s:%i:%i:%s to core",
      msg->type, self->plexus, self->node, self->type, self->ident_int, self->ident_str);
  }

  /* Take the msg and relay it to the client_core 
   *
   * At this stage the message should be considered 'untrusted'
   * as it comes from a 'client'.
   *
   * FIX : This is where some msg validity and pre-checking should
   * occur before the message is deemed 'trusted' and passed into 
   * the client_handler_core_process 
   */

  if (!msg)
  {
    i_printf (2, "l_msgproc_child_default_from_client unable to recv msg, assuming client dead and terminating");
    module_shutdown (self);         /* Terminate the process */
    exit (1);
    return 0;
  }

  if (msg->type == MSG_TERMINATE) 
  {
    i_printf (2, "l_msgproc_child_default_from_client client has requested normal termination");
    i_message_send (self, MSG_TERMINATE, NULL, 0, NULL, MSG_FLAG_REQ, 0);
    module_shutdown (self);
    exit (1);
    return 0;
  }

  /* Authenticate / Authorise the message */

  if (!self->auth)
  {
    i_printf (1, "l_msgproc_child_default_from_client called with no authentication credentials in self");
    module_shutdown (self);
    exit (1);
    return 0;
  }

  if (self->auth->level < 1)
  {
    /* Not yet authenticated, perform authenticity verification */
    i_message *dup_msg;
    i_resource_address *authres;
 
    /* Because msg will be freed at the end of this
     * function, a dup_msg is created and past to the post_auth
     * callback functions
     */

    /* Customer resource will be the authenticator */
    authres = i_customer_get_resaddr_by_id (self, msg->auth->customer_id);
    if (!authres)
    {
      i_printf (1, "l_msgproc_child_default_from_client failed to get customer resource address for authentication");
      return 0;
    }
      
    dup_msg = i_message_duplicate (msg);    /* Create a dup message for the callback to use */
    if (!dup_msg)
    {
      i_printf (1, "l_msgproc_child_default_from_client failed to create dup_msg");
      i_resource_free_address (authres);
      return 0;
    } 
    static_client_socket = client_socket;

    num = i_authentication_verify (self, authres, msg->auth, l_msgproc_child_default_from_client_auth_cb, dup_msg);
    i_resource_free_address (authres);
    if (num != 0)
    {
      i_printf (1, "l_msgproc_child_default_from_client unable to request authentication (num=%i)", num);
      i_message_free (dup_msg);
      return 0;
    }

    /* dup_msg will be freed by l_msgproc_client_message_auth_cb
     * either already or when it is called by the callback 
     */
    
    /* Update reqtime */
//    l_reqtime_authsent ();

  }
  else
  {
    /* Authentication has already been performed */
    l_relay_to_client_core (self, msg, client_socket);

    /* Update reqtime */
//    l_reqtime_reqrelayed ();
  }

  return 0;
}

int l_msgproc_child_default_from_client_auth_cb (i_resource *self, i_authentication *auth, int result, void *passdata)
{
  /* This callback is called when an authentication response is recvd.
   *
   * First, examine the response to see if it was denied or not.
   * Then, providing it was not denied and is not an error, relay the msg
   */

  i_message *original_msg = passdata;
  
  /* Update reqtime */
//  l_reqtime_authrecvd ();

  if (!original_msg) 
  {
    i_printf (1, "l_msgproc_child_default_from_client_auth_cb called with NULL original message");
    return -1;
  }

  if (result == AUTH_RESULT_OK)
  {
    /* Authentication successful */
  
    self->auth = i_authentication_duplicate (auth);
  
    l_relay_to_client_core (self, original_msg, static_client_socket);
    i_message_free (original_msg);

    /* Update reqtime */
//    l_reqtime_reqrelayed ();
    
    return 0;
  }

  /* Authentication did not proceed 
   *
   * i_respond_denied is used to denote an authentication
   * related problem
   */

  /* Switch sockets */
  self->core_socket = static_client_socket;

  /* Re-enable blocking */
  fcntl (static_client_socket->sockfd, F_SETFL, fcntl (static_client_socket->sockfd, F_GETFL, 0) & ~O_NONBLOCK);

  /* Respond with error */
  switch (result)
  {
    case AUTH_RESULT_TIMEOUT: i_printf (1, "l_msgproc_child_default_from_client_auth_cb authentication timeout occurred for %s", auth->username);
                             i_respond_nodata (self, original_msg);
                              break;
    case AUTH_RESULT_ERROR: i_printf (1, "l_msgproc_child_default_from_client_auth_cb an error occurred during authentication for %s", auth->username);
                             i_respond_nodata (self, original_msg);
                            break;
    case AUTH_RESULT_DENIED: i_printf (1, "l_msgproc_child_default_from_client_auth_cb authentication denied for username %s", auth->username);
                             i_respond_denied (self, original_msg);
                             break;
  }

  /* Free original */
  i_message_free (original_msg);

  /* Shutdown */
//  module_shutdown (self);
//  exit (1);

  return 0;
}

/* Default from client_handler_core */

int l_msgproc_child_default_from_core (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* Called by a client_handler_child resource to process all unhandled requests and 
   * unhandled responses from the client_handler_core resource
   */

  int num;
  i_socket *client_socket = passdata;

  /* Update reqtime */
//  l_reqtime_resprecvd ();
  
  i_printf (2, "l_msgproc_child_default_from_core processing message from client_handler_core to %s:%s:%i:%i:%s",
    self->plexus, self->node, self->type, self->ident_int, self->ident_str);

  /* Relay to the client */

  num = i_message_write_sockfd (self, client_socket, msg);      /* Relay to the client */
  if (num != 0)
  {
    i_printf (1, "l_msgproc_child_default_from_core unable to forward msg to client");
    i_respond_route_failed (self, msg);
    return 0;
  }

  return 0;
}

/* Fatal error from client_handler_core */

int l_msgproc_child_fatal_from_core (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  i_printf (2, "l_msgproc_child_fatal_from_core called, terminating resource");
  module_shutdown (self);
  exit (0);
  return 0;
}

int l_msgproc_child_fatal_from_client (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  i_printf (2, "l_msgproc_child_fatal_from_client called, terminating resource");
  module_shutdown (self);
  exit (0);
  return 0;
}
