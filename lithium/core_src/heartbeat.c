#include <stdlib.h>

#include <induction.h>
#include <induction/message.h>
#include <induction/respond.h>
#include <induction/hashtable.h>
#include <induction/socket.h>
#include <induction/timer.h>
#include <induction/construct.h>
#include <induction/heartbeat.h>

#include "heartbeat.h"

extern i_hashtable *global_res_table;

int l_heartbeat_enable (i_resource *self, i_resource *res)
{
  /* Add timer */
  res->heartbeat_timer = i_timer_add (self, HEARTBEAT_CHECK_SEC, 0, l_heartbeat_timer, res);
  
  return 0;
}

int l_heartbeat_timer (i_resource *self, i_timer *timer, void *passdata)
{
  /* 
   * ALWAYS RETURN 0 To keep timer alive!! 
   */
  
  /* Check a heartbeat has been received */
  i_resource *res = passdata;
  if (res->heartbeat_recvd == 0)
  {
    /* No heartbeat received, restart */
    if (res->construct)
    {
      i_printf (0, "l_heartbeat_timer resource %i:%i:%s has no heartbeat -- restarting (pid %i)", res->type, res->ident_int, res->ident_str, res->construct->pid);
    }
    res->heartbeat_timer = NULL;
    res->heartbeatfail_count++;
    i_resource_local_restart (self, global_res_table, res);    
    return -1; /* Kill timer */
  }

  /* Reset flag */
  res->heartbeat_recvd = 0;

  return 0;
}

int l_heartbeat_handler (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* Called when a resource sends a heartbeat.
   * Always return 0 to keep the handler active 
   */

  /* Set flag */
  i_resource *res = passdata;
  res->heartbeat_recvd = 1;
  
  return 0;
}

