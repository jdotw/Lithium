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
#include "construct.h"
#include "module.h"
#include "hashtable.h"
#include "list.h"
#include "form.h"
#include "auth.h"
#include "hierarchy.h"
#include "socket.h"
#include "timeutil.h"
#include "cement.h"
#include "entity.h"
#include "inventory.h"
#include "object.h"
#include "metric.h"
#include "timer.h"
#include "ipregistry.h"
#include "svcregistry.h"
#include "procregistry.h"
#include "xsanregistry.h"
#include "incident.h"

extern i_resource *global_self;

/* Local resource manipulation functions 
 *
 * A local resource is a resource that was actually spawned from the 
 * running process
 * 
 */

i_resource* i_resource_local_get (i_hashtable *res_table, i_resource_address *res_addr)
{
  i_resource *res;
  i_hashtable_key *key;

  key = i_hashtable_key_resource (res_addr, res_table->size);
  if (!key) { i_printf (1, "i_resource_local_get couldnt create key"); return NULL; }

  res = (i_resource *) i_hashtable_get (res_table, key);
  i_hashtable_free_key (key);

  return res;
}

/* Resource creation */

i_resource* i_resource_local_create (i_resource *self, i_resource_address *owner_addr, i_hashtable *res_table, int type, int ident_int, char *ident_str, char *module_name, char *root, char *customer_id, i_entity *ent)
{
  int num;
  i_resource *res;
  i_construct *core_construct;
  struct stat statbuf;

  /* Create struct */
  res = (i_resource *) malloc (sizeof(i_resource));
  if (!res) 
  {
    i_printf (1, "i_resource_create_local failed to malloc res");
    return NULL;
  }
  memset (res, 0, sizeof(i_resource));
  if (!self) global_self = res;  /* !self means we were called by construct */

  /* Setup struct */
  res->type = type;                                       /* Note, these are the values being passed to the construct. Later they are duplicated to be unique to the struct */
  res->ident_int = ident_int;
  if (ident_str) res->ident_str = strdup (ident_str);
  if (module_name) res->module_name = strdup (module_name);
  if (root) res->root = strdup (root);
  if (customer_id) res->customer_id = strdup (customer_id);
  if (ent) res->ent = i_entity_duplicate (ent);
  if (owner_addr) res->owner = i_resource_address_duplicate (owner_addr);
  res->auth = i_authentication_create ();

  /* Inherit values */
  if (self) 
  {
    /* Set some inherited values */
    core_construct = self->construct;
    res->plexus = strdup (self->plexus);
    res->node = strdup (self->node);
    res->log_level = self->log_level;
  }
  else 
  {
    /* New stand alone module, no parent */
    core_construct = NULL;    
  }

  /* Perform ident_int auto-assign if needed */
  if (res->ident_int == -1)
  {
    /* Automatically assign an ident_int based on first-available */

    if (res_table)
    {
      res->ident_int = 0;
      while (i_resource_local_get (res_table, RES_ADDR(res)))
      {
        res->ident_int++;
      }
    }
    else res->ident_int = 0;    /* No res table, assume 0 */

  }
  
  /* Perform duplicate resource check */
  if (res_table)
  {
    i_hashtable_key *key;
    i_resource *existing;
  
    key = i_hashtable_key_resource (RES_ADDR(res), res_table->size);
    if (!key) { i_printf (1, "i_resource_create_local unable to create htkey during duplicate check"); return NULL; }
  
    existing = i_hashtable_get (res_table, key);                                  /* Check to see if there is a duplicate resource */
    i_hashtable_free_key (key);
    if (existing)
    {
      /* Duplicate resource, can't proceed */

      i_printf (1, "i_resource_create_local unable to create resource. %s:%s:%i:%i:%s already exists!", res->node, res->plexus, res->type, res->ident_int, res->ident_str);
      i_resource_free (res);
      errno = ERR_RES_DUPLICATE;
      return NULL;
    }
  }

  /* Make sure the fsroot exists */
  if (res->root)
  {
    num = stat (res->root, &statbuf);
    if (num != 0)
    {
      num = mkdir (res->root, 0750);
      if (num != 0)
      {
        i_printf (1, "i_resource_create_local failed to create res->root (%s %s)", res->root, strerror(errno));
        i_resource_free (res);
        return NULL;
      }
    }
  }

  /* Build the hierarchy */

  if (res_table && owner_addr)
  {
    i_hashtable_key *key;
    i_resource *parent;

    key = i_hashtable_key_resource (owner_addr, res_table->size);
    if (!key) { i_printf (1, "i_resource_create_local unable to create htkey during hierarchy build"); return NULL; }
    parent = i_hashtable_get (res_table, key);                                  /* Retrieve the parent */
    i_hashtable_free_key (key);

    res->hierarchy = i_hierarchy_build (res, parent);
    if (!res->hierarchy)
    { 
      i_printf (1, "i_resource_create_local failed to build resource hierarchy for %s:%s:%i:%i:%s", 
        res->plexus, res->node, res->type, res->ident_int, res->ident_str);
    }
  }

  /* Create the construct */
  res->construct = i_construct_create (core_construct, module_name);
  if (!res->construct) 
  { 
    i_printf (1, "i_resource_create_local unable to create construct for %s:%s:%i:%i:%s", res->node, res->plexus, res->type, res->ident_int, res->ident_str);
    i_resource_free (res);
    return NULL; 
  }

  /* Open module */
  if (module_name)									                                /* Module specified, ready to spawn */
  {
    int num;

    num = i_construct_spawn (self, res);                                            /* Spawn the construct */
    
    /* Place the resource into the resource table */
  
    if (res_table)
    {
      i_hashtable_key *key;
  
      key = i_hashtable_key_resource (RES_ADDR(res), res_table->size);
      if (!key) 
      { 
        i_printf (1, "i_resource_create_local unable to create htkey during ht_put"); 
        num = i_resource_local_destroy (self, res_table, RES_ADDR(res));
        if (num != 0)
        { 
          i_printf (1, "i_resource_create_local also failed to terminate new resource, freeing manually");
          i_resource_free (res); /* Terminate failed, free manually */
        }
        /* NOTE: res is freed by i_resource_local_terminate */
        return NULL; 
      }
  
      i_hashtable_put (res_table, key, res);
      i_hashtable_free_key (key);
    }

    /* NOTE: DO NOT CALL i_resource_free FROM THIS POINT ON. The
     * hashtable should have i_resource_free set as the destructor
     */
    
    if (num == -1) 
    {
      /* Spawn definately didnt work */
      i_printf (1, "i_resource_create_local failed initial spawning of resource, sleeping it");
      i_resource_local_sleep (self, res_table, RES_ADDR(res), RES_FAST_RESTART_SLEEP_SECONDS);  /* Put the resource to sleep */
    }
    else 
    {
      /* Spawn worked, waiting for registration
       * The module is 'spawned' but has not yet registerd. As such
       * the module is put in the WAITING_REGISTER state initially. The core
       * will then periodically check for WAITING_REGISTER state resources
       * and attempt a restart where required
       */

      gettimeofday (&res->spawn_time, NULL);
      res->state = RES_STATE_WAITING_REGISTER;
      
      /* Add the resource to the expected-incoming list */
      i_resource_local_incoming_list_add (self, res);
    }
  }

  return res;
}

int i_resource_local_spawn (i_resource *self, i_resource *res)
{
  /* Spawn the given resource's construct */

  int num;
  
  res->construct = i_construct_create (self->construct, res->module_name);
  if (!res->construct)
  {
    i_printf (1, "i_resource_local_spawn failed to create construct");
    return -1;
  }

  num = i_construct_spawn (self, res, res->config_form);
  if (num == -1)
  {
    i_printf (1, "i_resource_local_spawn failed to spawn construct");
    return -1;
  }

  i_printf (0, "i_resource_local_spawn spawned resource for %i:%i:%s", res->type, res->ident_int, res->ident_str);

  return 0;
}

/* 
 * RESOURCE TERMINATION
 *
 * When a resource is terminated the process is killed and 
 * the construct freed. Any resources owned by the terminated
 * resource are also terminated
 *
 */

typedef struct
{
  i_list *child_list;
  i_resource *term_res;
} i_resource_local_terminate_iterate_data;

void i_resource_local_terminate_iterate (i_resource *self, i_hashtable *res_table, void *data, void *passdata)
{
  /* Loops through the resource table to find the following:
   *
   * Resources owned by the resource being terminated. These resources are
   * them terminated and removed from the restable
   */

  int num;
  i_resource *res = data;
  i_resource_local_terminate_iterate_data *iterate_data = passdata;

  if (!iterate_data || !iterate_data->term_res) return;

  num = i_resource_address_compare (RES_ADDR(iterate_data->term_res), res->owner);
  if (num == 0)
  {
    /* This resource is owned by the resource being terminated. Kill it too */
    i_list_enqueue (iterate_data->child_list, res);
  }
}

int i_resource_local_terminate (i_hashtable *res_table, i_resource_address *res_addr)
{
  /* Terminates a resource. When a resource is terminated, the hashtable is
   * iterated to look for resources which are owned by this resource. Anything
   * owned by this resource (and subsequent owned resources) and terminated
   * and removed from the resource table. This is based on the assumption that
   * those resources will be respawned alone with the original resource 
   */
  int num;
  i_resource *res;
  i_resource *child_res;
  i_resource_local_terminate_iterate_data *iterate_data;

  /* Find resource to be terminated */

  res = i_resource_local_get (res_table, res_addr);
  if (!res) { i_printf (1, "i_resource_local_terminate did not find resource to terminate"); return -1; }

  /* Build list of all child resources (i.e resources owned by this one or its
   * children
   */
  
  iterate_data = (i_resource_local_terminate_iterate_data *) malloc (sizeof(i_resource_local_terminate_iterate_data));
  if (!iterate_data)
  { i_printf (1, "i_resource_local_terminate failed to malloc iterate_data struct"); return -1; }
  memset (iterate_data, 0, sizeof(i_resource_local_terminate_iterate_data));
  iterate_data->child_list = i_list_create ();
  iterate_data->term_res = res;

  i_hashtable_iterate (global_self, res_table, i_resource_local_terminate_iterate, iterate_data);

  /* Destroy child resources */

  for (i_list_move_head(iterate_data->child_list); (child_res=i_list_restore(iterate_data->child_list))!=NULL; i_list_move_next(iterate_data->child_list))
  {
    num = i_resource_local_destroy (global_self, res_table, RES_ADDR(child_res));
    if (num != 0)
    { 
      i_printf (1, "i_resource_local_terminate child resource destruction failed for %s:%s:%i:%i:%s", 
        res->plexus, res->node, res->type, res->ident_int, res->ident_str); 
    }
  }

  i_list_free (iterate_data->child_list);
  free (iterate_data);

  /* Free resource's core_socket */
  if (res->core_socket)
  {
    i_socket_free (res->core_socket);
    res->core_socket = NULL;
  }

  /* Free resources heartbeat timer */
  if (res->heartbeat_timer)
  {
    i_timer_remove (res->heartbeat_timer);
    res->heartbeat_timer = NULL;
  }

  /* Destroy the construct */
  if (res->construct)
  {
    if (res->construct->heartbeat_failed)
    {
      /* Heartbeat failure, perform stack trace and then kill */
      pid_t child_pid = fork ();
      if (child_pid == 0)
      {
        /* Child process forked to perform a hung process log */
        char *command_str;
        asprintf (&command_str, "/Library/Lithium/LithiumCore.app/Contents/MacOS/hang_reporter.sh %i", res->construct->pid);
        execlp ("/bin/sh", "sh", "-c", command_str, NULL);
        free (command_str);
        exit (0);
      }
    }
    else
    {
      /* Normal process termination */
//      if (res->construct->pid > 0) { 
//        i_printf (0, "i_resource_local_terminate attempting to kill PID %i for %i:%i:%s", res->construct->pid, res->type, res->ident_int, res->ident_str);
//        kill (res->construct->pid, SIGTERM); 
//      } /* DEBUG: Original */
      //if (res->construct->pid > 0) { kill (res->construct->pid, SIGABRT); } /* DEBUG to find hangs */
    }
    i_construct_free (res->construct);
    res->construct = NULL;
  }

  /* De-register IPs and Software */

  if (res->type == RES_DEVICE)
  {
    i_resource *customer_res;

    customer_res = i_resource_local_get (res_table, res->hierarchy->cust_addr);
    if (customer_res)
    {
      num = i_ipregistry_deregister (customer_res, res->hierarchy->cust_addr, RES_ADDR(res), NULL);
      if (num == -1)
      { i_printf (1, "i_resource_local_terminate warning, failed to de-register IP registry entries for device %s", res->ident_str); }
      num = i_svcregistry_deregister (customer_res, res->hierarchy->cust_addr, RES_ADDR(res), NULL);
      if (num == -1)
      { i_printf (1, "i_resource_local_terminate warning, failed to de-register service registry entries for device %s", res->ident_str); }
      num = i_procregistry_deregister (customer_res, res->hierarchy->cust_addr, RES_ADDR(res), NULL);
      if (num == -1)
      { i_printf (1, "i_resource_local_terminate warning, failed to de-register process registry entries for device %s", res->ident_str); }
      num = i_xsanregistry_deregister (customer_res, res->hierarchy->cust_addr, RES_ADDR(res), NULL);
      if (num == -1)
      { i_printf (1, "i_resource_local_terminate warning, failed to de-register xsan registry entries for device %s", res->ident_str); }
      num = i_inventory_deregister (customer_res, res->hierarchy->cust_addr, RES_ADDR(res), INV_ALL, NULL, NULL, NULL);
      if (num == -1)
      { i_printf (1, "i_resource_local_terminate warning, failed to de-register inventory entries for device %s", res->ident_str); }
      num = i_incident_clearall (customer_res, res->hierarchy->cust_addr, RES_ADDR(res));
      if (num == -1)
      { i_printf (1, "i_resource_local_terminate warning, failed to clear all incidents for device %s", res->ident_str); }
    }
    else
    { i_printf (1, "i_resource_local_terminate warning, failed to find customer_res for device %s", res->ident_str); }
  }

  return 0;		
}

/* RESOURCE REMOVAL
 *
 * When i_resource_local_remove is called the resource is just
 * removed from the res_table
 */

int i_resource_local_remove (i_resource *self, i_hashtable *res_table, i_resource_address *res_addr)
{
  /* Remove a resource from the hashtable. */

  int num;
  i_hashtable_key *key;

  key = i_hashtable_key_resource (res_addr, res_table->size);
  if (!key) { i_printf (1, "i_resource_local_remove couldnt create key"); return -1; }

  num = i_hashtable_remove (res_table, key);
  i_hashtable_free_key (key);
  if (num == -1) { i_printf (1, "i_resource_local_remove unable to remove hashtable entry"); return -1; }

  return 0;
}

/* DEVICE DESTRUCTION
 *
 * When i_resource_local_destroy is called a terminate and remove combination
 * is performed on the given res in the given res_table
 */

int i_resource_local_destroy (i_resource *self, i_hashtable *res_table, i_resource_address *res_addr)
{
  int num;

  num = i_resource_local_terminate (res_table, res_addr);
  if (num != 0)
  { i_printf (1, "i_resource_local_destroy failed to terminate resource. removing anyway"); }

  num = i_resource_local_remove (self, res_table, res_addr);
  if (num != 0)
  { 
    i_printf (1, "i_resource_local_destroy failed to remove resource. termination succeeded");
    return -1;
  }

  return 0;
}

int i_resource_local_sleep (i_resource *self, i_hashtable *res_table, i_resource_address *res_addr, int seconds)		/* Puts a resource to sleep */
{
  /* Kills the resource's construct and sets it to a sleep state */

  i_resource *res;
  struct timeval now;
        
  res = i_resource_local_get (res_table, res_addr);
  if (!res) { i_printf (1, "i_resource_local_sleep did not find resource to terminate"); return -1; }
  
  if (res == self) { i_printf (1, "i_resource_local_sleep can not put self resource to sleep"); return -1; }

  gettimeofday (&now, NULL);

  i_resource_local_terminate (res_table, RES_ADDR(res));     /* Terminate the resource's process */
  
  res->state = RES_STATE_SLEEP;
  res->status = ENTSTATE_CRITICAL;
  res->wake_time.tv_sec = now.tv_sec + seconds;
  res->wake_time.tv_usec = 0;

  return 0;
}

int i_resource_local_wake (i_resource *self, i_hashtable *res_table, i_resource_address *res_addr)	/* Awakens a sleeping resource */
{
  int num;
  i_resource *res;
        
  res = i_resource_local_get (res_table, res_addr);
  if (!res) { i_printf (1, "i_resource_wake did not find resource to wake"); return -1; }

  num = i_resource_local_spawn (self, res);
  if (num != 0)
  { 
    i_printf (1, "i_resource_local_wake failed to spawn resource construct - resleeping"); 
    i_resource_local_sleep (self, res_table, res_addr, RES_FAST_RESTART_SLEEP_SECONDS);  /* Put the resource to sleep */
    return -1; 
  }

  gettimeofday (&res->spawn_time, NULL);                                                  /* Record spawn time */
  res->state = RES_STATE_WAITING_REGISTER;	

  /* Add the resource to the expected-incoming list */
  i_resource_local_incoming_list_add (self, res);

  return 0;
}

int i_resource_local_restart (i_resource *self, i_hashtable *res_table, i_resource *res)			/* Terminate and restart and do the necessary house keeping */
{													/* Returns 0 on immediate restart, 1 on sleep for fast restart and -1 for error */	
  int num;
  struct timeval now;

  if (!self || !res) return -1;
  
  i_printf (1, "i_resource_local_restart res->fast_restart_count for %i:%i:%s is %i", res->type, res->ident_int, res->ident_str, res->fast_restart_count);

  /* Check for a fast-restart condition */
  
  if (res->fast_restart_count > RES_FAST_RESTART_THRESHOLD)                                 /* Has this resource gone restart-happy */
  {
    i_printf (2, "i_resource_local_restart putting device %i:%i:%s asleep for fast restarts", res->type, res->ident_int, res->ident_str);
  
    i_resource_local_sleep (self, res_table, RES_ADDR(res), RES_FAST_RESTART_SLEEP_SECONDS);  /* Put the resource to sleep */
    res->fast_restart_count = 0;  /* Clear the slate for the restart */

    return 1;
  }

  /* Terminate the resource */

  i_resource_local_terminate (res_table, RES_ADDR(res));
  
  /* Respawn */

  num = i_resource_local_spawn (self, res);
  if (num == -1) 
  { 
    i_printf (1, "i_resource_local_restart failed to spawn new construct - putting it to sleep"); 
    i_resource_local_sleep (self, res_table, RES_ADDR(res), RES_FAST_RESTART_SLEEP_SECONDS);  /* Put the resource to sleep */
    return -1; 
  }
  res->state = RES_STATE_WAITING_REGISTER;

  /* Add the resource to the expected-incoming list */
  i_resource_local_incoming_list_add (self, res);

  gettimeofday (&now, NULL);

  i_printf (1, "i_resource_local_restart i_time_diff_seconds between restarts for %i:%i:%s is %i", res->type, res->ident_int, res->ident_str, i_time_diff_seconds(&now, &res->spawn_time));

  if ((i_time_diff_seconds(&now, &res->spawn_time)) < RES_FAST_RESTART_SECONDS)		/* Is this a fast restart */
    res->fast_restart_count++;
  else
    res->fast_restart_count = 0;

  gettimeofday (&res->spawn_time, NULL);                                                  /* Record spawn time */
  res->restart_count++;

  return 0;
}

