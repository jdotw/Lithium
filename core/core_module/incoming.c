#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/form.h>
#include <induction/xml.h>

#include "incoming.h"
#include "resource_message.h"
#include "resource_spawn.h"
#include "resource_destroy.h"
#include "resource_terminate.h"
#include "heartbeat.h"

int l_incoming_resource (i_resource *self, i_socket *socket, void *passdata)
{
  /* Called when there is an incmming connection from a child process 
   *
   * ALWAYS RETURN 0, persistent callback
   * 
   */
  
  i_hashtable *res_table = passdata;
  i_resource_incoming_cbdata *incoming_op;
  
  incoming_op = i_resource_local_incoming (self, res_table, socket->sockfd, l_incoming_resource_incoming_callback, NULL);
  if (!incoming_op)
  { i_printf (1, "l_incoming_resource failed to attach incoming resource"); return 0; }

  return 0;
}

int l_incoming_resource_incoming_callback (i_resource *self, i_resource *res, void *passdata)
{
  int num;
  
  if (res)
  {
    num = i_msgproc_enable (self, res->core_socket);
    if (num != 0)
    { 
      i_printf (1, "l_incoming_resource failed to enable i_msgproc on res->core_socket for %i:%i:%s",
        res->type, res->ident_int, res->ident_str); 
      return -1;
    }

    /* Handlers */
    i_msgproc_handler_add (self, res->core_socket, MSG_FORM_GET, i_form_get_handler, NULL);
    i_msgproc_handler_add (self, res->core_socket, MSG_FORM_SEND, i_form_send_handler, NULL);
    i_msgproc_handler_add (self, res->core_socket, MSG_XML_GET, i_xml_get_handler, NULL);
    i_msgproc_handler_add (self, res->core_socket, MSG_RES_SPAWN, l_resource_spawn_handler, res);
    i_msgproc_handler_add (self, res->core_socket, MSG_RES_DESTROY, l_resource_destroy_handler, res);
    i_msgproc_handler_add (self, res->core_socket, MSG_RES_TERMINATE, l_resource_terminate_handler, res);
    i_msgproc_handler_add (self, res->core_socket, MSG_HEARTBEAT, l_heartbeat_handler, res);

    /* Defaut/Fatal Handlers */
    i_msgproc_callback_set_default (self, res->core_socket, l_resource_msgproc_default_func, res);
    i_msgproc_handler_set_default (self, res->core_socket, l_resource_msgproc_default_func, res);
    i_msgproc_handler_set_fatal (self, res->core_socket, l_resource_msgproc_fatal_func, res);

    /* Enable heartbeat checking */
    num = l_heartbeat_enable (self, res);
    if (num != 0)
    { i_printf (1, "l_incoming_resource_incoming_callback failed to enable heartbeat checking"); }
  }
  else
  { i_printf (1, "l_incoming_resource_incoming_callback failed to attach an incoming resource"); }

  return 0;
}
