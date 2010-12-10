#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "induction.h"
#include "timer.h"
#include "socket.h"
#include "list.h"
#include "message.h"
#include "msgproc.h"

extern i_resource *global_self;

/* Socket Struct Manipulation */

i_socket* i_socket_create ()
{
  i_socket *sock;

  sock = (i_socket *) malloc (sizeof(i_socket));
  if (!sock) 
  { i_printf (1, "i_socket_create failed to malloc socket"); return NULL; }
  memset (sock, 0, sizeof(i_socket));

  /* Do not create the read/write pending lists,
   * they are created as needed
   */ 

  return sock;
}

void i_socket_free (void *socketptr)
{
  i_socket *sock = socketptr;

  if (!sock) return;
  
  if (sock->sockfd > 0) close (sock->sockfd);

  /* Disable i_msgproc if present */

  if (sock->msgproc)
  { i_msgproc_disable (global_self, sock); }

  /* Remove pending read/write operations */
  
  if (sock->read_pending)
  {
    i_socket_data *op;
    for (i_list_move_head(sock->read_pending); (op=i_list_restore(sock->read_pending))!=NULL; i_list_move_next(sock->read_pending))
    { i_socket_read_cancel (op); }
    i_list_free (sock->read_pending);
  }
  
  if (sock->write_pending)
  {
    i_socket_data *op;
    for (i_list_move_head(sock->write_pending); (op=i_list_restore(sock->write_pending))!=NULL; i_list_move_next(sock->write_pending))
    { i_socket_write_cancel (op); }
    i_list_free (sock->write_pending);
  }

  /* Remove socket callbacks */
  
  i_socket_callback_remove_by_socket (sock);

  /* Free callback any data */
  
  if (sock->connect_data) i_socket_connect_cancel (sock);
  if (sock->msg_read_cbdata) i_message_read_cancel (sock);

  free (sock);
}

/* Socket Data Struct Manipulation */

i_socket_data* i_socket_data_create ()
{
  i_socket_data *data;

  data = (i_socket_data *) malloc (sizeof(i_socket_data));
  if (!data)
  { i_printf (1, "i_socket_create_data failed to malloc data struct"); return NULL; }
  memset (data, 0, sizeof(i_socket_data));

  return data;
}

void i_socket_data_free (void *dataptr)
{
  i_socket_data *data = dataptr;

  if (!data) return;

  if (data->data) free (data->data);

  free (data);
}

i_socket_data* i_socket_data_duplicate (i_socket_data *orig)
{
  i_socket_data *dup;

  if (!orig) return NULL;

  dup = i_socket_data_create ();
  if (!dup)
  { i_printf (1, "i_socket_data_duplicate failed to create dup struct"); return NULL; }
  dup->socket = orig->socket;
  if (orig->data && orig->datasize > 0)
  {
    dup->data = malloc (orig->datasize);
    memcpy (dup->data, orig->data, orig->datasize);
    dup->datasize = orig->datasize;
  }
  dup->offset = orig->offset;
  dup->callback_func = orig->callback_func;
  dup->passdata = orig->passdata;

  return dup;
}



/* Socket Connect Data Struct Manipulation */

i_socket_connect_data* i_socket_connect_data_create ()
{
  i_socket_connect_data *data;

  data = (i_socket_connect_data *) malloc (sizeof(i_socket_connect_data));
  if (!data)
  { i_printf (1, "i_socket_connect_data_create failed to malloc struct"); return NULL; }
  memset (data, 0, sizeof(i_socket_connect_data));

  return data;
}

void i_socket_connect_data_free (void *dataptr)
{
  i_socket_connect_data *data = dataptr;

  if (!data) return;

  if (data->timeout_timer)
  { i_timer_remove (data->timeout_timer); }
  if (data->socket_callback)
  { i_socket_callback_remove (data->socket_callback); }

  free (data);
}

void i_socket_connect_cancel (i_socket *sock)
{
  if (!sock || !sock->connect_data) return;

  if (sock->connect_data->socket_callback) i_socket_callback_remove (sock->connect_data->socket_callback);
  if (sock->connect_data->timeout_timer) i_timer_remove (sock->connect_data->timeout_timer);

  i_socket_connect_data_free (sock->connect_data);
  sock->connect_data = NULL;
}
