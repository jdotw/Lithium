#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <libxml/parser.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

#include <induction.h>
#include <induction/callback.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/list.h>
#include <induction/postgresql.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/incident.h>
#include <induction/xml.h>

#include "netscan.h"

#define PIPE_TIMEOUT_SECONDS 450

/* Process Struct Manipulation */

l_netscan_proc* l_netscan_proc_create ()
{
  l_netscan_proc *proc;

  proc = (l_netscan_proc *) malloc (sizeof(l_netscan_proc));
  if (!proc)
  { i_printf (1, "l_netscan_proc_create failed to malloc l_netscan_proc struct"); return NULL; }
  memset (proc, 0, sizeof(l_netscan_proc));

  return proc;
}

void l_netscan_proc_free (void *procptr)
{
  l_netscan_proc *proc = procptr;

  if (!proc) return;

  if (proc->network_str) free (proc->network_str);
  if (proc->mask_str) free (proc->mask_str);
  if (proc->pid > 0) kill (proc->pid, SIGKILL);
  if (proc->readfd > 0) close (proc->readfd);
  if (proc->writefd > 0) close (proc->writefd);
  if (proc->sockcb) i_socket_callback_remove (proc->sockcb);
  if (proc->sock) { proc->sock->sockfd = -1; i_socket_free (proc->sock); }
  if (proc->timeout_timer) i_timer_remove (proc->timeout_timer);
  if (proc->output_str) free (proc->output_str);
  if (proc->cb) i_callback_free (proc->cb);

  free (proc);
}

/* Network Scan Execution */

l_netscan_proc* l_netscan_scan (i_resource *self, char *network_str, char *mask_str, int (*cbfunc) (), void *passdata)
{
  l_netscan_proc *proc;

  /* Create process struct */
  proc = l_netscan_proc_create ();
  if (!proc)
  { i_printf (1, "l_netscan_scan failed to create proc struct"); return NULL; }
  proc->network_str = strdup (network_str);
  if (mask_str) proc->mask_str = strdup (mask_str);

  /* Add callback if specified */
  if (cbfunc)
  {
    proc->cb = i_callback_create ();
    if (!proc->cb)
    { i_printf (1, "l_netscan_scan failed to create proc->cb"); l_netscan_proc_free (proc); return NULL; }
    proc->cb->func = cbfunc;
    proc->cb->passdata = passdata;
  }

  /* Create pipes */
  int num;
  int fd1[2];
  int fd2[2];
  num = pipe (fd1);
  if (num < 0)
  { i_printf (1, "l_netscan_scan failed to call pipe for fd1"); l_netscan_proc_free (proc); return NULL; }
  proc->writefd = fd1[1];
  num = pipe (fd2);
  if (num < 0)
  { i_printf (1, "l_netscan_scan failed to call pipe for fd2"); l_netscan_proc_free (proc); return NULL; }
  proc->readfd = fd2[0];

  /* Fork */
  proc->pid = fork ();
  if (proc->pid < 0)
  { i_printf (1, "l_netscan_scan failed to call fork()"); l_netscan_proc_free (proc); return NULL; }

  if (proc->pid > 0)
  {
    /* Parent Process */
    close(fd1[0]);
    close(fd2[1]);

    /* Create 'socket' struct for the readfd */
    proc->sock = i_socket_create ();
    if (!proc->sock)
    { i_printf (1, "l_netscan_scan failed to create blank socket"); l_netscan_proc_free (proc); return NULL; }
    proc->sock->sockfd = proc->readfd;
    fcntl (proc->readfd, F_SETFL, O_NONBLOCK);

    /* Install readfd socket callback */
    proc->sockcb = i_socket_callback_add (self, SOCKET_CALLBACK_READ, proc->sock, l_netscan_scan_socketcb, proc);
    if (!proc->sockcb)
    { i_printf (1, "l_netscan_scan failed to add socket callback"); l_netscan_proc_free (proc); return NULL; }

    /* Install timeout */
    proc->timeout_timer = i_timer_add (self, PIPE_TIMEOUT_SECONDS, 0, l_netscan_scan_timeoutcb, proc);
    if (!proc->timeout_timer)
    { i_printf (1, "l_netscan_scan failed to add timeout callback"); l_netscan_proc_free (proc); return NULL; }
  }
  else
  {
    /* Action Child */
    free (proc);
    close(fd1[1]);
    close(fd2[0]);
    
    if (fd1[0] != STDIN_FILENO)
    {
      if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
        fprintf (stdout, "dup2 error to stdin");
      close(fd1[0]);
    } 
    if (fd2[1] != STDOUT_FILENO)
    {
      if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
        fprintf (stdout, "dup2 error to stdout");
      close(fd2[1]);
    } 

    /* Exec */
    if (mask_str)
    {
      struct in_addr maskaddr;
      inet_aton (mask_str, &maskaddr);
      if (~maskaddr.s_addr == 0)
      {
        // asprintf (&args, "/opt/local/bin/nmap -O -sS -sU -p U:161,623,T:80,311 -oX - %s/32", network_str); 
        //asprintf (&args, "nmap -O -sU -p U:161 -oX - %s/32", network_str); 
        char *network_arg;
        asprintf (&network_arg, "%s/32", network_str);
        num = execlp ("nmap", "nmap", "-O", "-sU", "-p", "U:161", "-oX", "-", network_arg, NULL);
        free (network_arg);
      }
      else
      {
        // asprintf (&args, "/opt/local/bin/nmap -O -sS -sU -p U:161,623,T:80,311 -oX - %s/%.0f", 
        //asprintf (&args, "nmap -O -sU -p U:161 -oX - %s/%.0f", 
        //  network_str, 32 - (log(ntohl(~maskaddr.s_addr)) / log(2))); 
        char *network_arg;
        asprintf (&network_arg, "%s/%.0f", network_str, 32 - (log(ntohl(~maskaddr.s_addr)) / log(2)));
        num = execlp ("nmap", "nmap", "-O", "-sU", "-p", "U:161", "-oX", "-", network_arg, NULL);
        free (network_arg);
      }
    }
    else
    // { asprintf (&args, "/opt/local/bin/nmap -O -sS -sU -p U:161,623,T:80,311 -oX - %s", network_str); }
    { 
      //asprintf (&args, "nmap -O -sU -p U:161 -oX - %s", network_str); 
      num = execlp ("nmap", "nmap", "-O", "-sU", "-p", "U:161", "-oX", "-", network_str, NULL);
    }

    if (num == -1)
    {
      fprintf (stdout, "execlp error");
    } 
    
    exit (1);
  } 
  
  return proc;
}

int l_netscan_scan_socketcb (i_resource *self, i_socket *sock, void *passdata)
{
  /* Called when there is something to read from the child process.
   *
   * Only return -1 when the socket callback is to be
   * removed.
   * Return 0 to keep the callback active
   */

  int num;
  size_t readcount;
  char *output;
  l_netscan_proc *proc = passdata;

  /* Read response */
  output = (char *) malloc (1024);
  readcount = read (sock->sockfd, output, 1023);
  if (readcount == 1023)
  {
    /* There could be more to be read */
    char temp_buf[1024];
    num = readcount;
    while (num == 1023)
    {
      num = read (sock->sockfd, temp_buf, 1023);
      output = realloc (output, readcount + num + 1);
      if (!output)
      { i_printf (1, "l_netscan_scan_socketcb failed to realloc output to %i bytes", readcount + num + 1); return 0; }
      memcpy (output + readcount, temp_buf, num);
      readcount += num;
    }
  }

  /* NULL Terminate output string */ 
  output[readcount] = '\0';

  i_printf (1, "l_netscan_scan_socketcb received %s", output);

  /* Check result */
  if (readcount < 1)
  { 
    /* Failed to read from child, command must be complete */

    /* Parse XML */
    if (proc->output_str && strlen(proc->output_str) > 0 && strstr(proc->output_str, "<?xml "))
    { l_netscan_parse (self, proc->network_str, proc->mask_str, proc->output_str); }

    /* Run callback */
    if (proc->cb && proc->cb->func)
    { proc->cb->func (self, proc, proc->cb->passdata); }

    /* Cleanup */
    l_netscan_proc_free (proc);
  }
  else
  {
    /* Append output to command */
    if (proc->output_str)
    {
      /* Append */
      int oldsize = strlen (proc->output_str);
      int newsize = oldsize + readcount + 1;
      proc->output_str = realloc (proc->output_str, newsize);
      memcpy (proc->output_str+oldsize, output, readcount);
      proc->output_str[newsize-1] = '\0';
    }
    else
    {
      /* New string */
      proc->output_str = strdup (output);
    }
  }

  /* Cleanup */
  free (output);

  return 0;
}

int l_netscan_scan_timeoutcb (i_resource *self, i_timer *timer, void *passdata)
{
  l_netscan_proc *proc = passdata;

  /* NULL Timer */
  proc->timeout_timer = NULL;

  /* Execute callback for current command */
  if (proc->cb && proc->cb->func)
  { 
    /* Run callback */
    proc->cb->func (self, proc, proc->cb->passdata); 
  }

  /* Terminate the current process */    
  l_netscan_proc_free (proc);

  return -1;    /* Don't keep timer alive */
}


