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

i_socket* i_socket_create_unix (char *path, int type)
{
  int len;
  int num;
  i_socket *sock;

  sock = i_socket_create ();
  if (!sock) 
  { i_printf (1, "i_socket_create_unix failed to create socket struct"); return NULL; }
  sock->type = SOCKET_UNIX;

  sock->sockfd = socket (AF_UNIX, SOCK_STREAM, 0);
  if (sock->sockfd == -1) 
  { i_printf (1, "i_socket_create_unix failed to call socket()"); i_socket_free (sock); return NULL; }

  sock->addr_un.sun_family = AF_UNIX;
  strncpy (sock->addr_un.sun_path, path, sizeof(sock->addr_un.sun_path)-1);
  len = SUN_LEN (&sock->addr_un);
  fcntl (sock->sockfd, F_SETFL, O_NONBLOCK);

  if (type == SOCKET_LISTEN)
  {
    unlink (sock->addr_un.sun_path);

    num = bind (sock->sockfd, (struct sockaddr *)&sock->addr_un, len);
    if (num == -1) 
    { i_printf (1, "i_socket_create_unix failed to bind socket"); i_socket_free (sock); return NULL; }

    num = listen (sock->sockfd, 100);
    if (num == -1) 
    { i_printf (1, "i_socket_create_unix failed to put socket in listen state"); i_socket_free (sock); return NULL; }
  }
  else
  {
    num = connect (sock->sockfd, (struct sockaddr *)&sock->addr_un, len);
    if (num == -1) 
    { i_printf (1, "i_socket_create_unix failed to connect socket"); i_socket_free (sock); return NULL; }
  }

  return sock;
}

i_socket* i_socket_accept_unix (int listener)
{
  socklen_t len;
  i_socket *sock;

  sock = i_socket_create ();
  if (!sock) 
  { i_printf (1, "i_socket_accept_unix failed to malloc socket struct"); return NULL; }
  sock->type = SOCKET_UNIX;

  len = sizeof (struct sockaddr_un);
  sock->sockfd = accept (listener, (struct sockaddr *)&sock->addr_un, &len);
  if (sock->sockfd < 1) 
  { 
    i_printf (1, "i_socket_accept_unix failed to accept() incoming connection (%s)", strerror(errno));
    i_socket_free (sock);
    return NULL; 
  }

  fcntl (sock->sockfd, F_SETFL, O_NONBLOCK);

  return sock;
}
  

