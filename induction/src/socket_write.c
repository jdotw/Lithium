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

/* Write */

i_socket_data* i_socket_write (i_resource *self, i_socket *sock, void *data, int datasize, int (*callback_func) (i_resource *self, i_socket *sock, int result, void *passdata), void *passdata)
{
  /* Enqueue the write operation */

  int num;
  i_socket_data *op;

  if (!sock || !data || datasize < 1) return NULL;

  /* Create/Setup socket_data operation struct */
  
  op = i_socket_data_create ();
  if (!op)
  { i_printf (1, "i_socket_write failed to create i_socket_data struct"); return NULL; }
  op->socket = sock;
  op->data = malloc (datasize);
  if (!op->data)
  { i_printf (1, "i_socket_write failed to malloc op->data (%i bytes)", datasize); i_socket_data_free (op); return NULL; }
  memcpy (op->data, data, datasize);
  op->datasize = datasize;
  op->offset = 0;
  op->callback_func = callback_func;
  op->passdata = passdata;
  gettimeofday(&op->queued_tstamp, NULL);

  /* Install write callback is necessary */

  if (!sock->write_socket_callback)
  { 
    sock->write_socket_callback = i_socket_callback_add (self, SOCKET_CALLBACK_WRITE, sock, i_socket_write_socket_callback, NULL);
    if (!sock->write_socket_callback)
    { i_printf (1, "i_socket_write failed to install write_callback for socket"); i_socket_data_free (op); return NULL; }
  }

  /* Enqueue the write operation */

  if (!sock->write_pending)
  { 
    sock->write_pending = i_list_create ();
    if (!sock->write_pending)
    { 
      i_printf (1, "i_socket_write failed to create sock->write_pending"); 
      i_socket_data_free (op); 
      i_socket_callback_remove (sock->write_socket_callback);
      sock->write_socket_callback = NULL;
      return NULL; 
    }
    i_list_set_destructor (sock->write_pending, i_socket_data_free);
  }

  num = i_list_enqueue (sock->write_pending, op);
  if (num != 0)
  { 
    i_printf (1, "i_socket_write failed to enqueue data op struct to sock->write_pending"); 
    i_socket_data_free (op); 
    i_socket_callback_remove (sock->write_socket_callback);
    sock->write_socket_callback = NULL;
    if (sock->write_pending && sock->write_pending->size < 1)
    { i_list_free (sock->write_pending); sock->write_pending = NULL; }
    return NULL; 
  }

  /* Finished */

  return op;
}
  
int i_socket_write_socket_callback (i_resource *self, i_socket *sock, void *passdata)
{
  /* Called when select determines that the socket
   * can be written to without blocking
   *
   * Return 0 to keep the callback active (i.e further writes pending)
   * Return -1 to remove the callback (i.e all writes finished)
   */

  int num;

  /*
   * Though this looks ineffient, the write-one-op-at-a-time
   * is done because after performing the write and calling the
   * callback, we don't know if the socket actually still exists
   */

  /* De-queue write op */
  i_list_move_head (sock->write_pending);
  i_socket_data *op = i_list_restore (sock->write_pending);
  if (!op)
  { 
    i_printf (1, "i_socket_write_socket_callback failed to restore first pending write operation"); 
    if (sock->write_pending)
    { i_list_free (sock->write_pending); }
    sock->write_pending = NULL;
    sock->write_socket_callback = NULL;
    return -1;
  }

  /* Attempt to write */
  int count = write (sock->sockfd, op->data + op->offset, op->datasize - op->offset);
  if (count > 0)
  {
    /* Some (if not all) data sent */
    op->offset += count;
    if (op->offset == op->datasize)
    {
      /* Write operation complete */
      int ret_val;
      i_socket_data *dup_op;
      struct timeval now;

      gettimeofday(&now, NULL);

      dup_op = i_socket_data_duplicate (op);
      num = i_list_search (sock->write_pending, op);
      if (num == 0)
      { i_list_delete (sock->write_pending); }
    
      /* op is likely freed at this stage */

      if (sock->write_pending->size < 1)
      {
        /* No more writes pending, free pending
         * list and remove callback (return -1)
         */
        i_list_free (sock->write_pending);
        sock->write_pending = NULL;
        sock->write_socket_callback = NULL;
        ret_val = -1;
      }
      else { ret_val = 0; }
      
      if (dup_op->callback_func)
      { dup_op->callback_func (self, sock, SOCKET_RESULT_SUCCESS, dup_op->passdata); }
      i_socket_data_free (dup_op);

      /* for all we know the whole socket could be freed now */

      return ret_val;
    }

  }
  else if (count == -1)    
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

    i_printf (1, "i_socket_write_socket_callback encountered fatal error. write() returned -1 (%s)", strerror(errno));
     
    i_list_free (sock->write_pending);
    sock->write_pending = NULL;
    sock->write_socket_callback = NULL;

    if (callback_func)
    { callback_func (self, sock, SOCKET_RESULT_FAILED, callback_passdata); }

    return -1;
  }
  else if (count == 0)
  {
    /* Socket disconnected */
    void *callback_passdata = op->passdata;
    int (*callback_func) () = op->callback_func;

    i_printf (1, "i_socket_read_socket_callback failed, socket has been disconnected. read() returned 0 and errno = 0");

    i_list_free (sock->write_pending);
    sock->write_pending = NULL;
    sock->write_socket_callback = NULL;

    if (callback_func)
    { callback_func (self, sock, SOCKET_RESULT_FAILED, callback_passdata); }

    return -1;

  }

  /* Write operations still pending, return 0 */

  return 0;
}

int i_socket_write_cancel (i_socket_data *op)
{
  /* FIX needs to cancel socket write op */
  return 0;
}
