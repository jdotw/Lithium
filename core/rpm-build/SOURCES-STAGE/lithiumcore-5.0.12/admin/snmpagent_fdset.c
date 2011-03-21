#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/hierarchy.h>

#include "snmpagent.h"

i_timer* static_snmptimeout_timer = NULL;

/* SNMP sub-system i_loop_fdset pre and post processors */

int l_snmpagent_fdset_preprocessor (i_resource *self, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data)
{
  int num;
  int fds=0;
  struct timeval timeout = { 0, 0 };
  int block = 1;

  num = snmp_select_info (&fds, read_fdset, &timeout, &block);
  if (block == 0)
  {
    /* The timeout value should be used as a select timeout, set a timer for
     * it 
     */
    if (!static_snmptimeout_timer)
    { static_snmptimeout_timer = i_timer_add (self, timeout.tv_sec, timeout.tv_usec, l_snmpagent_fdset_snmptimeout_callback, NULL); }
    else
    { i_timer_set_timeout (static_snmptimeout_timer, timeout.tv_sec, timeout.tv_usec); }
  }

  if (fds < 1) return 0;
  
  return fds-1;
}

int l_snmpagent_fdset_postprocessor (i_resource *self, int select_num, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data)
{
  /* Perform snmp_read if needed */
  if (select_num > 0)
  { 
    agent_check_and_process (0);
  }

  return 0;      
}

int l_snmpagent_fdset_snmptimeout_callback (i_resource *self, i_timer *timer, void *passdata)
{
  /* Called when the interval given by the last call 
   * to snmp_select_info has expired. 
   */

  snmp_timeout ();
  static_snmptimeout_timer = NULL;

  return -1;
}
