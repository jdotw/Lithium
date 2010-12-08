#include <stdlib.h>

#include <induction.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/respond.h>
#include <induction/hashtable.h>

#include "resource_destroy.h"

extern i_hashtable *global_res_table;

int l_resource_destroy_handler (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* Always return 0 to keep handler alive */
  int num;
  i_resource_address *addr;
  i_resource *res = passdata;

  if (!self || !msg || !msg->data) return 0;

  addr = i_resource_address_string_to_struct (msg->data);
  if (!addr)
  {
    i_printf (1, "l_resource_destroy_handler failed to convert msg->data to resource_address struct");
    i_respond_failed (res, msg, 0);
    return 0;
  }

  num = i_resource_local_destroy (self, global_res_table, addr);
  if (num != 0) 
  {
    i_printf (2, "l_resource_destroy_handler failed to destroy resource %i:%i:%s", addr->type, addr->ident_int, addr->ident_str);
    i_resource_free_address (addr);
    i_respond_failed (res, msg, 0);
    return 0;
  }
  i_resource_free_address (addr);

  i_respond_ok (res, msg);
  
  return 0;
}

