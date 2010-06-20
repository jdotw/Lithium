#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#include "induction.h"
#include "message.h"
#include "socket.h"
#include "msgproc.h"
#include "hashtable.h"
#include "timer.h"
#include "construct.h"
#include "module.h"
#include "heartbeat.h"

int i_heartbeat_enable (i_resource *self)
{
  /* Add timer */
  self->heartbeat_timer = i_timer_add (self, HEARTBEAT_SEND_SEC, 0, i_heartbeat_timer, NULL);
  return 0;  
}

int i_heartbeat_timer (i_resource *self, i_timer *timer, void *passdata)
{
  /*
   * ALWAYS Return 0 to keep the timer running!!!
   */
  
  /* Send heartbeat */
  long msgid = i_message_send (self, MSG_HEARTBEAT, NULL, 0, NULL, MSG_FLAG_REQ, 0);
  if (msgid == -1)
  {
    i_printf (0, "i_heartbeat_timer failed to send heartbeat -- terminating");
    exit (0); 
  }

  return 0;
}

