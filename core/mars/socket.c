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

#include "rrd.h"
#include "socket.h"

#define BUFLEN 10000

static unsigned long static_update_count = 0;

unsigned long m_socket_updatecount ()
{ return static_update_count; }

void m_socket_updatecount_reset ()
{ static_update_count = 0; }

int m_socket_readcb (i_resource *self, i_socket *sock, void *passdata)
{
  /* ALWAYS RETURN 0 to keep callback alive */
  
  int num;
  char buf[BUFLEN];
  socklen_t addr_len;
  struct sockaddr_in remote_addr;

  /* Receive packet */
  addr_len = sizeof(struct sockaddr);
  num = recvfrom (sock->sockfd, buf, BUFLEN-1, MSG_WAITALL, (struct sockaddr *) &remote_addr, &addr_len);
  if (num == -1)
  { i_printf (1, "m_socket_readcb failed to call recvfrom %s", strerror(errno)); return 0; }
  buf[num] = '\0';

  /* Extract Priority and command */
  char *bufptr = buf;
  uint32_t priority;
  memcpy (&priority, bufptr, sizeof(uint32_t));
  bufptr += sizeof(uint32_t);
  char *command_str = strdup (bufptr);

  /* Execute command */
  m_rrd_exec (self, priority, command_str);
  free (command_str);
  static_update_count++;

  return 0;
}
