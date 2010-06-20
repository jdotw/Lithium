#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/auth.h>
#include <induction/socket.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <lithium/snmp.h>

#include "envmon.h"

/* NetBotz Environmental Monitoring Sub-System */

/* Camera picture refresh */

typedef struct v_envmon_wget_proc_s
{
  int writefd;
  int readfd;
  pid_t pid;

  struct i_socket_s *sock;
  struct i_socket_callback_s *sockcb;

  unsigned short got_result;
} v_envmon_wget_proc;

int v_envmon_campic_refresh (i_resource *self, i_metric *met, int opcode)
{
  /* This function refreshes/fetches the campic from the netbotz unit */
  int num;
  int fd1[2];
  int fd2[2];
  char *fsroot;
  i_metric_value *val;
  v_envmon_wget_proc *wget = (v_envmon_wget_proc *) met->refresh_data;
  v_envmon_item *env = (v_envmon_item *) met->obj->itemptr;
  
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:

      /* Create pipes */
      num = pipe (fd1);
      if (num < 0)
      { i_printf (1, "v_envmon_campic_refresh failed to call pipe for fd1"); return -1; }
      num = pipe (fd2);
      if (num < 0)
      { i_printf (1, "v_envmon_campic_refresh failed to call pipe for fd2"); return -1; }
      
      /* Create process struct */
      wget = (v_envmon_wget_proc *) malloc (sizeof(v_envmon_wget_proc));
      met->refresh_data = wget;
      memset (wget, 0, sizeof(v_envmon_wget_proc));
      wget->writefd = fd1[1];
      wget->readfd = fd2[0];

      /* Get fsroot */
      fsroot = i_entity_path (self, ENTITY(met), 0, ENTPATH_ROOT);

      /* Fork process */
      wget->pid = fork ();
      if (wget->pid > 0)
      {
        /* PARENT PROCESS */
        free (fsroot);
        close (fd1[0]);
        close (fd2[1]);

        /* Create socket and add callback */
        wget->sock = i_socket_create ();
        wget->sock->sockfd = wget->readfd;
        fcntl (wget->readfd, F_SETFL, O_NONBLOCK);
        wget->sockcb = i_socket_callback_add (self, SOCKET_CALLBACK_READ, wget->sock, v_envmon_campic_sockcb, met);
      }
      else
      {
        /* CHILD PROCESS */
        char *wget_str;

        /* Close unused sockets */
        free (wget);
        close (fd1[1]);
        close (fd2[0]);

        /* Create command */
        if (met == env->campic_small)
        { 
          asprintf (&wget_str, "wget -q -t 1 -O %s/campic.jpg http://netbotzmonitor:netbotzmonitor@%s/images/nbEyeEnc_0_CAMERA/QVGA/campic.jpg",
            fsroot, self->hierarchy->dev->ip_str); 
        }
        else
        { 
          asprintf (&wget_str, "wget -q -t 1 -O %s/campic.jpg http://netbotzmonitor:netbotzmonitor@%s/images/nbEyeEnc_0_CAMERA/VGA/campic.jpg",
            fsroot, self->hierarchy->dev->ip_str); 
        }
        free (fsroot);

        /* Execute command */
        num = system (wget_str);
        free (wget_str);

        /* Send result */
        write (fd2[1], &num, sizeof(int));

        /* Exit */
        exit (0);
      }
      
      break;

    case REFOP_COLLISION:
      break;

    case REFOP_TERMINATE:
      val = i_metric_value_create ();
      if (wget)
      {
        /* Check/set refresh result and metric value */
        if (wget->got_result == 1)
        { val->integer = 1; }
        else
        { val->integer = 0;  }
        met->refresh_result = REFRESULT_OK; 

        /* Free wget struct */
        if (wget->readfd > 0) close (wget->readfd);
        if (wget->writefd > 0) close (wget->writefd);
        if (wget->sockcb) i_socket_callback_remove (wget->sockcb);
        if (wget->sock) { wget->sock->sockfd = -1; i_socket_free (wget->sock); }
        free (wget);
        met->refresh_data = NULL;
      }
      else
      { 
        /* No refresh data present */
        met->refresh_result = REFRESULT_TOTAL_FAIL; 
        val->integer = 0; 
      }
      i_metric_value_enqueue (self, met, val);
      break;

    case REFOP_CLEANDATA:
      break;
  }

  return 0;
}

int v_envmon_campic_sockcb (i_resource *self, i_socket *sock, void *passdata)
{
  int num;
  int retval;
  i_metric *met = (i_metric *) passdata;
  v_envmon_wget_proc *wget = (v_envmon_wget_proc *) met->refresh_data;

  /* Read response */
  retval = 0;
  num = read (wget->readfd, &retval, sizeof(int));
  if (num == sizeof(int))
  {
    /* Check response */
    if (retval == 0)
    {
      /* Successfully received file */
      wget->got_result = 1;
    }
  }

  /* Terminate refresh */
  i_entity_refresh_terminate (ENTITY(met));

  return 0;
}
