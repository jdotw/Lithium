#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

#include <induction.h>
#include <induction/callback.h>
#include <induction/socket.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/entity.h>
#include <induction/device.h>

#include "devtest.h"

#define PIPE_TIMEOUT_SECONDS 10
#define PIPE_TIMEOUT_THRESHOLD 1   /* Max consecutive timeouts before restarting ipmitool process */

/* Process Struct Manipulation */

l_devtest_xintel_proc* l_devtest_xintel_proc_create ()
{
  l_devtest_xintel_proc *proc;

  proc = (l_devtest_xintel_proc *) malloc (sizeof(l_devtest_xintel_proc));
  if (!proc)
  { i_printf (1, "l_devtest_xintel_proc_create failed to malloc l_devtest_xintel_proc struct"); return NULL; }
  memset (proc, 0, sizeof(l_devtest_xintel_proc));

  return proc;
}

void l_devtest_xintel_proc_free (void *procptr)
{
  l_devtest_xintel_proc *proc = procptr;

  if (!proc) return;

  if (proc->readfd > 0) close (proc->readfd);
  if (proc->writefd > 0) close (proc->writefd);
  if (proc->sockcb) i_socket_callback_remove (proc->sockcb);
  if (proc->sock) { proc->sock->sockfd = -1; i_socket_free (proc->sock); }
  if (proc->timeout_timer) i_timer_remove (proc->timeout_timer);
  if (proc->curcmd) l_devtest_xintel_cmd_free (proc->curcmd);

  free (proc);
}

/* Command Struct Manipulation */

l_devtest_xintel_cmd* l_devtest_xintel_cmd_create ()
{
  l_devtest_xintel_cmd *cmd;

  cmd = (l_devtest_xintel_cmd *) malloc (sizeof(l_devtest_xintel_cmd));
  if (!cmd)
  { i_printf (1, "l_devtest_xintel_cmd_create failed to malloc l_devtest_xintel_cmd struct"); return NULL; }
  memset (cmd, 0, sizeof(l_devtest_xintel_cmd));

  return cmd;
}

void l_devtest_xintel_cmd_free (void *cmdptr)
{
  l_devtest_xintel_cmd *cmd = cmdptr;

  if (!cmd) return;

  if (cmd->command_str) free (cmd->command_str);
  if (cmd->args_str) free (cmd->args_str);
  if (cmd->output_str) free (cmd->output_str);
  if (cmd->raw_data) free (cmd->raw_data);
  if (cmd->cb) i_callback_free (cmd->cb);

  free (cmd);
}

/* Ipmitool Command Execution */

int l_devtest_xintel (i_resource *self, i_device *dev, int (*cbfunc) (i_resource *self, int result, void *passdata), void *passdata)
{
  l_devtest_xintel_cmd *cmd;

  /* Create cmd */
  cmd = l_devtest_xintel_cmd_create ();
  if (!cmd)
  { i_printf (1, "l_devtest_xintel failed to create cmd struct"); return -1; }
  cmd->command_str = strdup ("chassis");
  cmd->args_str = strdup ("status");

  /* Add the callback */
  if (cbfunc)
  {
    cmd->cb = i_callback_create ();
    if (!cmd->cb)
    { i_printf (1, "l_devtest_xintel failed to create cmd->cb"); l_devtest_xintel_cmd_free (cmd); return -1; }
    cmd->cb->func = cbfunc;
    cmd->cb->passdata = passdata;
  }

  /* No ipmitool process is currently running. Start one */
  l_devtest_xintel_proc *proc = l_devtest_xintel_spawn (self, dev);
  if (!proc)
  {
    i_printf (1, "l_devtest_xintel failed to spawn ipmitool");
    l_devtest_xintel_cmd_free (cmd);
    return -1;
  }
  proc->curcmd = cmd;

  /* Execute the command */
  size_t exec_strlen;
  size_t write_count;
  char *exec_str;

  /* Create exec str */
  if (cmd->args_str)
  { asprintf (&exec_str, "%s %s", cmd->command_str, cmd->args_str); }
  else
  { asprintf (&exec_str, "%s", cmd->command_str); }
  exec_strlen = strlen (exec_str);

  /* Write command to pipe */
  write_count = write (proc->writefd, exec_str, exec_strlen+1);
  close (proc->writefd);
  free (exec_str);
  if (write_count != exec_strlen+1)
  {
    /* Execute callback for current command */
    if (cmd->cb && cmd->cb->func)
    { cmd->cb->func (self, 0, cmd->cb->passdata); }

    l_devtest_xintel_cmd_free (cmd);
    l_devtest_xintel_proc_free (proc);

    return -1;
  }

  /* Reset timeout timer */
  i_timer_reset (proc->timeout_timer);

  return 0;
}

int l_devtest_xintel_socketcb (i_resource *self, i_socket *sock, void *passdata)
{
  /* Called when there is something to read from the 
   * ipmitool child process.
   *
   * Only return -1 when the socket callback is to be
   * removed.
   * Return 0 to keep the callback active
   */

  int num;
  size_t readcount;
  char *output;
  l_devtest_xintel_proc *proc = passdata;

  /* Reset timeout count */
  proc->timeout_count = 0;

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

  /* Append to raw data */
  if (proc->curcmd->raw_data)
  {
    proc->curcmd->raw_data = realloc (proc->curcmd->raw_data, proc->curcmd->raw_datasize + readcount);
  }
  else
  {
    if (readcount > 0)
    { proc->curcmd->raw_data = malloc (readcount); }
  }
  if (proc->curcmd->raw_data)
  {
    memcpy (proc->curcmd->raw_data + proc->curcmd->raw_datasize, output, readcount);
    proc->curcmd->raw_datasize += readcount;
  }

  /* NULL Terminate output string */ 
  output[readcount] = '\0';

  /* Check result */
  if (readcount < 1)
  { 
    /* Failed to read from child, command must be complete
     *
     * Check to see is any ERROR's were reported
     */

    free (output);

    if (!proc->curcmd->output_str || strstr(proc->curcmd->output_str, "ERROR"))
    {
      /* Execute callback for current command */
      if (proc->curcmd->cb && proc->curcmd->cb->func)
      { proc->curcmd->cb->func (self, 0, proc->curcmd->cb->passdata); }
    }
    else
    {
      /* Execute callback for current command */
      if (proc->curcmd->cb && proc->curcmd->cb->func)
      { proc->curcmd->cb->func (self, 1, proc->curcmd->cb->passdata); }
    }

    /* Free current command */
    if (proc->curcmd)
    { l_devtest_xintel_cmd_free (proc->curcmd); proc->curcmd = NULL; }

    /* Terminate IPMI tool */
    proc->sockcb = NULL;
    l_devtest_xintel_proc_free (proc);
    return -1;
  }
  else
  {
    /* Append output to command */
    if (proc->curcmd->output_str)
    {
      /* Append */
      int oldsize = strlen (proc->curcmd->output_str);
      int newsize = oldsize + readcount + 1;
      proc->curcmd->output_str = realloc (proc->curcmd->output_str, newsize);
      memcpy (proc->curcmd->output_str+oldsize, output, readcount);
      proc->curcmd->output_str[newsize-1] = '\0';
    }
    else
    {
      /* New string */
      proc->curcmd->output_str = strdup (output);
    }
    
    /* Cleanup */
    free (output);

    return 0;
  }
}

int l_devtest_xintel_timeoutcb (i_resource *self, i_timer *timer, void *passdata)
{
  /* Called when an ipmitool command times out. The current
   * command is terminated (with the cb called) and hence
   * the current ipmitool process is also terminated.
   *
   * An exec_next call is made to start the next command
   * 
   * Always NULL the timer and return -1;
   */

  l_devtest_xintel_proc *proc = passdata;

  i_printf (1, "l_devtest_xintel_timeoutcb called!");

  /* NULL Timer */
  proc->timeout_timer = NULL;

  /* Execute callback for current command */
  if (proc->curcmd->cb && proc->curcmd->cb->func)
  { proc->curcmd->cb->func (self, 0, proc->curcmd->cb->passdata); }

  /* Free current command */
  if (proc->curcmd)
  { 
    l_devtest_xintel_cmd_free (proc->curcmd); 
    proc->curcmd = NULL; 
  }

  /* Terminate the current process */    
  l_devtest_xintel_proc_free (proc);

  return -1;
}

/* ipmitool process manipulation */

l_devtest_xintel_proc *l_devtest_xintel_spawn (i_resource *self, i_device *dev)
{
  int num;
  int fd1[2];
  int fd2[2];
  l_devtest_xintel_proc *proc;

  proc = l_devtest_xintel_proc_create ();
  if (!proc)
  { i_printf (1, "l_devtest_xintel_spawn failed to create proc struct"); return NULL; }

  num = pipe (fd1);
  if (num < 0)
  { i_printf (1, "i_ipmitool_spawn failed to call pipe for fd1"); l_devtest_xintel_proc_free (proc); return NULL; }
  proc->writefd = fd1[1];

  num = pipe (fd2);
  if (num < 0)
  { i_printf (1, "i_ipmitool_spawn failed to call pipe for fd2"); l_devtest_xintel_proc_free (proc); return NULL; }
  proc->readfd = fd2[0];

  proc->pid = fork ();
  if (proc->pid < 0)
  { i_printf (1, "i_ipmitool_spawn failed to call fork()"); l_devtest_xintel_proc_free (proc); return NULL; }

  if (proc->pid > 0)
  {
    /* Parent Process */

    close(fd1[0]);
    close(fd2[1]);

    /* Create 'socket' struct for the readfd */
    proc->sock = i_socket_create ();
    if (!proc->sock)
    { i_printf (1, "l_devtest_xintel_spawn failed to create blank socket"); return NULL; }
    proc->sock->sockfd = proc->readfd;
    fcntl (proc->readfd, F_SETFL, O_NONBLOCK);

    /* Install readfd socket callback */
    proc->sockcb = i_socket_callback_add (self, SOCKET_CALLBACK_READ, proc->sock, l_devtest_xintel_socketcb, proc);
    if (!proc->sockcb)
    { i_printf (1, "l_devtest_xintel_spawn failed to add socket callback"); return NULL; }

    /* Install timeout */
    proc->timeout_timer = i_timer_add (self, PIPE_TIMEOUT_SECONDS, 0, l_devtest_xintel_timeoutcb, proc);
    if (!proc->timeout_timer)
    { i_printf (1, "l_devtest_xintel_spawn failed to add timeout callback"); return NULL; }
  }
  else
  {
    /* IPMITOOL TOOL CHILD */
    
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

    num = execlp ("lcipmitool", "lcipmitool", "-H", dev->ip_str, "-U", dev->username_str, "-P", dev->password_str, "shell", NULL);
    if (num == -1)
    {
      fprintf (stdout, "execlp error");
    } 
    
    exit (1);
  } 
  
  return proc;
} 


