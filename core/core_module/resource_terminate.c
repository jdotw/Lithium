#include <stdlib.h>

#include <induction.h>
#include <induction/message.h>
#include <induction/respond.h>
#include <induction/hashtable.h>
#include <induction/socket.h>

extern i_hashtable *global_res_table;

int l_resource_terminate_handler (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* Always return 0 to keep the handler active */
  int num;
  i_resource *res = passdata;
  i_resource_address *addr;

  if (!self || !msg || !msg->data) return 0;

  addr = i_resource_address_string_to_struct (msg->data);
  if (!addr)
  {
    i_printf (1, "l_resource_terminate_handler failed to convert msg->data to resource_address");
    i_respond_failed (res, msg, 0);
    return 0; 
  }

  num = i_resource_local_terminate (global_res_table, addr);
  if (num != 0) 
  {
    i_printf (2, "l_resource_terminate_handler failed to terminate resource %i:%i:%s", addr->type, addr->ident_int, addr->ident_str);
    i_resource_free_address (addr);
    i_respond_failed (res, msg, 0);
    return 0;
  }
  i_resource_free_address (addr);

  i_respond_ok (res, msg);
  
  return 0;
}

