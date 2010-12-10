#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <dlfcn.h>

#include "induction.h"
#include "construct.h"
#include "module.h"
#include "auth.h"
#include "loop.h"
#include "heartbeat.h"
#include "init.h"
#include "timer.h"
#include "memcheck.h"

extern i_resource *global_self;

void i_construct_free (i_construct *construct)
{
  if (!construct) return;

  if (construct->module_name) free (construct->module_name);
  if (construct->module_path) free (construct->module_path);
  if (construct->log_path) free (construct->log_path);
  if (construct->config_path) free (construct->config_path);
  if (construct->module) i_module_free (construct->module);

  free (construct);
}

i_construct* i_construct_create (i_construct *core, char *module_name)				/* Form the new construct */
{
  i_construct *construct;

  construct = (i_construct *) malloc (sizeof(i_construct));
  if (!construct) return NULL;
  memset (construct, 0, sizeof(i_construct));

  if (module_name) construct->module_name = strdup (module_name);

  if (core) 										                                  /* This is to be a non-core construct */
  {
    if (core->module_path) construct->module_path = strdup (core->module_path);	      /* Clone module_path */
    if (core->config_path) construct->config_path = strdup (core->config_path);	      /* Clone config_path */
    if (core->log_path) construct->log_path = strdup (core->log_path);	              /* Clone log_path */
    construct->quiet = core->quiet;		            			                      /* Same 'quiet' status is assume */
  }

  construct->id = rand ();								                              /* Create the random contruct ID */

  return construct;
}

int i_construct_spawn (i_resource *parent, i_resource *resource) /* Spawn the construct, passing data to it, return PID */
{
  int pid;
  int num;

  if (!parent || !resource) { i_printf (1, "i_construct_spawn called without parent/resource. failed"); return -1; }

  if (!resource->construct->module_name) 
  {
    i_printf (1, "i_construct_spawn called with no construct->module_name");
    return -1;
  }

  if (i_module_check_construct (resource->construct) == -1) 
  {
    i_printf (1, "i_construct_spawn failed module check for %s", resource->construct->module_name);
    return -1; 
  }

  if (parent != resource)	        							                      /* We are spawning a child of the core, fork the process */
  {
    pid = fork ();									                                  /* Fork the process */
    resource->construct->pid = pid;
    resource->perflog = parent->perflog;

    if (pid != 0) 									                                  /* Are we the core process ? */
    {
      return pid;								                                      /* Return back to the core */
    }
  }

  /* ############################################################################################*/
  /* This is now the newly spawned construct */

  global_self = resource;                                                               /* Change the global self */

  if (parent != resource)                                                              /* Make sure we're not spawning ourselves (construct) */
  {
    /* 
     * This is a new child resource. Free some things not
     * needed by the child but inherited from the fork
     * of the parent process.
     */

    /* Here we try and free as much memory as we can */

    if (parent->construct->module)
    {
      const char *errstr;
      void* (*fork_clean_up_func) (i_resource *parent, i_resource *resource);           /* A function which cleans up a modules instance specific data after a fork */

      fork_clean_up_func = dlsym (parent->construct->module->handle, "module_fork_clean_up"); /* Try to resolve the symbol */
      errstr = dlerror();
      if (!errstr) { fork_clean_up_func (parent, resource); }                            /* Run the func if present */
    }
    i_resource_free (parent);
    parent = NULL;    /* Parent would _NOT_ have been freed in the cleanup */
  }

  num = i_init (resource);   /* Initialise induction */
  if (num != 0)
  { i_printf (0, "i_construct_spawn failed to initialise induction (i_init). terminating"); exit (1); }

  resource->construct->module = i_module_open (resource->construct->module_path, resource->construct->module_name);	/* Open our initial .so */
  if (!resource->construct->module) { i_printf (0, "i_construct_spawn terminating newly spawned construct. module open failed"); exit (1); }

  if (resource->type != RES_CORE && resource->type != RES_EXEC && resource->type != RES_LCON && resource->type != RES_CLIENT_HANDLER_CHILD)
  {
    /* Non-core resource */

    resource->auth->level = AUTH_LEVEL_INTERNAL;

    num = i_resource_register (resource, "core", i_construct_spawn_post_registration, NULL);
    if (num != 0)
    { i_printf (1, "i_construct_spawn failed to start resource registration process. terminating"); exit (1); }

    num = i_heartbeat_enable (resource);
    if (num != 0)
    { i_printf (1, "i_construct_spawn failed to enable heartbeats"); }
  }
  else
  {
    num = resource->construct->module->init (resource);
    if (num != 0)
    { i_printf (1, "i_construct_spawn failed to call module_init. terminating"); exit (1); }
    num = resource->construct->module->entry (resource);
    if (num != 0)
    { i_printf (1, "i_construct_spawn failed to call module_init. terminating"); exit (1); }
  }

  /* Add default timers */
#ifdef OS_DARWIN
  i_timer_add (resource, 60, 0, i_memcheck_perflog_timer, NULL);
#endif

  /* Enter loop */
  i_loop (resource);

  /* Loop exited, terminate */

  num = resource->construct->module->shutdown (resource);
  if (num == -1) { i_printf (0, "i_construct_spawn module_shutdown failed after i_loop exited (%s)", resource->construct->module_name); exit (1); }
      
  exit (0);										/* Module should have taken care of this, but just incase */

  /* ############################################################################################*/
}

int i_construct_spawn_post_registration (i_resource *self, int result, void *passdata)
{
  int num;

  if (result != 0)
  {
    i_printf (0, "i_construct_spawn_post_registration recvd -1 result. exiting.");
    exit (1);
  }

  num = self->construct->module->init (self);                  /* Initialise the module */
  if (num == -1) { i_printf (0, "i_construct_spawn_post_registration module_init failed (%s)", self->construct->module_name); exit (1); }

  num = self->construct->module->entry (self);                 /* Run the modules entry function */
  if (num == -1) { i_printf (0, "i_construct_spawn_post_registration module_entry failed (%s)", self->construct->module_name); exit (1); }
  
  return 0;
}

