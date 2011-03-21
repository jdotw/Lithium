#include <stdio.h>
#include <stdlib.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/message.h>
#include <induction/respond.h>

void l_route_message (i_resource *self, i_hashtable *res_table, i_resource *res, i_message *msg)
{
  int num;
  i_resource *dst_res;

  if (!msg->dst || !msg->src) return;

  i_printf (2, "l_resource_msg_route called to route msg %li from %s:%s:%i:%i:%s to %s:%s:%i:%i:%s",
    msg->msgid, msg->src->plexus, msg->src->node, msg->src->type, msg->src->ident_int, msg->src->ident_str,
    msg->dst->plexus, msg->dst->node, msg->dst->type, msg->dst->ident_int, msg->dst->ident_str);
    
  dst_res = i_resource_local_get (res_table, msg->dst);
  if (!dst_res)
  {
    /* Destination is not in our res table  */
    if (msg->dst->type == RES_CLIENT_HANDLER_CHILD)
    { 
      /* Message is destined for a client */
      i_resource_address *client_handler_addr;
      char *res_addr_str;

      asprintf (&res_addr_str, "::%i:0:", RES_CLIENT_HANDLER_CORE);
      client_handler_addr = i_resource_address_string_to_struct (res_addr_str);
      free (res_addr_str);
      if (!client_handler_addr)
      {
        i_printf (1, "l_resource_msg_route failed to create addr for client_handler_core");
        i_respond_route_failed (res, msg);
        return;
      }
      
      dst_res = i_resource_local_get (res_table, client_handler_addr); /* Find the client handler */
      i_resource_free_address (client_handler_addr);
      if (!dst_res)
      {
        i_printf (1, "l_resource_msg_route unable to find client_handler_core resource");
        i_respond_route_failed (res, msg);
        return;
      }

    }
    else
    {
      /* Message has reached a dead end */

      char *src_str;
      char *dst_str;

      i_respond_route_failed (res, msg);

      src_str = i_resource_address_struct_to_string (RES_ADDR(msg->src));
      dst_str = i_resource_address_struct_to_string (RES_ADDR(msg->dst));
      i_printf (1, "l_resource_msg_route failed to route msg from %s to %s (dst_res not found)", src_str, dst_str);
      if (src_str) free (src_str);
      if (dst_str) free (dst_str);
  
      return;
    }

  }

  if (!dst_res->core_socket)
  {
    i_respond_route_failed (res, msg);
    i_printf (1, "l_resource_msg_route failed because destination (%s:%s:%i:%i:%s) is not connected",
      dst_res->plexus, dst_res->node, dst_res->type, dst_res->ident_int, dst_res->ident_str);

    return;
  }
  
  num = i_message_write_sockfd (self, dst_res->core_socket, msg);   /* Forward the MSG */
  if (num != 0) 
  { 
    i_respond_route_failed (res, msg);
    i_printf (1, "l_resource_msg_route failed to route msg from %s:%s:%i:%i:%s to %s:%s:%i:%i:%s (send to dst_res failed)",
      msg->src->plexus, msg->src->node, msg->src->type, msg->src->ident_int, msg->src->ident_str,
      dst_res->plexus, dst_res->node, dst_res->type, dst_res->ident_int, dst_res->ident_str);
    return; 
  }

  i_printf (2, "l_resource_msg_route msg %li from %s:%s:%i:%i:%s to %s:%s:%i:%i:%s routed successfully",
    msg->msgid, msg->src->plexus, msg->src->node, msg->src->type, msg->src->ident_int, msg->src->ident_str,
    msg->dst->plexus, msg->dst->node, msg->dst->type, msg->dst->ident_int, msg->dst->ident_str);
    

}
