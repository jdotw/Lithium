#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
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
#include "timer.h"

/* Create socket */

i_socket* i_socket_create_tcp (i_resource *self, char *address, int port, int (*callback_func) (i_resource *self, i_socket *sock, void *passdata), void *passdata)
{
  /* Instigates the connection process, calls
   * callback once the connection is established.
   */

  int num;
  i_socket *sock;

  if (!self) return NULL;

  sock = i_socket_create ();
  if (!sock) 
  { i_printf (1, "i_socket_create_tcp failed to create socket struct"); return NULL; }
  sock->type = SOCKET_TCP;

  sock->sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock->sockfd == -1)
  { i_printf (1, "i_socket_create_tcp failed to call socket()"); i_socket_free (sock); return NULL; }

  fcntl (sock->sockfd, F_SETFL, O_NONBLOCK);					/* Go to non-blocking mode */
  sock->addr_in.sin_family = AF_INET;      						/* host byte order */
  sock->addr_in.sin_port = htons (port);    					/* short, network byte order */

  if (address)	/* Socket is to be connected */
  {
    int num;
    struct hostent *he;

    he = gethostbyname(address);
    if (!he)
    { i_printf (1, "i_socket_create_tcp failed to resolve address '%s'", address); i_socket_free (sock); return NULL; }

    sock->addr_in.sin_addr = *((struct in_addr *)he->h_addr);	
    bzero(&(sock->addr_in.sin_zero), 8);					/* zero the rest of the struct */

    num = connect (sock->sockfd, (struct sockaddr *)&sock->addr_in, sizeof(struct sockaddr));
    if (num == -1)
    {
      if (errno == EINPROGRESS)
      {
        /* Connect in progress, install callback */
        sock->connect_data = i_socket_connect_data_create ();
        if (!sock->connect_data)
        { i_printf (1, "i_socket_create_tcp failed to malloc connect_data"); i_socket_free (sock); return NULL; }
        sock->connect_data->socket = sock;
        sock->connect_data->callback_func = callback_func;
        sock->connect_data->passdata = passdata;
        
        sock->connect_data->socket_callback = i_socket_callback_add (self, SOCKET_CALLBACK_WRITE_PREEMPT, sock, i_socket_create_tcp_socket_callback, NULL);
        if (!sock->connect_data->socket_callback)
        { i_printf (1, "i_socket_create_tcp failed to add socket_callback"); i_socket_free (sock); return NULL; }

        sock->connect_data->timeout_timer = i_timer_add (self, SOCKET_TCP_CONNECT_TIMEOUT_SEC, 0, i_socket_create_tcp_timeout_callback, sock);
        if (!sock->connect_data->timeout_timer)
        { i_printf (1, "i_socket_create_tcp failed to add timeout_timer"); i_socket_free (sock); return NULL; }
      }
      else
      {
        /* Error occurred */
        i_printf (1, "i_socket_create_tcp failed to create connection to %s:%i (%s)", address, port, strerror(errno));
        i_socket_free (sock);
        return NULL;
      }
    }
    else
    {
      /* Connect worked instantly */
      callback_func (self, sock, passdata);
    }
  }
  else          /* Socket is a listener */
  {
    int sockopt_val = 1;

    sock->addr_in.sin_addr.s_addr = INADDR_ANY;
    bzero(&(sock->addr_in.sin_zero), 8);

    num = setsockopt (sock->sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt_val, sizeof(sockopt_val));
    if (num < 0)
    { i_printf (1, "i_socket_create_tcp setsockopt failed (%s)", strerror(errno)); i_socket_free (sock); return NULL; }

    num = bind (sock->sockfd, (struct sockaddr *)&sock->addr_in, sizeof(struct sockaddr));
    if (num == -1) 
    { i_printf (1, "i_socket_create_tcp bind fails (%s) on port %i", strerror(errno), port); i_socket_free (sock); return NULL; }

    num = listen(sock->sockfd, 10);
    if (num == -1) 
    { i_printf (1, "i_socket_create_tcp listed() failed (%s)", strerror(errno)); i_socket_free (sock); return NULL; }
  }
  
  return sock;
}

int i_socket_create_tcp_socket_callback (i_resource *self, i_socket *sock, void *passdata)
{
  /* Always return -1, non-persistent callback */

  int num;
  int errcode;
  socklen_t optlen;

  optlen = sizeof(int);
  num = getsockopt (sock->sockfd, SOL_SOCKET, SO_ERROR, &errcode, &optlen);
  if (num != 0 || errcode != 0)
  {
    /* Error occurred */
    i_printf (2, "i_socket_create_tcp_socket_callback encountered error connecting socket");
    if (sock->connect_data->callback_func)
    { sock->connect_data->callback_func (self, NULL, sock->connect_data->passdata); }
    sock->connect_data->socket_callback = NULL;
    i_socket_free (sock);
    return -1;
  }

  /* Successfully Connected */
  i_printf (2, "i_socket_create_tcp_socket_callback successfully connected socket");
  if (sock->connect_data->callback_func)
  { sock->connect_data->callback_func (self, sock, sock->connect_data->passdata); }
  sock->connect_data->socket_callback = NULL;
  i_socket_connect_data_free (sock->connect_data);
  sock->connect_data = NULL;

  return -1;
}

int i_socket_create_tcp_timeout_callback (i_resource *self, i_timer *timer, void *passdata)
{
  /* Always return -1, non-persistent callback */
  i_socket *sock = passdata;
  
  i_printf (2, "i_socket_create_tcp_timeout_callback timeout occurred");
  sock->connect_data->callback_func (self, NULL, sock->connect_data->passdata);
  sock->connect_data->timeout_timer = NULL;
  i_socket_free (sock);
  return -1;
}

/* Accept Connection */

i_socket* i_socket_accept_tcp (int listenfd)
{
  /* Note, this doesnt need to be done in an async way
   * because we assume a select() has been done on the
   * socket to ensure something is ready to be read
   * from the socket 
   */

  socklen_t sin_size;
  i_socket *sock;

  sock = i_socket_create ();
  if (!sock) 
  { i_printf (1, "i_socket_accept_tcp failed to create socket struct"); return NULL; }
  sock->type = SOCKET_TCP;

  sin_size = sizeof (struct sockaddr_in);
  sock->sockfd = accept (listenfd, (struct sockaddr *) &sock->addr_in, &sin_size);
  if (sock->sockfd < 1) 
  { i_printf (1, "i_socket_accept_tcp failed to accept incoming connection"); i_socket_free (socket); return NULL; }

  fcntl (sock->sockfd, F_SETFL, O_NONBLOCK);

  return sock;
}

