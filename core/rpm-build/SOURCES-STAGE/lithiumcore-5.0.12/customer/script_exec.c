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

#include "script.h"

#define PIPE_TIMEOUT_SECONDS 300

/* Script Handling/Maitenance */

l_script_proc* l_script_proc_create ()
{
  l_script_proc *proc;

  proc = (l_script_proc *) malloc (sizeof(l_script_proc));
  if (!proc)
  { i_printf (1, "l_script_proc_create failed to malloc l_script_proc struct"); return NULL; }
  memset (proc, 0, sizeof(l_script_proc));

  return proc;
}

void l_script_proc_free (void *procptr)
{
  l_script_proc *proc = procptr;

  if (!proc) return;

  if (proc->pid > 0) kill (proc->pid, SIGKILL);
  if (proc->readfd > 0) close (proc->readfd);
  if (proc->writefd > 0) close (proc->writefd);
  if (proc->sockcb) i_socket_callback_remove (proc->sockcb);
  if (proc->sock) { proc->sock->sockfd = -1; i_socket_free (proc->sock); }
  if (proc->timeout_timer) i_timer_remove (proc->timeout_timer);
  if (proc->script_file) free (proc->script_file);
  if (proc->output_str) free (proc->output_str);
  if (proc->command_str) free (proc->command_str);
  if (proc->cb) i_callback_free (proc->cb);

  free (proc);
}

/* Script Execution */

l_script_proc* l_script_exec (i_resource *self, char *script_file, char *command_str, int (*cbfunc) (), void *passdata)
{
  l_script_proc *proc;

  /* Create process struct */
  proc = l_script_proc_create ();
  if (!proc)
  { i_printf (1, "l_script_exec failed to create proc struct"); return NULL; }
  proc->script_file = strdup (script_file);
  proc->command_str = strdup (command_str);

  /* Add callback if specified */
  if (cbfunc)
  {
    proc->cb = i_callback_create ();
    if (!proc->cb)
    { i_printf (1, "l_script_exec failed to create proc->cb"); l_script_proc_free (proc); return NULL; }
    proc->cb->func = cbfunc;
    proc->cb->passdata = passdata;
  }

  /* Create pipes */
  int num;
  int fd1[2];
  int fd2[2];
  num = pipe (fd1);
  if (num < 0)
  { i_printf (1, "l_script_exec failed to call pipe for fd1"); l_script_proc_free (proc); return NULL; }
  proc->writefd = fd1[1];
  num = pipe (fd2);
  if (num < 0)
  { i_printf (1, "l_script_exec failed to call pipe for fd2"); l_script_proc_free (proc); return NULL; }
  proc->readfd = fd2[0];

  /* Fork */
  proc->pid = fork ();
  if (proc->pid < 0)
  { i_printf (1, "l_script_exec failed to call fork()"); l_script_proc_free (proc); return NULL; }

  if (proc->pid > 0)
  {
    /* Parent Process */
    close(fd1[0]);
    close(fd2[1]);

    /* Create 'socket' struct for the readfd */
    proc->sock = i_socket_create ();
    if (!proc->sock)
    { i_printf (1, "l_script_exec failed to create blank socket"); l_script_proc_free (proc); return NULL; }
    proc->sock->sockfd = proc->readfd;
    fcntl (proc->readfd, F_SETFL, O_NONBLOCK);

    /* Install readfd socket callback */
    proc->sockcb = i_socket_callback_add (self, SOCKET_CALLBACK_READ, proc->sock, l_script_exec_socketcb, proc);
    if (!proc->sockcb)
    { i_printf (1, "l_script_exec failed to add socket callback"); l_script_proc_free (proc); return NULL; }

    /* Install timeout */
    proc->timeout_timer = i_timer_add (self, PIPE_TIMEOUT_SECONDS, 0, l_script_exec_timeoutcb, proc);
    if (!proc->timeout_timer)
    { i_printf (1, "l_script_exec failed to add timeout callback"); l_script_proc_free (proc); return NULL; }
  }
  else
  {
    /* Action Child */
    free (proc);
    close(fd1[1]);
    close(fd2[0]);

    signal (SIGPIPE, SIG_DFL);
    
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
    char *args;
    asprintf (&args, "yes '' | %s %s | sed 's/$/\\&\\#xD\\;\\&\\#xA\\;/g' 2>&1", proc->script_file, proc->command_str);
    num = execlp ("sh", "sh", "-c", args, NULL); 
    if (num == -1)
    {
      fprintf (stdout, "execlp error");
    } 
   //system (args);
    
    exit (1);
  } 
  
  return proc;
}

int l_script_exec_socketcb (i_resource *self, i_socket *sock, void *passdata)
{
  /* Called when there is something to read from the 
   * script child process.
   *
   * Only return -1 when the socket callback is to be
   * removed.
   * Return 0 to keep the callback active
   */

  int num;
  size_t readcount;
  char *output;
  l_script_proc *proc = passdata;

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
      memcpy (output + readcount, temp_buf, num);
      readcount += num;
    }
  }

  /* NULL Terminate output string */ 
  output[readcount] = '\0';

  i_printf (1, "l_script_exec_socketcb output=%s", output);
  
  /* Check result */
  if (readcount < 1)
  { 
    /* Failed to read from child, command must be complete
     *
     * Check to see is any ERROR's were reported
     */

    /* Run callback */
    if (proc->cb && proc->cb->func)
    { proc->cb->func (self, proc, proc->cb->passdata); }
    
    /* Exec next command */
    l_script_proc_free (proc);
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

int l_script_exec_timeoutcb (i_resource *self, i_timer *timer, void *passdata)
{
  l_script_proc *proc = passdata;

  i_printf (1, "l_script_exec_timeoutcb timeout occurred");
  
  /* NULL Timer */
  proc->timeout_timer = NULL;

  /* Execute callback for current command */
  if (proc->cb && proc->cb->func)
  { proc->cb->func (self, proc, proc->cb->passdata); }

  /* Terminate the current process */    
  l_script_proc_free (proc);

  return -1;    /* Don't keep timer alive */
}


