#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <induction.h>
#include <induction/timer.h>
#include <induction/hashtable.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/list.h>
#include <induction/path.h>
#include <induction/log.h>
#include <induction/configfile.h>
#include <induction/files.h>
#include <induction/form.h>

#include "lithium.h"
#include "config.h"
#include "main.h"
#include "resource_message.h"
#include "incoming.h"
#include "spawn.h"

i_hashtable *global_res_table = NULL;
i_list *global_spawn_schedule_list = NULL;
struct timeval global_start_tv;

/* Define a few things about this module */

void module_info ()
{
	i_printf (0, "Lithium - Core Module");
}

int module_init (i_resource *self)
{
  struct rlimit lim;

  gettimeofday (&global_start_tv, NULL);

  /* Ensure paths exist */
#ifdef OS_DARWIN
  system ("mkdir -p /Library/Logs/Lithium");
  system ("mkdir -p /Library/Logs/Lithium/ClientService");
  system ("chown root:lithium /Library/Logs/Lithium/ClientService");
  system ("chmod g+w /Library/Logs/Lithium/ClientService");
#endif

  /* Set rlimit */
  getrlimit(RLIMIT_NOFILE, &lim);
  i_printf (0, "module_init rlimit RLIMIT_NOFILE was %li (%li)", lim.rlim_max, lim.rlim_cur);

  lim.rlim_cur = lim.rlim_max = 8192;
  if (setrlimit(RLIMIT_NOFILE, &lim) != 0)
  { i_printf (1, "module_init warning, failed to set rlimit"); }

  getrlimit(RLIMIT_NOFILE, &lim);
  i_printf (0, "module_init rlimit RLIMIT_NOFILE is now %li (%li)", lim.rlim_max, lim.rlim_cur);

  getrlimit(RLIMIT_NPROC, &lim);
  i_printf (0, "module_init rlimit RLIMIT_NPROC is now %li (%li)", lim.rlim_max, lim.rlim_cur);

  i_printf (0, "module_init complete");
  return 0;
}

int module_entry (i_resource *self)
{
  int num;
  char *root;
  char *sock_path;
  char *str;
  i_hashtable *res_table;

  i_printf (0, "module_entry entered");

  /* Enable logging */
  num = i_log_enable (self, "core_log.db");
  if (num != 0)
  { i_printf (1, "module_entry warning, failed to enable i_log"); }

  /* Create the spawn schedule list */
  global_spawn_schedule_list = i_list_create ();
  if (!global_spawn_schedule_list) 
  { i_printf (1, "module_entry failed to create global_spawn_schedule_list"); return -1; }
  i_list_set_destructor (global_spawn_schedule_list, l_spawn_schedule_free_callbackdata);
  
  /* Create the resource table */
  res_table = i_hashtable_create (RES_TABLE_SIZE);
  if (!res_table) { i_printf (1, "module_entry unable to create res_table"); return -1; }
  global_res_table = res_table;
  i_hashtable_set_destructor (res_table, i_resource_free);

//  i_timer *timer;
//  timer = i_timer_add (self, RES_CHECK_INTERVAL_SEC, 0, i_resource_local_check_all, res_table);
//  if (!timer) { i_printf (1, "module_entry unable to add resource check timer"); return -1; }

  /* Create the core listener socket */
  sock_path = i_path_socket("lithium", "core");
  self->core_socket = i_socket_create_unix (sock_path, SOCKET_LISTEN);
  free (sock_path);
  if (!self->core_socket) return -1;
  self->core_socket->read_preempt_callback = i_socket_callback_add (self, SOCKET_CALLBACK_READ_PREEMPT, self->core_socket, l_incoming_resource, res_table);
  if (!self->core_socket->read_preempt_callback)
  { i_printf (1, "module_entry unable to add core listener socket callback"); return -1; }

  /* Spawn the SMS resource */
  str = i_configfile_get_section (self, NODECONF_FILE, "sms");
  if (str)
  {
    free (str);
    root = i_path_glue (self->root, "sms");
    num = l_spawn_scheduled (self, res_table, RES_SMS, 0, NULL, "sms.so", root, NULL, 1);
    free (root);
    if (num != 0) { i_printf (1, "module_entry failed to spawn sms resource"); return -1; }
  }

  /* Spawn the client handler resource */
  root = i_path_glue (self->root, "client_handler");
  num = l_spawn_scheduled (self, res_table, RES_CLIENT_HANDLER_CORE, 0, NULL, "client_handler.so", root, NULL, 1);
  free (root); 
  if (num != 0) { i_printf (1, "module_entry unable to i_create_resource for client handler"); return -1; }

  /* Spawn the admin resource */
  num = l_spawn_scheduled (self, res_table, RES_ADMIN, 0, NULL, "admin.so", self->root, NULL, 1);
  if (num != 0) { i_printf (1, "module_entry unable to create admin resource"); return -1; }

  /* Create the res spawn schedule timer */
  i_timer_add (self, 0, 500000, l_spawn_scheduled_callback, NULL);

  return 0;
}

int module_shutdown (i_resource *self)
{
  i_printf (0, "module_shutdown complete");
  return 0;
}


