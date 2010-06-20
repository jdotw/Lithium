#include <stdio.h>
#include <ctype.h>
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
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/dump.h>
#include <lithium/avail.h>

#include "xserve_intel.h"
#include "ipmitool.h"

#define PIPE_TIMEOUT_SECONDS 200
#define PIPE_TIMEOUT_THRESHOLD 1   /* Max consecutive timeouts before restarting ipmitool process */

static v_ipmitool_proc *static_proc = NULL;
static i_list *static_cmdqueue = NULL;

/* Variable retrieval */

i_list* v_ipmitool_proc_cmdqueue ()
{ return static_cmdqueue; }

/* Process Struct Manipulation */

v_ipmitool_proc* v_ipmitool_proc_create ()
{
  v_ipmitool_proc *proc;

  proc = (v_ipmitool_proc *) malloc (sizeof(v_ipmitool_proc));
  if (!proc)
  { i_printf (1, "v_ipmitool_proc_create failed to malloc v_ipmitool_proc struct"); return NULL; }
  memset (proc, 0, sizeof(v_ipmitool_proc));

  return proc;
}

void v_ipmitool_proc_free (void *procptr)
{
  v_ipmitool_proc *proc = procptr;

  if (!proc) return;

  if (proc->readfd > 0) close (proc->readfd);
  if (proc->writefd > 0) close (proc->writefd);
  if (proc->errfd > 0) close (proc->errfd);
  if (proc->sockcb) i_socket_callback_remove (proc->sockcb);
  if (proc->sock) { proc->sock->sockfd = -1; i_socket_free (proc->sock); }
  if (proc->errsockcb) i_socket_callback_remove (proc->errsockcb);
  if (proc->errsock) { proc->errsock->sockfd = -1; i_socket_free (proc->errsock); }
  if (proc->timeout_timer) i_timer_remove (proc->timeout_timer);
  if (proc->curcmd) v_ipmitool_cmd_free (proc->curcmd);

  free (proc);
}

/* Command Struct Manipulation */

v_ipmitool_cmd* v_ipmitool_cmd_create ()
{
  v_ipmitool_cmd *cmd;

  cmd = (v_ipmitool_cmd *) malloc (sizeof(v_ipmitool_cmd));
  if (!cmd)
  { i_printf (1, "v_ipmitool_cmd_create failed to malloc v_ipmitool_cmd struct"); return NULL; }
  memset (cmd, 0, sizeof(v_ipmitool_cmd));

  return cmd;
}

void v_ipmitool_cmd_free (void *cmdptr)
{
  v_ipmitool_cmd *cmd = cmdptr;

  if (!cmd) return;

  if (cmd->command_str) free (cmd->command_str);
  if (cmd->args_str) free (cmd->args_str);
  if (cmd->output_str) free (cmd->output_str);
  if (cmd->raw_data) free (cmd->raw_data);
  if (cmd->cb) i_callback_free (cmd->cb);

  free (cmd);
}

/* Ipmitool Command Execution */

v_ipmitool_cmd* v_ipmitool_exec (i_resource *self, char *command_str, char *args_str, unsigned short flags, int (*cbfunc) (i_resource *self, v_ipmitool_cmd *cmd, int result, void *passdata), void *passdata)
{
  /* Called by external functions to execute an ipmitool
   * command. The command is added to the static_cmdqueue.
   * If no ipmitool (lcipmitool) process is active, one is spawned and 
   * the exec_next func is called to execute the first 
   * queued command. Returns the ipmitool cmd struct.
   */
  int num;
  v_ipmitool_cmd *cmd;

  if (!command_str)
  { i_printf (1, "v_ipmitool_exec called with NULL command"); return NULL; }

  /* Check marking */
  if (self->hierarchy->dev->mark == ENTSTATE_OUTOFSERVICE) return NULL;

  /* Check/create cmdqueue */
  if (!static_cmdqueue)
  { 
    static_cmdqueue = i_list_create (); 
    if (!static_cmdqueue)
    { i_printf (1, "v_ipmitool_exec failed to create static_cmdqueue list"); return NULL; }
  }

  /* Create cmd */
  cmd = v_ipmitool_cmd_create ();
  if (!cmd)
  { i_printf (1, "v_ipmitool_exec failed to create cmd struct"); return NULL; }
  cmd->command_str = strdup (command_str);
  if (args_str) cmd->args_str = strdup (args_str);

  /* Add the callback */
  if (cbfunc)
  {
    cmd->cb = i_callback_create ();
    if (!cmd->cb)
    { i_printf (1, "v_ipmitool_exec failed to create cmd->cb"); v_ipmitool_cmd_free (cmd); return NULL; }
    cmd->cb->func = cbfunc;
    cmd->cb->passdata = passdata;
  }

  /* Enqueue the cmd */
  if (flags & IPMIFLAG_URGENT)
  {
    /* Enqueue to start of command queue (urgent) */
    num = i_list_push (static_cmdqueue, cmd);
  }
  else
  {
    /* Enqueue to end of command queue */
    num = i_list_enqueue (static_cmdqueue, cmd);
  }
  if (num != 0)
  {
    v_ipmitool_cmd_free (cmd);
    if (static_cmdqueue->size == 0)
    { i_list_free (static_cmdqueue); static_cmdqueue = NULL; }
    return NULL;
  }

  /* Check for a current ipmitool process */
  if (!static_proc)
  {
    /* No ipmitool process is currently running. Start one */
    num = v_ipmitool_spawn (self);
    if (num != 0)
    {
      i_printf (1, "v_ipmitool_exec failed to spawn ipmitool");
      num = i_list_search (static_cmdqueue, cmd);
      if (num == 0)
      { i_list_delete (static_cmdqueue); }
      v_ipmitool_cmd_free (cmd);
      if (static_cmdqueue->size == 0)
      { i_list_free (static_cmdqueue); static_cmdqueue = NULL; }
      return NULL;
    }

    /* Execute first command */
    num = v_ipmitool_exec_next (self);
    if (num != 0)
    {
      i_printf (1, "v_ipmitool_exec failed to exec next command");
      v_ipmitool_terminate (self);
      num = i_list_search (static_cmdqueue, cmd);
      if (num == 0)
      { i_list_delete (static_cmdqueue); }
      v_ipmitool_cmd_free (cmd);
      if (static_cmdqueue->size == 0)
      { i_list_free (static_cmdqueue); static_cmdqueue = NULL; }
      return NULL;
    }
  }

  /* Command is now queued, as an ipmiprocess is 
   * already running, it will be executed as the queue
   * is processed
   */

  return cmd;
}

int v_ipmitool_cancel (i_resource *self, v_ipmitool_cmd *cmd)
{
  int num;

  if (static_proc && static_proc->curcmd == cmd)
  {
    /* Command is in-progress, terminate it */
    num = v_ipmitool_terminate (self);
    if (num != 0)
    { i_printf (1, "v_ipmitool_cancel warning, failed to terminate current ipmitool process"); }

    /* Move to next command */
    num = v_ipmitool_exec_next (self);
    if (num != 0)
    {  i_printf (1, "v_ipmitool_cancel failed to restart command execution"); }
  }
  else if (static_cmdqueue && i_list_search (static_cmdqueue, cmd) == 0)
  {
    /* Command is queued */
    i_list_delete (static_cmdqueue);
  }

  return 0;
}

int v_ipmitool_exec_next (i_resource *self)
{
  /* Execute the next queued command */
  int num;
  size_t exec_strlen;
  size_t write_count;
  char *exec_str;
  v_ipmitool_cmd *cmd;

  /* Check for a process */
  if (!static_proc)
  {
    /* No ipmitool process is currently running. Start one */
    num = v_ipmitool_spawn (self);
    if (num != 0)
    {
      i_printf (1, "v_ipmitool_exec_next failed to spawn ipmitool");
      return -1;
    }
  }

  /* Get command line from queue */
  i_list_move_head (static_cmdqueue);
  cmd = i_list_restore (static_cmdqueue);
  if (!cmd)
  {
    /* No more commands terminate the process */
    v_ipmitool_terminate (self);
    return 0;
  }

  /* Transfer cmd pointer to proc struct */
  static_proc->curcmd = cmd;

  /* Remove cmd from the queue */
  i_list_delete (static_cmdqueue);
  if (static_cmdqueue->size == 0)
  { i_list_free (static_cmdqueue); static_cmdqueue = NULL; }

  /* Create exec str */
  if (static_proc->curcmd->args_str)
  { asprintf (&exec_str, "%s %s\n", static_proc->curcmd->command_str, static_proc->curcmd->args_str); }
  else
  { asprintf (&exec_str, "%s\n", static_proc->curcmd->command_str); }
  exec_strlen = strlen (exec_str);

  /* Write command to pipe */
  gettimeofday (&static_proc->curcmd->req_time, NULL);
  write_count = write (static_proc->writefd, exec_str, exec_strlen);
  close (static_proc->writefd);
  static_proc->writefd = 0;
  free (exec_str);
  if (write_count != exec_strlen)
  {
    /* Failed to write command. Exec the cmds
     * callback and then recursively call exec_next
     * to try the next command
     */
    i_printf (1, "v_ipmitool_exec_next failed to send command '%s' to ipmitool process", cmd->command_str);
    
    /* Execute callback for current command */
    if (static_proc->curcmd->cb && static_proc->curcmd->cb->func)
    { static_proc->curcmd->cb->func (self, static_proc->curcmd, IPMIRESULT_FAILED, static_proc->curcmd->cb->passdata); }

    /* Free current command */
    if (static_proc->curcmd)
    { v_ipmitool_cmd_free (static_proc->curcmd); static_proc->curcmd = NULL; }

    return v_ipmitool_exec_next (self);
  }

  /* Reset timeout timer */
  i_timer_reset (static_proc->timeout_timer);

  return 0;
}

int v_ipmitool_exec_socketcb (i_resource *self, i_socket *sock, void *passdata)
{
  /* Called when there is something to read from the 
   * ipmitool child process.
   *
   * Only return -1 when the socket callback is to be
   * removed.
   * Return 0 to keep the callback active
   */

  int num;
  ssize_t readcount;
  char *output;

  /* Reset timeout count */
  static_proc->timeout_count = 0;

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
  if (static_proc->curcmd->raw_data)
  {
    static_proc->curcmd->raw_data = realloc (static_proc->curcmd->raw_data, static_proc->curcmd->raw_datasize + readcount);
  }
  else
  {
    if (readcount > 0)
    { static_proc->curcmd->raw_data = malloc (readcount); }
  }
  if (static_proc->curcmd->raw_data)
  {
    memcpy (static_proc->curcmd->raw_data + static_proc->curcmd->raw_datasize, output, readcount);
    static_proc->curcmd->raw_datasize += readcount;
  }

  /* NULL Terminate output string */ 
  if (readcount >= 0) output[readcount] = '\0';

  /* Check result */
  if (readcount < 1)
  { 
    /* Failed to read from child, command must be complete
     *
     * Check to see is any ERROR's were reported
     */

    gettimeofday (&static_proc->curcmd->resp_time, NULL);
    
    if (!static_proc->curcmd->output_str || strstr(static_proc->curcmd->output_str, "ERROR") || strstr(static_proc->curcmd->output_str, "lose"))
    {
      /* Error occurred */
      l_avail_record_fail (v_osx_ipmi_availobj());

      /* Execute callback for current command */
      if (static_proc->curcmd->cb && static_proc->curcmd->cb->func)
      { static_proc->curcmd->cb->func (self, static_proc->curcmd, IPMIRESULT_FAILED, static_proc->curcmd->cb->passdata); }
    }
    else
    {
      /* Record OK */
      l_avail_record_ok (v_osx_ipmi_availobj(), &static_proc->curcmd->req_time, &static_proc->curcmd->resp_time);

      /* Check format */
      if (strstr(static_proc->curcmd->output_str, "RAW RSP"))
      {

        /* Create new raw data buffer */
        free (static_proc->curcmd->raw_data);
        static_proc->curcmd->raw_data = malloc (strlen(static_proc->curcmd->output_str)+1);
        char *data_buf = (char *) static_proc->curcmd->raw_data;
        memset (static_proc->curcmd->raw_data, 0, strlen(static_proc->curcmd->output_str)+1);
        static_proc->curcmd->raw_datasize = 0;

        /* Find start of raw data */
        char *raw_start = strstr (static_proc->curcmd->output_str, "RAW RSP");
        if (raw_start)
        {
          char *raw_ptr = raw_start;
          size_t i;
          for (i=0; i < strlen(raw_start); i++)
          {
            raw_ptr++;
            if (raw_start[i] == '\n') 
            {
              raw_ptr++;
              break;
            }
          }

          /* Parse raw data */
          for (i=0; i < strlen(raw_ptr); i++)
          {
            if (isalnum(raw_ptr[i]))
            {
              char byte = (char) strtol (raw_ptr+i, NULL, 16);
              data_buf[static_proc->curcmd->raw_datasize] = byte;
              static_proc->curcmd->raw_datasize++;
              i++;
            }
          }
        }
      }

      /* Execute callback for current command */
      if (static_proc->curcmd->cb && static_proc->curcmd->cb->func)
      { static_proc->curcmd->cb->func (self, static_proc->curcmd, IPMIRESULT_OK, static_proc->curcmd->cb->passdata); }
    }

    /* Free current command */
    if (static_proc->curcmd)
    { v_ipmitool_cmd_free (static_proc->curcmd); static_proc->curcmd = NULL; }

    /* Terminate IPMI tool */
    static_proc->sockcb = NULL;
    v_ipmitool_terminate (self);
    
    /* Exec next command */
    v_ipmitool_exec_next (self);
  }
  else
  {
    /* Append output to command */
    if (static_proc->curcmd->output_str)
    {
      /* Append */
      int oldsize = strlen (static_proc->curcmd->output_str);
      int newsize = oldsize + readcount + 1;
      static_proc->curcmd->output_str = realloc (static_proc->curcmd->output_str, newsize);
      memcpy (static_proc->curcmd->output_str+oldsize, output, readcount);
      static_proc->curcmd->output_str[newsize-1] = '\0';
    }
    else
    {
      /* New string */
      static_proc->curcmd->output_str = strdup (output);
    }
  }

  /* Cleanup */
  free (output);

  return 0;
}

int v_ipmitool_exec_errsocketcb (i_resource *self, i_socket *sock, void *passdata)
{
  /* Called when there is something to read from the
   * stderr of the ipmitool child process.
   *
   * Return 0 to keep the callback active
   */

  int num;
  ssize_t readcount;
  char *output;

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

  /* Log the error */
  if (readcount >= 0) 
  {
    output[readcount] = '\0';
    i_printf (2, "IPMI Error: '%s'", output);
  }

  /* Cleanup */
  free (output);

  /* Check for appropriate return */
  if (readcount <= 0) 
  { 
    i_printf (2, "IPMI: Error reading from ipmitool process stderr");
    static_proc->errsockcb = NULL;
    return -1; 
  }

  return 0;
}


int v_ipmitool_exec_timeoutcb (i_resource *self, i_timer *timer, void *passdata)
{
  /* Called when an ipmitool command times out. The current
   * command is terminated (with the cb called) and hence
   * the current ipmitool process is also terminated.
   *
   * An exec_next call is made to start the next command
   * 
   * Always NULL the timer and return -1;
   */

  int num;

  /* NULL Timer */
  static_proc->timeout_timer = NULL;

  /* Record failure */
  l_avail_record_fail (v_osx_ipmi_availobj());

  /* Execute callback for current command */
  if (static_proc->curcmd->cb && static_proc->curcmd->cb->func)
  { static_proc->curcmd->cb->func (self, static_proc->curcmd, IPMIRESULT_TIMEOUT, static_proc->curcmd->cb->passdata); }

  /* Free current command */
  if (static_proc->curcmd)
  { v_ipmitool_cmd_free (static_proc->curcmd); static_proc->curcmd = NULL; }

  i_printf (1, "i_ipmitool_exec_timeoutcb %i ipmitool timeout occurred; restarting ipmitool", PIPE_TIMEOUT_THRESHOLD);

  /* Terminate the current process */    
  num = v_ipmitool_terminate (self);
  if (num != 0)
  { i_printf (1, "i_ipmitool_exec_timeoutcb warning, failed to terminate current ipmitool process"); }

  /* More commands, call exec_next */
  num = v_ipmitool_exec_next (self);
  if (num != 0)
  {  i_printf (1, "i_ipmitool_exec_timeoutcb failed to restart command execution"); }

  return -1;
}

/* ipmitool process manipulation */

int v_ipmitool_spawn (i_resource *self)
{
  int num;
  int fd1[2];
  int fd2[2];
  int fd3[2];
  v_ipmitool_proc *proc;

  proc = v_ipmitool_proc_create ();
  if (!proc)
  { i_printf (1, "v_ipmitool_spawn failed to create proc struct"); return -1; }

  num = pipe (fd1);
  if (num < 0)
  { i_printf (1, "i_ipmitool_spawn failed to call pipe for fd1"); v_ipmitool_proc_free (proc); return -1; }
  proc->writefd = fd1[1];

  num = pipe (fd2);
  if (num < 0)
  { i_printf (1, "i_ipmitool_spawn failed to call pipe for fd2"); v_ipmitool_proc_free (proc); return -1; }
  proc->readfd = fd2[0];

  num = pipe (fd3);
  if (num < 0)
  { i_printf (1, "i_ipmitool_spawn failed to call pipe for fd3"); v_ipmitool_proc_free (proc); return -1; }
  proc->errfd = fd3[0];

  proc->pid = fork ();
  if (proc->pid < 0)
  { i_printf (1, "i_ipmitool_spawn failed to call fork()"); v_ipmitool_proc_free (proc); return -1; }

  if (proc->pid > 0)
  {
    /* Parent Process */
    close(fd1[0]);
    close(fd2[1]);
    close(fd3[1]);

    /* Set static pointer */
    static_proc = proc;

    /* Create 'socket' struct for the readfd */
    proc->sock = i_socket_create ();
    if (!proc->sock)
    { i_printf (1, "v_ipmitool_spawn failed to create blank socket"); v_ipmitool_terminate (self); return -1; }
    proc->sock->sockfd = proc->readfd;
    fcntl (proc->readfd, F_SETFL, O_NONBLOCK);

    /* Install readfd socket callback */
    proc->sockcb = i_socket_callback_add (self, SOCKET_CALLBACK_READ, proc->sock, v_ipmitool_exec_socketcb, NULL);
    if (!proc->sockcb)
    { i_printf (1, "v_ipmitool_spawn failed to add socket callback"); v_ipmitool_terminate (self); return -1; }

    /* Create 'errsock' struct for the errfd */
    proc->errsock = i_socket_create ();
    if (!proc->errsock)
    { i_printf (1, "v_ipmitool_spawn failed to create blank errsocket"); v_ipmitool_terminate (self); return -1; }
    proc->errsock->sockfd = proc->errfd;
    fcntl (proc->errfd, F_SETFL, O_NONBLOCK);

    /* Install errfd socket callback */
    proc->errsockcb = i_socket_callback_add (self, SOCKET_CALLBACK_READ, proc->errsock, v_ipmitool_exec_errsocketcb, NULL);
    if (!proc->errsockcb)
    { i_printf (1, "v_ipmitool_spawn failed to add errsocket callback"); v_ipmitool_terminate (self); return -1; }

    /* Install timeout */
    proc->timeout_timer = i_timer_add (self, PIPE_TIMEOUT_SECONDS, 0, v_ipmitool_exec_timeoutcb, NULL);
    if (!proc->timeout_timer)
    { i_printf (1, "v_ipmitool_spawn failed to add timeout callback"); v_ipmitool_terminate (self); return -1; }
  }
  else
  {
    /* IPMITOOL TOOL CHILD */
    
    free (proc);
    close(fd1[1]);
    close(fd2[0]);
    close(fd3[0]);
    
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
    if (fd3[1] != STDERR_FILENO)
    {
      if (dup2(fd3[1], STDERR_FILENO) != STDERR_FILENO)
        fprintf (stdout, "dup2 error to stderr");
      close(fd3[1]);
    } 

    char *lom_ip_str;
    if (self->hierarchy->dev->lom_ip_str && strlen(self->hierarchy->dev->lom_ip_str) > 0)
    { lom_ip_str = self->hierarchy->dev->lom_ip_str; }
    else
    { lom_ip_str = self->hierarchy->dev->ip_str; }

    char *lom_username_str;
    if (self->hierarchy->dev->lom_username_str && strlen(self->hierarchy->dev->lom_username_str) > 0)
    { lom_username_str = self->hierarchy->dev->lom_username_str; }
    else
    { lom_username_str = self->hierarchy->dev->username_str; }

    char *lom_password_str;
    if (self->hierarchy->dev->lom_password_str && strlen(self->hierarchy->dev->lom_password_str) > 0)
    { lom_password_str = self->hierarchy->dev->lom_password_str; }
    else
    { lom_password_str = self->hierarchy->dev->password_str; }

    if (!lom_ip_str || strlen(lom_ip_str) < 1 || strcmp(lom_ip_str, "127.0.0.1")==0 || strcmp(lom_ip_str, "local")==0 || strcmp(lom_ip_str, "localhost")==0)
    { num = execlp ("lcipmitool", "lcipmitool", "-c", "-U", lom_username_str, "-P", lom_password_str, "shell", NULL); }
    else
    { num = execlp ("lcipmitool", "lcipmitool", "-c", "-H", lom_ip_str, "-U", lom_username_str, "-P", lom_password_str, "shell", NULL); }
    if (num == -1)
    {
      fprintf (stdout, "execlp error");
    } 
    
    exit (1);
  } 
  
  return 0;
} 

int v_ipmitool_terminate (i_resource *self)
{
  if (static_proc)
  { v_ipmitool_proc_free (static_proc); static_proc = NULL; }
  
  return 0;
}


