#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <syslog.h>
#include <sys/un.h>
#include <string.h>

#include "induction.h"
#include "socket.h"
#include "list.h"

/* Read */

i_socket_data* i_socket_read (i_resource *self, i_socket *sock, int datasize, int (*callback_func) (i_resource *self, i_socket *sock, i_socket_data *data, int result, void *passdata), void *passdata)
{
  /* Enqueue the read operation */

  int num;
  i_socket_data *op;

  if (!sock || datasize < 1) return NULL;

  /* Create/Setup socket_data operation struct */

  op = i_socket_data_create ();
  if (!op)
  { i_printf (1, "i_socket_read failed to create i_socket_data struct"); return NULL; }
  op->socket = sock;
  op->data = malloc (datasize);
  if (!op->data)
  { i_printf (1, "i_socket_read failed to malloc op->data (%i bytes)", datasize); i_socket_data_free (op); return NULL; }
  memset (op->data, 0, datasize);
  op->datasize = datasize;
  op->offset = 0;
  op->callback_func = callback_func;
  op->passdata = passdata;

  /* Install read socket callback if necessary */

  if (!sock->read_socket_callback)
  {
    sock->read_socket_callback = i_socket_callback_add (self, SOCKET_CALLBACK_READ, sock, i_socket_read_socket_callback, NULL);
    if (!sock->read_socket_callback)
    { i_printf (1, "i_socket_read failed to install socket_read callback"); i_socket_data_free (op); return NULL; }
  }
  
  /* Enqueue the read operation */

  if (!sock->read_pending)
  {
    sock->read_pending = i_list_create ();
    if (!sock->read_pending)
    { 
      i_printf (1, "i_socket_read failed to create socket->read_pending");
      i_socket_data_free (op);
      i_socket_callback_remove (sock->read_socket_callback);
      sock->read_socket_callback = NULL;
      return NULL;
    }
    i_list_set_destructor (sock->read_pending, i_socket_data_free);
  }
  
  num = i_list_enqueue (sock->read_pending, op);
  if (num != 0)
  { 
    i_printf (1, "i_socket_read failed to enqueue data op to socket->read_pending"); 
    i_socket_data_free (op);
    i_socket_callback_remove (sock->read_socket_callback);
    sock->read_socket_callback = NULL;
    i_socket_data_free (op);
    if (sock->read_pending && sock->read_pending->size < 1)
    { i_list_free (sock->read_pending); sock->read_pending = NULL; }
    return NULL; 
  }

  /* Finished */

  return op;
}

int i_socket_read_socket_callback (i_resource *self, i_socket *sock, void *passdata)
{
  /* Called when select determiend that there is data
   * to be read from the socket 
   *
   * Return 0 to keep the callback active (i.e pending reads)
   * Return -1 to cancel the callback
   */

  int num;
  int count;
  i_socket_data *op;
  fd_set fdset;
  struct timeval timeout;

  /* Find the first pending read operation */
  
  i_list_move_head (sock->read_pending);
  op = i_list_restore (sock->read_pending);
  if (!op)
  {
    i_printf (1, "i_socket_read_socket_callback failed to restore initial pending read operation"); 
    if (sock->read_pending)
    { i_list_free (sock->read_pending); }
    sock->read_pending = NULL;
    sock->read_socket_callback = NULL;
    return -1;
  }

  /* Call select to make sure there really is
   * something to read. This is done to ensure that 
   * this function hasnt been called prematurely.
   */

  FD_ZERO (&fdset);
  FD_SET (sock->sockfd, &fdset);
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  num = select (sock->sockfd+1, &fdset, NULL, NULL, &timeout);
  if (num < 1)
  {
    /* This function was called prematurely */
    return 0;
  }

  /* Attempt to read */

  count = read (sock->sockfd, op->data + op->offset, op->datasize - op->offset);
  if (count > 0)
  {
    /* Some (if not all) data read */
    
    op->offset += count;
    
    if (op->offset == op->datasize)
    {
      /* Read operation complete */
      int ret_val;
      i_socket_data *dup_op;

      dup_op = i_socket_data_duplicate (op);
      num = i_list_search (sock->read_pending, op);
      if (num == 0)
      { i_list_delete (sock->read_pending); }

      /* op could now be freed (and likely is) */
      
      if (sock->read_pending->size < 1)
      {
        /* No more readss pending, free pending
         * list and remove callback (return -1)
         */
        i_list_free (sock->read_pending);
        sock->read_pending = NULL;
        sock->read_socket_callback = NULL;
        ret_val = -1;
      }
      else
      { ret_val = 0; }

      if (dup_op->callback_func)
      { dup_op->callback_func (self, sock, dup_op, SOCKET_RESULT_SUCCESS, dup_op->passdata); }
      i_socket_data_free (dup_op);

      /* For all we know, the whole socket could be freed now */

      return ret_val;
    }
  }
  else if (count == -1 && errno != EAGAIN)
  {
    /* A FATAL error has occurred. Because these errors are fatal, 
     * all pending operations are removed, the callback is
     * called with the failed result and the callback removed instantly.
     *
     * There is a good chance the socket will be freed in the callback call,
     * hence the operation list is freed, callback pointer removed, etc
     * first
     */

    void *callback_passdata = op->passdata;
    int (*callback_func) () = op->callback_func;

    i_printf (1, "i_socket_read_socket_callback encountered fatal error. read() returned -1 (%s)", strerror(errno));

    i_list_free (sock->read_pending);
    sock->read_pending = NULL;
    sock->read_socket_callback = NULL;

    if (callback_func)
    { callback_func (self, sock, NULL, SOCKET_RESULT_FAILED, callback_passdata); }

    return -1;
  }
  else if (count == 0)
  {
    /* Socket has been disconnected. */
    void *callback_passdata = op->passdata;
    int (*callback_func) () = op->callback_func;

    i_printf (2, "i_socket_read_socket_callback failed, socket has been disconnected. read() returned 0 and errno = 0");

    i_list_free (sock->read_pending);
    sock->read_pending = NULL;
    sock->read_socket_callback = NULL;

    if (callback_func)
    { callback_func (self, sock, NULL, SOCKET_RESULT_FAILED, callback_passdata); }

    return -1;
  }

  /* Read operations still pending, return 0 */

  return 0;
}
  
int i_socket_read_cancel (i_socket_data *op)
{
  /* FIX needs to cancel the read op */
  
  return 0;
}
