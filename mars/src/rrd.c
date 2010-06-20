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
#include <induction/timer.h>
#include <induction/list.h>
#include <induction/rrdtool.h>

#include "rrd.h"

static i_list *static_proc_list = NULL;
static i_list *static_freeproc_list = NULL;
static i_list *static_cmdqueue = NULL;
static int static_q_drops = 0;

/* Pointer retrieval */

i_list* m_rrd_proclist ()
{ return static_proc_list; }

i_list* m_rrd_freeproclist ()
{ return static_freeproc_list; }

i_list* m_rrd_cmdqueue ()
{ return static_cmdqueue; }

int m_rrd_q_drops ()
{ return static_q_drops; }

void m_rrd_q_drops_reset ()
{ static_q_drops = 0; }

/* Initialise */

int m_rrd_init (i_resource *self)
{
  int i; 
  
  /* Create lists */
  static_proc_list = i_list_create ();
  static_freeproc_list = i_list_create ();
  static_cmdqueue = i_list_create ();

  /* Spawn rrdtool processes */
  for (i=0; i < RRD_PROC_COUNT; i++)
  {
    m_rrd_proc *proc;

    /* Create proc */
    proc = m_rrd_proc_spawn (self);
    if (!proc)
    { i_printf (1, "m_rrd_init failed to spawn proc %i", i); continue; }
  }

  return 0;
}

/* Exec */

int m_rrd_exec (i_resource *self, int priority, char *cmd_str)
{
  /* Perform priority-based queuing 
   *
   * 0 = Must be executed immediately (create, etc)
   * 1 = Should be given priority (queued to a depth)
   * 2 = Can be dropped immediately if queue present
   */

  i_list *cmdqueue = m_rrd_cmdqueue ();
  if (priority == 0)
  {
    /* Command must be executed */
    i_list_push (cmdqueue, strdup(cmd_str));
  }
  else if (priority == 1)
  {
    /* Command can be queued */
    if (cmdqueue->size < MAX_P1_Q_LENGTH)
    {
      /* Enqueue command */
      i_list_enqueue (cmdqueue, strdup(cmd_str)); 
    }
    else
    {
      /* Drop the update */
      static_q_drops++;
      return 0;
    }
  }
  else
  {
    /* Command should be dropped if (small) queue present */
    if (cmdqueue->size < MAX_P2_Q_LENGTH)
    {
      /* Enqueue command */
      i_list_enqueue (cmdqueue, strdup(cmd_str)); 
    }
    else
    {
      /* Drop the update */
      static_q_drops++;
      return 0;
    }
  }

  /* Call the queue runner */
  m_rrd_queuerunner (self);

  return 0;
}

/* Que Runner */

int m_rrd_queuerunner (i_resource *self)
{
  /* The queue runner is called when a situation arises 
   * where it is possible that command execution should 
   * occur. I.e there is a new command in the queue or a 
   * process is free to run an existing command
   */
  
  m_rrd_proc *proc;
  i_list *cmdqueue;
  i_list *proc_list;
  i_list *freeproc_list;

  /* Check process count */
  proc_list = m_rrd_proclist ();
  if (proc_list->size < RRD_PROC_COUNT)
  {
    unsigned long i;

    /* Proc count too low, spawn more */
    i_printf (1, "m_rrd_queuerunner proc count too low, spawning %i processes", RRD_PROC_COUNT - proc_list->size);
    for (i=0; i < (RRD_PROC_COUNT - proc_list->size); i++)
    { 
      proc = m_rrd_proc_spawn (self); 
      if (!proc)
      { i_printf (1, "m_rrd_queuerunner failed to spawn rrd process"); continue; }
    }
  }

  /* Retrieve/Check Command Queue */
  cmdqueue = m_rrd_cmdqueue ();
  if (cmdqueue->size < 1) return 0;

  /* Check for free process */
  freeproc_list = m_rrd_freeproclist ();
  for (i_list_move_head(freeproc_list); (proc=i_list_restore(freeproc_list))!=NULL; i_list_move_next(freeproc_list))
  {
    /* Execute next command */
    int num;
    int cmd_strlen;
    char *cmd_str;

    /* Retrieve cmd */
    i_list_move_head (cmdqueue); 
    cmd_str = i_list_restore (cmdqueue); 
    i_list_delete (cmdqueue);

    /* Execute command */
    cmd_strlen = strlen (cmd_str);
    num = write (proc->writefd, cmd_str, cmd_strlen);
    if (num != cmd_strlen)
    { i_printf (0, "m_rrd_queuerunner failed to write entire command '%s' (length %i) to process. wrote %i bytes", cmd_str, cmd_strlen, num); }
    free (cmd_str);
    if (num == cmd_strlen)
    {
      /* Command running, remove process from freelist */
      i_list_delete (freeproc_list);
    }
    else
    { 
      /* Failed to execute command, kill process */
      m_rrd_proc_kill (self, proc);
    }

    /* Check cmdqueue size */
    if (cmdqueue->size < 1)
    { break; }
  }

  return 0;
}
