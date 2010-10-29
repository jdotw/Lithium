#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <dlfcn.h>

#include "induction.h"
#include "hashtable.h"
#include "timer.h"

extern i_resource *global_self;

/* Resource State Checking functions
 *
 * These are run periodically to check the state of all resources
 * and the resource table
 *
 */

void i_resource_local_sleep_check_single (i_resource *self, i_hashtable *res_table, void *data)		/* Check to see if a resource needs to be woken up and do so */
{													/* Void is used so this can be called from i_hashtable_iterate */
  struct timeval now;
  i_resource *res = data;

  if (res->state != RES_STATE_SLEEP) return;							/* Not sleeping */

  gettimeofday (&now, NULL);

  if (now.tv_sec >= res->wake_time.tv_sec)
    i_resource_local_wake (self, res_table, RES_ADDR(res));
}

void i_resource_local_waiting_check_single (i_resource *self, i_hashtable *res_table, i_resource *res)
{
  struct timeval now;

  if (res->state != RES_STATE_WAITING_REGISTER) return;
  
  gettimeofday (&now, NULL);

  if ((now.tv_sec - res->spawn_time.tv_sec) > RES_WAITING_REGISTER_TIMEOUT_SEC)
  {
    i_printf (0, "i_resource_local_waiting_check_single restarting %i:%i:%s due to register timeout being exceeded", res->type, res->ident_int, res->ident_str);
    i_resource_local_restart (self, res_table, res);
  }
}

void i_resource_local_check_single (i_resource *self, i_hashtable *res_table, i_resource *res)
{
  if (res->state == RES_STATE_SLEEP)
    i_resource_local_sleep_check_single (self, res_table, res);
  if (res->state == RES_STATE_WAITING_REGISTER)
    i_resource_local_waiting_check_single (self, res_table, res);
}

int i_resource_local_check_all (i_resource *self, i_timer *timer, void *tableptr)
{
  /* Go through the res table and perform a check on all resources.
   * The check includes :
   *
   * 1) Waking sleeping resources if required
   * 2) Restarting waiting_register resources if required
   *
   */

  i_hashtable *res_table = tableptr;
  
  i_printf (1, "i_resource_local_check_all running");
  
  i_hashtable_iterate (self, res_table, i_resource_local_check_single);
  
  i_printf (1, "i_resource_local_check_all completed");

  return 0;
}


