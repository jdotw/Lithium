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
#include <signal.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/socket.h>

#include "rrd.h"

/* Process management */

static int writefd;
static int readfd;
static i_socket *sock;
static i_socket_callback *sockcb;
int m_rrdcached_sockcb (i_resource *self, i_socket *sock, void *passdata);

int m_rrdcached_spawn (i_resource *self)
{
  int fd1[2];
  int fd2[2];

  /* Create pipes */
  pipe (fd1);
  writefd = fd1[1];
  pipe (fd2);
  readfd = fd2[0];

  /* Fork */
  pid_t pid = fork ();
  if (pid > 0)
  {
    /* Parent Process */
    close (fd1[0]);
    close (fd2[1]);

    /* Create socket */
    sock = i_socket_create ();
    sock->sockfd = readfd;
    fcntl (readfd, F_SETFL, O_NONBLOCK);

    /* Add read callback */
    sockcb = i_socket_callback_add (self, SOCKET_CALLBACK_READ, sock, m_rrdcached_sockcb, NULL);
  }
  else
  {
    /* Child (RRD) Process */
    close (fd1[1]);
    close (fd2[0]);

    /* Set up pipes/fds */
    if (fd1[0] != STDIN_FILENO)
    {
      if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO) fprintf (stdout, "dup2 error to stdin");
      close(fd1[0]);
    }
    if (fd2[1] != STDOUT_FILENO)
    {
      if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO) fprintf (stdout, "dup2 error to stdout");
      close(fd2[1]);
    }

    /* Exec rrdtool */
    execlp("lcrrdcached", "lcrrdcached", "-g", "-l", "/var/tmp/.lcrrdcached.sock", "-p", "/var/tmp/.lcrrdcached.pid", NULL);

    exit (1);
  }
  
  return 0;
}

/* Socket Callback */

int m_rrdcached_sockcb (i_resource *self, i_socket *sock, void *passdata)
{
  /* Always return 0 to keep callback alive */
  int num;
  char line[512];

  /* Read */
  num = read (sock->sockfd, line, 511);
  if (num > 0)
  {
    line[num] = '\0';
    i_printf(1, "m_rrdcached_sockcb received '%s' from rrdcached", line);
  }
  else
  {
    /* Error */
    i_printf (1, "m_rrdcached_sockcb warning, rrdcached failed"); 
    return -1;
  }

  return 0;
}
