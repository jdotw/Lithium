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

m_rrd_proc* m_rrd_proc_spawn (i_resource *self)
{
  int fd1[2];
  int fd2[2];
  m_rrd_proc *proc;
  i_list *list;

  /* Create struct */
  proc = (m_rrd_proc *) malloc (sizeof(m_rrd_proc));
  memset (proc, 0, sizeof(m_rrd_proc));

  /* Create pipes */
  int piperes = pipe (fd1);
  if (piperes != 0) i_printf(1, "m_rrd_proc_spawn warning: failed to pipe for fd1");
  proc->writefd = fd1[1];
  piperes = pipe (fd2);
  if (piperes != 0) i_printf(1, "m_rrd_proc_spawn warning: failed to pipe for fd1");
  proc->readfd = fd2[0];

  /* Fork */
  proc->pid = fork ();
  if (proc->pid > 0)
  {
    /* Parent Process */
    close (fd1[0]);
    close (fd2[1]);

    /* Create socket */
    proc->sock = i_socket_create ();
    proc->sock->sockfd = proc->readfd;
    fcntl (proc->readfd, F_SETFL, O_NONBLOCK);

    /* Add read callback */
    proc->sockcb = i_socket_callback_add (self, SOCKET_CALLBACK_READ, proc->sock, m_rrd_proc_sockcb, proc);

    /* Add to process list */
    list = m_rrd_proclist ();
    i_list_enqueue (list, proc);
    list = m_rrd_freeproclist ();
    i_list_enqueue (list, proc);
  }
  else
  {
    /* Child (RRD) Process */
    free (proc);
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
    execlp ("lcrrdtool", "lcrrdtool", "-", NULL);

    exit (1);
  }
  
  return proc;
}

int m_rrd_proc_kill (i_resource *self, m_rrd_proc *proc)
{
  int num;
  i_list *list;
  
  /* Remove from lists */
  list = m_rrd_proclist ();
  if (list)
  { num = i_list_search (list, proc); if (num == 0) i_list_delete (list); }
  list = m_rrd_freeproclist ();
  if (list)
  { num = i_list_search (list, proc); if (num == 0) i_list_delete (list); }
  
  /* Terminate process */
  if (proc->pid > 0) kill (proc->pid, SIGTERM);
  if (proc->sockcb) i_socket_callback_remove (proc->sockcb);
  if (proc->sock) { proc->sock->sockfd = -1; i_socket_free (proc->sock); }
  free (proc);

  return 0;
}

/* Socket Callback */

int m_rrd_proc_sockcb (i_resource *self, i_socket *sock, void *passdata)
{
  /* Always return 0 to keep callback alive */
  int num;
  char line[512];
  i_list *list;
  m_rrd_proc *proc = passdata;

  /* Read */
  num = read (sock->sockfd, line, 511);
  if (num > 0)
  {
    /* Terminate string */
    line[num] = '\0';

    /* Check for errors */
    if (strstr(line, "ERROR"))
    {
      if (!strstr(line, "illegal attempt to update using time"))
      {
        if (strstr(line, "Unable to connect to rrdcached"))
        {
          /* Cached is not responding, terminate ourselves in an attempt to
           * restart it 
           * */
          i_printf(1, "m_rrd_proc_sockcb rrdcachd has become unreachable, terminating");
          exit(1);
        }
        else
        {
          i_printf (1, "m_rrd_proc_sockcb rrdtool error response: %s", line); 
        }
      }
    }
    
    /* Return process to the free proc list */
    list = m_rrd_freeproclist ();
    i_list_enqueue (list, proc);
  }
  else
  {
    /* Error - Kill process */
    i_printf (1, "m_rrd_proc_sockcb killing failed rrd process"); 
    m_rrd_proc_kill (self, proc);
  }

  /* Call the queue runner */
  m_rrd_queuerunner (self);

  return 0;
}
