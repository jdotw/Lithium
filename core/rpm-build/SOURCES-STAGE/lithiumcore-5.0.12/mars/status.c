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

#include "socket.h"
#include "rrd.h"
#include "status.h"

int m_status_timercb (i_resource *self, i_timer *timer, void *passdata)
{
  i_list *proclist;
  i_list *freeproclist;
  i_list *cmdqueue;
  unsigned long update_count;

  /* Command queue */
  cmdqueue = m_rrd_cmdqueue ();
  if (cmdqueue)
  { i_printf (0, "m_status_timercb queue depth is %i", cmdqueue->size); }
  else
  { i_printf (0, "m_status_timercb queue is empty"); }

  /* Drops */
  int drops = m_rrd_q_drops ();
  m_rrd_q_drops_reset ();

  /* Process counts */
  proclist = m_rrd_proclist ();
  freeproclist = m_rrd_freeproclist ();
  i_printf (0, "m_status_timercb %i rrdtool processes present, %i processes idle", proclist->size, freeproclist->size);

  update_count = m_socket_updatecount ();
  if (update_count > 0)
  { i_printf (0, "m_status_timercb update rate is %.2f updates/sec (%li updates in %i seconds)", (float) update_count / UPDATE_INT_SEC, update_count, UPDATE_INT_SEC); }
  else
  { i_printf (0, "m_status_timercb no updates have been processed in %i seconds", UPDATE_INT_SEC); }
  m_socket_updatecount_reset ();

  if (drops > 0)
  { 
    i_printf (0, "m_status_timercb %i updates were dropped due to limited disk i/o", drops); 
  }  

  return 0;
}
