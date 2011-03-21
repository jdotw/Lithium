#include <stdio.h>
#include <stdlib.h>

#include <induction.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/hashtable.h>
#include <induction/respond.h>

#include "msgproc_core.h"
#include "main.h"

extern i_hashtable *global_client_table;

/* Default Callback */

int l_msgproc_core_default_from_core (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /*
   * The default function called for both unhandled requests and responses
   * that have come from the lithium core resource to the client_handler_core
   */
  
  int num;
  i_hashtable *client_table = passdata;

  /* Get the message */

  if (!msg)
  {
    i_printf (1, "l_msgproc_core_default_from_core unable to recv message from core. Terminating self");
    module_shutdown (self);       /* Assume we have lost the connection to the core and terminate */
    exit (1);
    return -1;
  }

  if (msg->dst)
  {
    i_printf (2, "l_msgproc_core_default_from_core processing message from core to %s:%s:%i:%i:%s",
      msg->dst->plexus, msg->dst->node, msg->dst->type, msg->dst->ident_int, msg->dst->ident_str);
  }
  else
  { i_printf (2, "l_msgproc_core_default_from_core processing message from core to self"); }

  /* See if the message is addressed to us, or to be relayed */

  if ((i_message_verify_self_destination(self, msg)) == 0)
  {
    /* The message is addressed to us */
    i_respond_failed (self, msg, 0);  /* Feature not supported right now */
  }
  else
  {
    /* The message is not addressed to us, try and route it */

    i_resource *dst_res;

    dst_res = i_resource_local_get (client_table, msg->dst);               /* See if the dst resource is in our client_table */
    if (!dst_res)
    {
      /* The dst_res isnt in our table, send a ROUTE_FAILED */

      i_printf (2, "l_msgproc_core_default_from_core unable to route msg from %s:%s:%i:%i:%s to %s:%s:%i:%i:%s (dst_res not found)",
        msg->src->plexus, msg->src->node, msg->src->type, msg->src->ident_int, msg->src->ident_str, 
        msg->dst->plexus, msg->dst->node, msg->dst->type, msg->dst->ident_int, msg->dst->ident_str);
      i_respond_route_failed (self, msg);
      return 0;
    }

    num = i_message_write_sockfd (self, dst_res->core_socket, msg);
    if (num != 0) 
    { 
      i_printf (1, "l_msgproc_core_default_from_core unable to forward msg to dst_res"); 
      i_respond_route_failed (self, msg);
      return 0;
    }    

  }

  return 0;
}

int l_msgproc_core_default_from_child (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* 
   * The default function called for both unhandled requests and responses
   * that have come from client_handler_child resources
   */

  int num;
  i_resource *res = passdata;

  if (msg->dst)
  {
    i_printf (2, "l_msgproc_core_default_from_child processing message from res %s:%s:%i:%i:%s to %s:%s:%i:%i:%s",
      res->plexus, res->node, res->type, res->ident_int, res->ident_str,
      msg->dst->plexus, msg->dst->node, msg->dst->type, msg->dst->ident_int, msg->dst->ident_str);
  }
  else
  {
    i_printf (2, "l_msgproc_core_default_from_child processing message from res %s:%s:%i:%i:%s to core",
      res->plexus, res->node, res->type, res->ident_int, res->ident_str);
  }

  /* Take the msg and relay it to the core */

  if (!msg)
  {
    i_printf (2, "l_msgproc_core_default_from_child detached resource %i:%i:%s", res->type, res->ident_int, res->ident_str);
    i_resource_local_destroy (self, global_client_table, RES_ADDR(res));
    return -1;
  }

  if (msg->type == MSG_TERMINATE)
  {
    /* The resource has informed us it is terminating */

    i_printf (2, "l_msgproc_core_default_from_child performing normal termination of %i:%i:%s", res->type, res->ident_int, res->ident_str);
    i_resource_local_destroy (self, global_client_table, RES_ADDR(res));
    return -1;
  }

  num = i_message_write_sockfd (self, self->core_socket, msg);
  if (num != 0)
  {
    i_printf (1, "l_msgproc_core_default_from_child unable to forward msg to core");
    i_respond_route_failed (res, msg);
    return 0;
  }

  return 0;

}

int l_msgproc_core_fatal_from_child (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  i_resource *res = passdata;

  i_printf (2, "l_msgproc_core_fatal_from_child called, destroying child resource");
  i_resource_local_destroy (self, global_client_table, RES_ADDR(res));

  return 0;
}

