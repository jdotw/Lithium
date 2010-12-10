#include <stdlib.h>

#include "induction.h"
#include "socket.h"
#include "message.h"
#include "msgproc.h"

int i_msgproc_read_preempt_callback (i_resource *self, i_socket *sock, void *passdata)
{
  /* Called when there is data to be read off a msgproc-enabled socket
   *
   * Return 0 to keep preempt callback alive
   */

  int num;
  
  if (!sock->msgproc)
  { i_printf (1, "i_msgproc_read_preempt_callback called with NULL sock->msgproc"); return -1; }

  num = i_message_read (self, sock, i_msgproc_read_msg_callback, sock);
  if (num != 0)
  { i_printf (1, "i_msgproc_read_preempt_callback failed to call i_message_read"); return -1; }

  return 0;
}

int i_msgproc_read_msg_callback (i_resource *self, i_message *msg, void *passdata)
{
  /* 
   * Called when a message has been recvd on a msgproc-enabled socket
   */

  int num;
  i_socket *sock = passdata;

  if (!msg)
  { 
    i_printf (2, "i_msgproc_read_msg_callback encountered a fatal error recving a message"); 
    i_msgproc_fatal (self, sock);
    return -1;
  }

  num = i_msgproc_process (self, sock, msg);
  if (num != 0)
  { i_printf (2, "i_msgproc_read_msg_callback failed to process recvd message"); }

  return 0;
}
  
