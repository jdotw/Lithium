#include <stdio.h>
#include <errno.h>

#include "induction.h"

int i_send_code (i_resource *self, i_resource_address *src, i_resource_address *dst, int code)
{
  if (!self || !self->core_socket) return -1;
  
  return i_send_code_to_sockfd (self->core_socket->sockfd, src, dst, code);  
}

int i_send_code_to_sockfd (int sockfd, i_resource_address *src, i_resource_address *dst, int code)
{
  int num;
  i_message *msg;
  
  msg = i_message_create (code, NULL, 0);
  if (!msg) { i_printf (1, "i_send_code_to_socket unable to create msg"); return -1; }

  i_message_address (msg, src, dst);

  num = i_message_send_to_sockfd (sockfd, msg);

  i_message_free (msg);
                           
  return num;
}

int i_send_ok (i_resource *self, i_resource_address *src, i_resource_address *dst)
{
  return i_send_code (self, src, dst, MSG_OK);
}

int i_send_fail (i_resource *self, i_resource_address *src, i_resource_address *dst, int error)
{
  int num;
  i_message *msg;

  msg = i_message_create (MSG_FAIL, &error, sizeof(int));
  if (!msg) return -1;

  i_message_address (msg, src, dst);
	
  num = i_message_send (self, msg);

  i_message_free (msg);
	
  return num;
}

int i_send_unknown (i_resource *self, i_resource_address *src, i_resource_address *dst)
{
  return i_send_code (self, src, dst, MSG_UNKNOWN);
}

int i_send_no_data (i_resource *self, i_resource_address *src, i_resource_address *dst)
{
  return i_send_code (self, src, dst, MSG_NO_DATA);
}

