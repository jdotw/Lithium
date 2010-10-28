#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/socket.h>
#include <induction/timer.h>

#include "rrd.h"
#include "socket.h"
#include "status.h"

void module_info ()
{
}

int module_init (i_resource *self)
{
  int num;
  struct sockaddr_in local_addr;
  i_socket *sock;
  i_socket_callback *sockcb;

  /* Initialise rrd */
  num = m_rrd_init (self);
  if (num != 0)
  { i_printf (1, "module_init failed to initialise rrd"); return -1; }

#if (defined (__i386__) || defined( __x86_64__ ))
  /* Fork to start rrdcached */
  pid_t child = fork ();
  if (child == 0)
  {
    execl("/Library/Lithium/LithiumCore.app/Contents/MacOS/lcrrdcached", "lcrrdcached", "-g", "-l", "/var/tmp/.lcrrdcached.sock", "-p", "/var/tmp/.lcrrdcached.pid", NULL);
  }
#endif

  /* Create socket */
  sock = i_socket_create ();
  sock->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock->sockfd == -1) 
  { i_printf (1, "module_init failed to create socket (%s)", strerror(errno)); return -1; }
  fcntl (sock->sockfd, F_SETFL, O_NONBLOCK);

  /* Bind socket */ 
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(51110);
  local_addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(local_addr.sin_zero), '\0', 8);
  num = bind (sock->sockfd, (struct sockaddr *) &local_addr, sizeof(struct sockaddr));
  if (num == -1)
  { i_printf (1, "module_init failed to bind socket (%s)", strerror(errno)); return -1; }

  /* Create socket callback */
  sockcb = i_socket_callback_add (self, SOCKET_CALLBACK_READ, sock, m_socket_readcb, NULL);
  if (!sockcb)
  { i_printf (1, "module_init failed to add socket callback"); return -1; }

  /* Add status timer */
  i_timer_add (self, UPDATE_INT_SEC, 0, m_status_timercb, NULL);
  //i_timer_add (self, 60, 0, m_status_timercb, NULL);

  return 0;
}

int module_entry ()
{
  return 0;
}

int module_shutdown ()
{
  return 0;
}



