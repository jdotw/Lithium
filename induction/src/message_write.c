#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "message.h"
#include "auth.h"
#include "socket.h"
#include "data.h"

/* Write */

int i_message_write_sockfd (i_resource *self, i_socket *sock, i_message *msg)
{
  /* Data format : 
   *
   * int *auth_datasize;
   * char *auth_data;
   *
   * int dst_addr_str_size;
   * char *dst_addr_str
   * int src_addr_str_size
   * char *src_addr_strr
   *
   * int type;
   * int flags;
   * long msgid;
   * long reqid;
   * 
   * int msg_datasize
   * char *msg_data
   *
   */

  int datasize;
  int auth_datasize;
  char *auth_data;
  char *dst_addr_str;
  char *src_addr_str;
  char *data;
  char *dataptr;
  i_socket_data *size_op;
  i_socket_data *data_op;

  /* Authentication, Destination and Source data */
  
  if (msg->auth)
  { 
    auth_data = i_authentication_struct_to_data (msg->auth, &auth_datasize); 
    if (!auth_data || auth_datasize < 1)
    { i_printf (1, "i_message_write_sockfd failed to convert msg->auth to auth_data"); return -1; }
  }
  else
  { auth_data = NULL; auth_datasize = 0; }

  if (msg->dst) 
  { dst_addr_str = i_resource_address_struct_to_string (msg->dst); }
  else
  { dst_addr_str = NULL; }

  if (msg->src) 
  { src_addr_str = i_resource_address_struct_to_string (msg->src); }
  else
  { src_addr_str = NULL; }

  /* Calculate datasize and malloc data */
  
  datasize = (6*sizeof(int)) + (2*sizeof(long)) + auth_datasize + msg->datasize;
  if (dst_addr_str) datasize += strlen (dst_addr_str) + 1;
  if (src_addr_str) datasize += strlen (src_addr_str) + 1;

  data = (char *) malloc (datasize);
  if (!data) 
  { 
    i_printf (1, "i_message_write_sockfd failed to malloc data (%i bytes)", datasize);
    if (auth_data) free (auth_data);
    if (dst_addr_str) free (dst_addr_str);
    if (src_addr_str) free (src_addr_str);
    return -1; 
  }
  memset (data, 0, datasize);
  dataptr = data;

  /* Add info to data block */

  dataptr = i_data_add_chunk (data, dataptr, datasize, auth_data, auth_datasize);
  free (auth_data);
  if (!dataptr)
  {
    i_printf (1, "i_message_write_sockfd failed to add auth_data to data"); 
    if (dst_addr_str) free (dst_addr_str);
    if (src_addr_str) free (src_addr_str);
    free (data); 
    return -1;
  }

  dataptr = i_data_add_string (data, dataptr, datasize, dst_addr_str);
  free (dst_addr_str);
  if (!dataptr)
  {
    i_printf (1, "i_message_write_sockfd failed to add auth_data to data"); 
    if (src_addr_str) free (src_addr_str);
    free (data); 
    return -1;
  }

  dataptr = i_data_add_string (data, dataptr, datasize, src_addr_str);
  free (src_addr_str);
  if (!dataptr)
  { i_printf (1, "i_message_write_sockfd failed to add auth_data to data"); free (data); return -1; }

  dataptr = i_data_add_int (data, dataptr, datasize, &msg->type);
  if (!dataptr)
  { i_printf (1, "i_message_write_sockfd failed to add msg->type to data"); free (data); return -1; }

  dataptr = i_data_add_int (data, dataptr, datasize, &msg->flags);
  if (!dataptr)
  { i_printf (1, "i_message_write_sockfd failed to add msg->flags to data"); free (data); return -1; }
  
  dataptr = i_data_add_long (data, dataptr, datasize, &msg->msgid);
  if (!dataptr)
  { i_printf (1, "i_message_write_sockfd failed to add msg->msgid to data"); free (data); return -1; }

  dataptr = i_data_add_long (data, dataptr, datasize, &msg->reqid);
  if (!dataptr)
  { i_printf (1, "i_message_write_sockfd failed to add msg->msgid to data"); free (data); return -1; }

  dataptr = i_data_add_chunk (data, dataptr, datasize, msg->data, msg->datasize);
  if (!dataptr)
  { i_printf (1, "i_message_write_sockfd failed to add msg->data to data"); free (data); return -1; }

  /* Write the datasize to the the socket */

  size_op = i_socket_write (self, sock, &datasize, sizeof(int), NULL, NULL);
  if (!size_op)
  { i_printf (1, "i_message_write_sockfd failed to write datasize int to socket"); free (data); return -1; }

  /* Write the data to the socket */
  
  data_op = i_socket_write (self, sock, data, datasize, NULL, NULL);
  free (data);
  if (!data_op)
  { i_printf (1, "i_message_write_sockfd failed to write data to socket"); return -1; }

  return 0;
}

