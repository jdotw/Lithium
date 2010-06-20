#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "message.h"
#include "data.h"
#include "socket.h"
#include "auth.h"

/* Callback data struct manipulation */

i_message_read_cbdata* i_message_read_cbdata_create ()
{
  i_message_read_cbdata *cbdata;

  cbdata = (i_message_read_cbdata *) malloc (sizeof(i_message_read_cbdata));
  if (!cbdata)
  { i_printf (1, "i_message_read_cbdata_create failed to malloc cbdata"); return NULL; }
  memset (cbdata, 0, sizeof(i_message_read_cbdata));

  return cbdata;
}

void i_message_read_cbdata_free (void *cbdataptr)
{
  i_message_read_cbdata *cbdata = cbdataptr;

  if (!cbdata) return;

  free (cbdata);
}

/* Message read */

int i_message_read (i_resource *self, i_socket *sock, int (*callback_func) (i_resource *self, i_message *msg, void *passdata), void *passdata)
{
  /* Called to receive an incoming message from the given socket
   *
   * The message recv operation is started, the callback will be run 
   * when the message has been received.
   *
   */

  if (sock->msg_read_cbdata)
  { i_printf (1, "i_message_read failed, sock->msg_read_cbdata already present"); return -1; }

  /* Setup the callback data */

  sock->msg_read_cbdata = i_message_read_cbdata_create ();
  if (!sock->msg_read_cbdata)
  { i_printf (1, "i_message_read failed to create cbdata struct"); return -1; }
  sock->msg_read_cbdata->callback_func = callback_func;
  sock->msg_read_cbdata->passdata = passdata;
    
  /* Read the datasize int from the socket */
  
  sock->msg_read_cbdata->size_op = i_socket_read (self, sock, sizeof(int), i_message_read_socketread_callback, sock->msg_read_cbdata);
  if (!sock->msg_read_cbdata->size_op)
  { 
    i_printf (1, "i_message_read failed to call i_socket_read to read datasize int"); 
    i_message_read_cbdata_free (sock->msg_read_cbdata);
    sock->msg_read_cbdata = NULL;
    return -1; 
  }

  return 0;
}

int i_message_read_socketread_callback (i_resource *self, i_socket *sock, i_socket_data *op, int result, void *passdata)
{
  /* Called when a pending read operation had completed */

  i_message_read_cbdata *cbdata = passdata;

  /* Remove the msg_read_cbdata pointer in the socket struct */
  
  sock->msg_read_cbdata = NULL;

  /* Process the callback result */

  if (result != SOCKET_RESULT_SUCCESS || !op || !op->data) 
  {
    /* Fatal Error */
    if (cbdata->size_op)
    { i_printf (2, "i_message_read_socketread_callback experienced a fatal error during 'size' read operation"); cbdata->size_op = NULL; }
    if (cbdata->data_op)
    { i_printf (2, "i_message_read_socketread_callback experienced a fatal error during 'data' read operation"); cbdata->data_op = NULL; }
    
    if (cbdata->callback_func)
    { cbdata->callback_func (self, NULL, cbdata->passdata); }
    i_message_read_cbdata_free (cbdata);

    return -1;
  }

  if (cbdata->size_op)
  {
    /* This is the return from the read for the datasize int */

    cbdata->size_op = NULL;      /* Remove the pointer to this data op */
    
    if (op->datasize > sizeof(int))
    { 
      i_printf (1, "i_message_read_socketread_callback recvd op->datasize > sizeof(int) (expecting datasize int)");
      if (cbdata->callback_func)
      { cbdata->callback_func (self, NULL, cbdata->passdata); }
      i_message_read_cbdata_free (cbdata);
      return -1;
    }

    memcpy (&cbdata->msg_datasize, op->data, sizeof(int));
    if (cbdata->msg_datasize < 1)
    { 
      i_printf (1, "i_message_read_socketread_callback recvd message datasize as < 1");
      if (cbdata->callback_func)
      { cbdata->callback_func (self, NULL, cbdata->passdata); }
      i_message_read_cbdata_free (cbdata);
      return -1;
    }

    cbdata->data_op = i_socket_read (self, sock, cbdata->msg_datasize, i_message_read_socketread_callback, cbdata);
    if (!cbdata->data_op)
    { 
      i_printf (1, "i_message_read_socketread_callback failed to call i_socket_read for message data");
      if (cbdata->callback_func)
      { cbdata->callback_func (self, NULL, cbdata->passdata); }
      i_message_read_cbdata_free (cbdata);
      return -1;
    }

    /* Re-add the msg_read_cbdata pointer to the socket,
     * as it will be used when this function is called
     * again foer the data_op read
     */

    sock->msg_read_cbdata = cbdata;
  }
  else
  {
    /* This must be the return from the read for the actual data */

    int auth_datasize;
    int datasize;
    int offset;
    char *data;
    char *dataptr;
    char *auth_data;
    char *dst_addr_str;
    char *src_addr_str;
    i_message *msg;

    /* Validate State/Data */

    if (!cbdata->data_op)
    {
      i_printf (1, "i_message_read_socketread_callback called with NULL sock->msg_read_cbdata->size_op and sock->msg_read_cbdata->data_op");
      if (cbdata->callback_func)
      { cbdata->callback_func (self, NULL, cbdata->passdata); }
      i_message_read_cbdata_free (cbdata);
      return -1;
    }

    cbdata->data_op = NULL;   /* Remove the pointer to this data op */

    /* Interpret the message */

    data = op->data;
    dataptr = data;
    datasize = op->datasize;

    msg = i_message_create ();
    if (!msg)
    { 
      i_printf (1, "i_message_read_socketread_callback failed to create message struct"); 
      if (cbdata->callback_func)
      { cbdata->callback_func (self, NULL, cbdata->passdata); }
      i_message_read_cbdata_free (cbdata);
      return -1;
    }

    auth_data = i_data_get_chunk (data, dataptr, datasize, &auth_datasize, &offset);
    if (!auth_data || offset < 1)
    {
      i_printf (1, "i_message_read_socketread_callback failed to get auth_data from op->data");
      if (cbdata->callback_func)
      { cbdata->callback_func (self, NULL, cbdata->passdata); }
      i_message_read_cbdata_free (cbdata);
      i_message_free (msg);
      return -1;
    }
    dataptr += offset;
    msg->auth = i_authentication_data_to_struct (auth_data, auth_datasize);
    free (auth_data);
    if (!msg->auth)
    {
      i_printf (1, "i_message_read_socketread_callback failed to convert auth_data to msg->auth struct");
      if (cbdata->callback_func)
      { cbdata->callback_func (self, NULL, cbdata->passdata); }
      i_message_read_cbdata_free (cbdata);
      i_message_free (msg);
      return -1;
    }

    dst_addr_str = i_data_get_string (data, dataptr, datasize, &offset);
    if (offset < 1)
    {
      i_printf (1, "i_message_read_socketread_callback failed to get dst_addr_str from op->data");
      if (cbdata->callback_func)
      { cbdata->callback_func (self, NULL, cbdata->passdata); }
      i_message_read_cbdata_free (cbdata);
      i_message_free (msg);
      return -1;
    }
    dataptr += offset;
    if (dst_addr_str)
    { 
      msg->dst = i_resource_address_string_to_struct (dst_addr_str); 
      free (dst_addr_str); 
      if (!msg->dst)
      {
        i_printf (1, "i_message_read_socketread_callback failed to convert dst_addr_str to msg->dst struct");
        if (cbdata->callback_func)
        { cbdata->callback_func (self, NULL, cbdata->passdata); }
        i_message_read_cbdata_free (cbdata);
        i_message_free (msg);
        return -1;
      }
    }

    src_addr_str = i_data_get_string (data, dataptr, datasize, &offset);
    if (offset < 1)
    {
      i_printf (1, "i_message_read_socketread_callback failed to get src_addr_str from op->data");
      if (cbdata->callback_func)
      { cbdata->callback_func (self, NULL, cbdata->passdata); }
      i_message_read_cbdata_free (cbdata);
      i_message_free (msg);
      return -1;
    }
    dataptr += offset;
    if (src_addr_str)
    {
      msg->src = i_resource_address_string_to_struct (src_addr_str);
      free (src_addr_str);
      if (!msg->src)
      {
        i_printf (1, "i_message_read_socketread_callback failed to convert src_addr_str to msg->src struct");
        if (cbdata->callback_func)
        { cbdata->callback_func (self, NULL, cbdata->passdata); }
        i_message_read_cbdata_free (cbdata);
        i_message_free (msg);
        return -1;
      }
    }

    msg->type = i_data_get_int (data, dataptr, datasize, &offset);
    if (offset < 1)
    {
      i_printf (1, "i_message_read_socketread_callback failed to get msg->type from op->data");
      if (cbdata->callback_func)
      { cbdata->callback_func (self, NULL, cbdata->passdata); }
      i_message_read_cbdata_free (cbdata);
      i_message_free (msg);
      return -1;
    }
    dataptr += offset;

    msg->flags = i_data_get_int (data, dataptr, datasize, &offset);
    if (offset < 1)
    {
      i_printf (1, "i_message_read_socketread_callback failed to get msg->flags from op->data");
      if (cbdata->callback_func)
      { cbdata->callback_func (self, NULL, cbdata->passdata); }
      i_message_read_cbdata_free (cbdata);
      i_message_free (msg);
      return -1;
    }
    dataptr += offset;

    msg->msgid = i_data_get_long (data, dataptr, datasize, &offset);
    if (offset < 1)
    {
      i_printf (1, "i_message_read_socketread_callback failed to get msg->msgid from op->data");
      if (cbdata->callback_func)
      { cbdata->callback_func (self, NULL, cbdata->passdata); }
      i_message_read_cbdata_free (cbdata);
      i_message_free (msg);
      return -1;
    }
    dataptr += offset;

    msg->reqid = i_data_get_long (data, dataptr, datasize, &offset);
    if (offset < 1)
    {
      i_printf (1, "i_message_read_socketread_callback failed to get msg->reqid from op->data");
      if (cbdata->callback_func)
      { cbdata->callback_func (self, NULL, cbdata->passdata); }
      i_message_read_cbdata_free (cbdata);
      i_message_free (msg);
      return -1;
    }
    dataptr += offset;
    
    msg->data = i_data_get_chunk (data, dataptr, datasize, &datasize, &offset);
    if (offset < 1)
    {
      i_printf (1, "i_message_read_socketread_callback failed to get msg->data from op->data");
      if (cbdata->callback_func)
      { cbdata->callback_func (self, NULL, cbdata->passdata); }
      i_message_read_cbdata_free (cbdata);
      i_message_free (msg);
      return -1;
    }
    dataptr += offset;
    msg->datasize = datasize;

    /* Run the callback */

    if (cbdata->callback_func)
    { cbdata->callback_func (self, msg, cbdata->passdata); }
    
    i_message_read_cbdata_free (cbdata);
    i_message_free (msg);
  }

  return 0;
}

/* Cancel */

void i_message_read_cancel (i_socket *sock)
{
  if (!sock || !sock->msg_read_cbdata)  return;

  if (sock->msg_read_cbdata->size_op) i_socket_read_cancel (sock->msg_read_cbdata->size_op);
  if (sock->msg_read_cbdata->data_op) i_socket_read_cancel (sock->msg_read_cbdata->data_op);
  i_message_read_cbdata_free (sock->msg_read_cbdata);
  sock->msg_read_cbdata = NULL;

  return;
}
