#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#include "induction.h"
#include "callback.h"
#include "socket.h"
#include "list.h"
#include "timer.h"
#include "rrdtool.h"

#define PIPE_TIMEOUT_SECONDS 120
#define PIPE_TIMEOUT_THRESHOLD 3   /* Max consecutive timeouts before restarting rrdtool process */

static i_rrdtool_proc *static_proc = NULL;
static i_list *static_cmdqueue = NULL;

/* Variable retrieval */

i_list* i_rrdtool_proc_cmdqueue ()
{ return static_cmdqueue; }

/* Process Struct Manipulation */

i_rrdtool_proc* i_rrdtool_proc_create ()
{
  i_rrdtool_proc *proc;

  proc = (i_rrdtool_proc *) malloc (sizeof(i_rrdtool_proc));
  if (!proc)
  { i_printf (1, "i_rrdtool_proc_create failed to malloc i_rrdtool_proc struct"); return NULL; }
  memset (proc, 0, sizeof(i_rrdtool_proc));

  return proc;
}

void i_rrdtool_proc_free (void *procptr)
{
  i_rrdtool_proc *proc = procptr;

  if (!proc) return;

  if (proc->readfd > 0) close (proc->readfd);
  if (proc->writefd > 0) close (proc->writefd);
  if (proc->sockcb) i_socket_callback_remove (proc->sockcb);
  if (proc->sock) { proc->sock->sockfd = -1; i_socket_free (proc->sock); }
  if (proc->timeout_timer) i_timer_remove (proc->timeout_timer);
  if (proc->curcmd) i_rrdtool_cmd_free (proc->curcmd);

  free (proc);
}

/* Command Struct Manipulation */

i_rrdtool_cmd* i_rrdtool_cmd_create ()
{
  i_rrdtool_cmd *cmd;

  cmd = (i_rrdtool_cmd *) malloc (sizeof(i_rrdtool_cmd));
  if (!cmd)
  { i_printf (1, "i_rrdtool_cmd_create failed to malloc i_rrdtool_cmd struct"); return NULL; }
  memset (cmd, 0, sizeof(i_rrdtool_cmd));

  return cmd;
}

void i_rrdtool_cmd_free (void *cmdptr)
{
  i_rrdtool_cmd *cmd = cmdptr;

  if (!cmd) return;

  if (cmd->fullpath_str) free (cmd->fullpath_str);
  if (cmd->command_str) free (cmd->command_str);
  if (cmd->output_str) free (cmd->output_str);
  if (cmd->cb) i_callback_free (cmd->cb);

  free (cmd);
}

/* RRDtool Command Execution */

i_rrdtool_cmd* i_rrdtool_exec (i_resource *self, char *fullpath_str, char *command_str, unsigned short flags, int (*cbfunc) (i_resource *self, i_rrdtool_cmd *cmd, int result, void *passdata), void *passdata)
{
  /* Called by external functions to execute an rrdtool
   * command. The command is added to the static_cmdqueue.
   * If no RRDtool process is active, one is spawned and 
   * the exec_next func is called to execute the first 
   * queued command. Returns the rrdtool cmd struct.
   */
  int num;
  i_rrdtool_cmd *cmd;

  if (!command_str)
  { i_printf (1, "i_rrdtool_exec called with NULL command"); return NULL; }

  /* Check/create cmdqueue */
  if (!static_cmdqueue)
  { 
    static_cmdqueue = i_list_create (); 
    if (!static_cmdqueue)
    { i_printf (1, "i_rrdtool_exec failed to create static_cmdqueue list"); return NULL; }
  }

  /* Create cmd */
  cmd = i_rrdtool_cmd_create ();
  if (!cmd)
  { i_printf (1, "i_rrdtool_exec failed to create cmd struct"); return NULL; }
  cmd->fullpath_str = strdup (fullpath_str);
  cmd->command_str = strdup (command_str);

  /* Add the callback */
  if (cbfunc)
  {
    cmd->cb = i_callback_create ();
    if (!cmd->cb)
    { i_printf (1, "i_rrdtool_exec failed to create cmd->cb"); i_rrdtool_cmd_free (cmd); return NULL; }
    cmd->cb->func = cbfunc;
    cmd->cb->passdata = passdata;
  }

  /* Enqueue the cmd */
  if (flags & RRDFLAG_URGENT)
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
    i_rrdtool_cmd_free (cmd);
    if (static_cmdqueue->size == 0)
    { i_list_free (static_cmdqueue); static_cmdqueue = NULL; }
    return NULL;
  }

  /* Check for a current rrdtool process */
  if (!static_proc)
  {
    /* No rrdtool process is currently running. Start one */
    num = i_rrdtool_spawn (self);
    if (num != 0)
    {
      i_printf (1, "i_rrdtool_exec failed to spawn rrdtool");
      num = i_list_search (static_cmdqueue, cmd);
      if (num == 0)
      { i_list_delete (static_cmdqueue); }
      i_rrdtool_cmd_free (cmd);
      if (static_cmdqueue->size == 0)
      { i_list_free (static_cmdqueue); static_cmdqueue = NULL; }
      return NULL;
    }

    /* Execute first command */
    num = i_rrdtool_exec_next (self);
    if (num != 0)
    {
      i_printf (1, "i_rrdtool_exec failed to exec next command");
      i_rrdtool_terminate (self);
      num = i_list_search (static_cmdqueue, cmd);
      if (num == 0)
      { i_list_delete (static_cmdqueue); }
      i_rrdtool_cmd_free (cmd);
      if (static_cmdqueue->size == 0)
      { i_list_free (static_cmdqueue); static_cmdqueue = NULL; }
      return NULL;
    }
  }

  /* Command is now queued, as an rrdprocess is 
   * already running, it will be executed as the queue
   * is processed
   */

  return cmd;
}

int i_rrdtool_exec_next (i_resource *self)
{
  /* Execute the next queued command */

  size_t exec_strlen;
  size_t write_count;
  char *exec_str;
  i_rrdtool_cmd *cmd;

  /* Get command line from queue */
  i_list_move_head (static_cmdqueue);
  cmd = i_list_restore (static_cmdqueue);
  if (!cmd)
  {
    /* No more commands terminate the process */
    i_rrdtool_terminate (self);
    return 0;
  }

  /* Transfer cmd pointer to proc struct */
  static_proc->curcmd = cmd;

  /* Remove cmd from the queue */
  i_list_delete (static_cmdqueue);
  if (static_cmdqueue->size == 0)
  { i_list_free (static_cmdqueue); static_cmdqueue = NULL; }

  /* Create exec str */
  asprintf (&exec_str, "%s\n", static_proc->curcmd->command_str);
  exec_strlen = strlen (exec_str);

  /* Write command to pipe */
  write_count = write (static_proc->writefd, exec_str, exec_strlen);
  free (exec_str);
  if (write_count != exec_strlen)
  {
    /* Failed to write command. Exec the cmds
     * callback and then recursively call exec_next
     * to try the next command
     */
    i_printf (1, "i_rrdtool_exec_next failed to send command '%s' to rrdtool process", cmd->command_str);
    
    /* Execute callback for current command */
    if (static_proc->curcmd->cb && static_proc->curcmd->cb->func)
    { static_proc->curcmd->cb->func (self, static_proc->curcmd, RRDRESULT_FAILED, static_proc->curcmd->cb->passdata); }

    /* Free current command */
    if (static_proc->curcmd)
    { i_rrdtool_cmd_free (static_proc->curcmd); static_proc->curcmd = NULL; }

    return i_rrdtool_exec_next (self);
  }

  /* Reset timeout timer */
  i_timer_reset (static_proc->timeout_timer);

  return 0;
}

int i_rrdtool_exec_socketcb (i_resource *self, i_socket *sock, void *passdata)
{
  /* Called when there is something to read from the 
   * rrdtool child process.
   *
   * Only return -1 when the socket callback is to be
   * removed.
   * Return 0 to keep the callback active
   */

  int num;
  int result;
  size_t readcount;
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
  output[readcount] = '\0';

  /* Check result */
  if (readcount < 1)
  { 
    /* Failed to read from child, attempt to
     * restart the rrdtool process and set 
     * the error code
     */
    i_rrdtool_cmd *curcmd;

    i_printf (1, "i_rrd_exec_socketcb failed to read data from rrdtool child process (%s), restarting rrdtool and moving to next command", strerror(errno));

    /* Clear output buff */
    free (output);
    output = NULL;

    /* Set result code */
    result = RRDRESULT_FAILED;

    /* Retrieve curcmd so it isnt freed */
    curcmd = static_proc->curcmd;
    static_proc->curcmd = NULL;

    /* Restart RRDtool */
    static_proc->sockcb = NULL;
    i_rrdtool_terminate (self);
    num = i_rrdtool_spawn (self);
    if (num != 0)
    { 
      /* Exec callback, free the command
       * and free the command queue
       */
      i_printf (1, "i_rrd_exec_socketcb failed to spawn replacement rrdtool"); 
      if (curcmd->cb && curcmd->cb->func)
      { curcmd->cb->func (self, curcmd, RRDRESULT_FAILED, curcmd->cb->passdata); }
      if (curcmd)
      { i_rrdtool_cmd_free (curcmd); }
      i_list_free (static_cmdqueue);
      static_cmdqueue = NULL;
      return -1;
    }

    /* Restore curcmd to new rrdtool proc */
    static_proc->curcmd = curcmd;
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
    free (output);
    output = NULL;

    /* Check to see if the output is complete */
    if (strstr(static_proc->curcmd->output_str, "OK") || strstr(static_proc->curcmd->output_str, "ERROR"))
    { 
      /* Command is complete */
      result = RRDRESULT_OK;

      /* NULL-Terminate the output string BEFORE
       * The OK. This is necessary because if we're
       * doing an RRDtool DUMP we DO NOT want the
       * OK string at the end of the output
       */
      char *okptr = strstr(static_proc->curcmd->output_str, "OK");
      if (okptr)
      { *okptr = '\0'; }
    }
    else
    { 
      /* Command is not yet complete, the OK has not been received */
      return 0;
    }
  }

  /* Execute callback for current command */
  if (static_proc->curcmd->cb && static_proc->curcmd->cb->func)
  { static_proc->curcmd->cb->func (self, static_proc->curcmd, result, static_proc->curcmd->cb->passdata); }

  /* Free current command */
  if (static_proc->curcmd)
  { i_rrdtool_cmd_free (static_proc->curcmd); static_proc->curcmd = NULL; }

  /* Exec next command */
  i_rrdtool_exec_next (self);

  return 0;
}

int i_rrdtool_exec_timeoutcb (i_resource *self, i_timer *timer, void *passdata)
{
  /* Called when an rrdtool command times out. The current
   * command is terminated (with the cb called) and then 
   * then timeout_count check is called. If the timeout_count
   * is >= the THRESHOLD then the current rrdtool is 
   * terminated and a new one is spawned. Then, regardless
   * of whether the THRESHOLD was reached or not, the next command is
   * executed.
   * 
   * Only return -1 when the timer is to be deactivated
   * Return 0 to keep the timer active
   */

  int num;

  /* Execute callback for current command */
  if (static_proc->curcmd->cb && static_proc->curcmd->cb->func)
  { static_proc->curcmd->cb->func (self, static_proc->curcmd, RRDRESULT_TIMEOUT, static_proc->curcmd->cb->passdata); }

  /* Free current command */
  if (static_proc->curcmd)
  { i_rrdtool_cmd_free (static_proc->curcmd); static_proc->curcmd = NULL; }

  /* Increment and check timeout count */
  static_proc->timeout_count++;
  if (static_proc->timeout_count == PIPE_TIMEOUT_THRESHOLD)
  {
    /* Maximum timeout count has been reached
     *
     * Terminate the whole monitoring process.
     *
     * The whole process (i.e core) is terminated because
     * at this point we can't determine whether or not the PID
     * we have the for RRDtool process is still ours and we don't 
     * want to go killing the wrong process. Hence we're in an
     * error/inconsistent condition and monitoring ought to be restarted
     */

    i_printf (1, "i_rrd_exec_timeoutcb %i successive rrdtool timeouts have occurred; restarting monitoring process", PIPE_TIMEOUT_THRESHOLD);
    exit (0);

    /* Terminate the current process */    
//    num = i_rrdtool_terminate (self);
//    if (num != 0)
//    { i_printf (1, "i_rrd_exec_timeoutcb warning, failed to terminate current rrdtool process"); }

    /* Check if there are more commands to run.
     * If there are more commands, spawn a new 
     * rrdtool process
     */
//    if (static_cmdqueue && static_cmdqueue->size > 0)
//    {
//      /* More commands to run, restart and exec_next */
//      num = i_rrdtool_spawn (self);
//      if (num != 0)
//      {  
//        i_printf (1, "i_rrd_exec_timeoutcb failed to spawn a new rrdtool process");
//        if (static_cmdqueue)
//        { i_list_free (static_cmdqueue); static_cmdqueue = NULL; }
//        return -1;
//      }
//    }
  }

  /* Check if there are more commands to run.
   * If there are more command, call exec_next
   * If there are no more commands, free the
   * command queue.
   */
  if (static_cmdqueue && static_cmdqueue->size > 0)
  {
    /* More commands, call exec_next */
    num = i_rrdtool_exec_next (self);
    if (num != 0)
    { 
      i_printf (1, "i_rrd_exec_timeoutcb failed to restart command execution"); 
      i_rrdtool_terminate (self);
      if (static_cmdqueue)
      { i_list_free (static_cmdqueue); static_cmdqueue = NULL; }
      return -1; 
    }
  }
  else
  {
    /* No more commands, cleanup and finish */
    i_list_free (static_cmdqueue);
    static_cmdqueue = NULL;
    return -1;
  }

  return 0;
}

/* RRDtool process manipulation */

int i_rrdtool_spawn (i_resource *self)
{
  int num;
  int fd1[2];
  int fd2[2];
  i_rrdtool_proc *proc;

  proc = i_rrdtool_proc_create ();
  if (!proc)
  { i_printf (1, "i_rrdtool_exec failed to create proc struct"); return -1; }

  num = pipe (fd1);
  if (num < 0)
  { i_printf (1, "i_rrd_exec failed to call pipe for fd1"); i_rrdtool_proc_free (proc); return -1; }
  proc->writefd = fd1[1];

  num = pipe (fd2);
  if (num < 0)
  { i_printf (1, "i_rrd_exec failed to call pipe for fd2"); i_rrdtool_proc_free (proc); return -1; }
  proc->readfd = fd2[0];

  proc->pid = fork ();
  if (proc->pid < 0)
  { i_printf (1, "i_rrd_exec failed to call fork()"); i_rrdtool_proc_free (proc); return -1; }

  if (proc->pid > 0)
  {
    /* Parent Process
     *
     * - Send first queued command
     */

    close(fd1[0]);
    close(fd2[1]);

    /* Set static pointer */
    static_proc = proc;

    /* Create 'socket' struct for the readfd */
    proc->sock = i_socket_create ();
    if (!proc->sock)
    { i_printf (1, "i_rrdtool_spawn failed to create blank socket"); i_rrdtool_terminate (self); return -1; }
    proc->sock->sockfd = proc->readfd;
    fcntl (proc->readfd, F_SETFL, O_NONBLOCK);

    /* Install readfd socket callback */
    proc->sockcb = i_socket_callback_add (self, SOCKET_CALLBACK_READ, proc->sock, i_rrdtool_exec_socketcb, NULL);
    if (!proc->sockcb)
    { i_printf (1, "i_rrdtool_spawn failed to add socket callback"); i_rrdtool_terminate (self); return -1; }

    /* Install timeout */
    proc->timeout_timer = i_timer_add (self, PIPE_TIMEOUT_SECONDS, 0, i_rrdtool_exec_timeoutcb, NULL);
    if (!proc->timeout_timer)
    { i_printf (1, "i_rrdtool_spawn failed to add timeout callback"); i_rrdtool_terminate (self); return -1; }

  }
  else
  {
    /* RRD TOOL CHILD */
    
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
    
    num = execlp ("lcrrdtool", "lcrrdtool", "-", NULL);
    if (num == -1)
    {
      fprintf (stdout, "execlp error");
    } 
    
    exit (1);
  } 
  
  return 0;
} 

int i_rrdtool_terminate (i_resource *self)
{
  if (static_proc)
  { i_rrdtool_proc_free (static_proc); static_proc = NULL; }
  
  return 0;
}


