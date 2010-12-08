#include <stdio.h>
#include <stdlib.h>

#include <induction.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/hashtable.h>
#include <induction/respond.h>

#include "resource_message.h"
#include "resource_terminate.h"
#include "resource_destroy.h"
#include "route.h"

extern i_hashtable *global_res_table;

int l_resource_msgproc_default_func (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* Called when an un-handled request or response is received from a
   * i_msgproc-enabled resource which is attached to this core 
   */

  i_resource *res = passdata;

  i_printf (2, "l_resource_msgproc_default_func processing incoming message from %i:%i:%s (type is %i)", res->type, res->ident_int, res->ident_str, msg->type);

  if ((i_message_verify_self_destination(self, msg)) != 0)
  {
    /* Message is not addressed to us, route it */
    l_route_message (self, global_res_table, res, msg);
  }
  else
  {
    /* Message is addressed to us, fail it */
    if (msg->flags & MSG_FLAG_REQ)
    { i_respond_failed (res, msg, 0); }
  }

  return 0;
}

int l_resource_msgproc_fatal_func (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* Called when a fatal error occurs in i_msgproc for a resource
   * attached to this core
   */

  i_resource *res = passdata;

  i_printf (1, "l_resource_msgproc_fatal_func fatal error occurred in i_msgproc for resource %i:%i:%s - Restarting resource", res->type, res->ident_int, res->ident_str);
  i_resource_local_restart (self, global_res_table, res);
  return 0; 
}
