#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dlfcn.h>
#include <signal.h>
#include <errno.h>
#include <syslog.h>

#include <induction.h>
#include <induction/construct.h>
#include <induction/module.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/auth.h>
#include <induction/configfile.h>
#include <induction/files.h>
#include <induction/loop.h>
#include <induction/version.h>

#include "config.h"
#include "usage.h"
#include "construct.h"
#include "version.h"
#include "signal_handlers.h"
#include "funcexec.h"

extern i_resource *global_self;

int main (int argc, char *argv[])
{
  int num;
  int exec_option = 0;
  char opt;
  extern int optind;
  extern char *optarg;
  char *node_ip = NULL;
  struct stat statbuf;
  i_resource *self;
  char hostname[256];

  signal (SIGPIPE, SIG_IGN);
//  signal (SIGSEGV, c_segfault_handler);
  signal (SIGABRT, c_abort_handler);
  signal (SIGCHLD, SIG_IGN);

  signal (SIGHUP, c_sighup_handler);

  char *dotptr;
  gethostname (hostname, 256);
  dotptr = strchr (hostname, '.');
  if (dotptr) *dotptr = '\0';
  
  self = i_resource_local_create (NULL, NULL, NULL, RES_CORE, 0, NULL, NULL, NULL, NULL, NULL);
  global_self = self;
  if (!self)
  {
    printf ("Failed to create self resource.\n");
    exit (2);
  }

  /* Process command line args */

  while ((opt = getopt(argc, argv, "+c:C:m:l:vVequ:p:n:r:zP")) != EOF)
  {
    switch (opt)
    {
      case 'c':	self->construct->config_path = strdup (optarg);
                break;
      case 'm':	self->construct->module_path = strdup (optarg);
                break;
      case 'l':	self->construct->log_path = strdup (optarg);
                break;
      case 'r': if (self->root) free (self->root);
                self->root = strdup (optarg);
                break;
      case 'v': self->log_level++;
                break;
      case 'V':	c_version ();
                exit (0);
                break;
      case 'e':	exec_option = 1;
                self->type = RES_EXEC;				/* We are an executing resource */
                break;
      case 'q':	self->construct->quiet = 1;
                break;
      case 'u': self->auth->username = strdup (optarg);
                break;
      case 'p': self->auth->password = strdup (optarg);
                break;
      case 'C': self->auth->customer_id = strdup (optarg);
                break;
      case 'n': node_ip = strdup(optarg);
                break;
      case 'z': sleep(200);
                break;
      case 'P': self->perflog++;
                break;
    }
  }

  /* Set paths */
  if (!self->root) self->root = strdup (DATA_PATH);
  if (!self->construct->config_path) self->construct->config_path = strdup (CONFIG_PATH);
  if (!self->construct->module_path) self->construct->module_path = strdup (MODULE_PATH);
  if (!self->construct->log_path) self->construct->log_path = strdup (LOG_PATH);

  /* Create root */
  num = stat (self->root, &statbuf);
  if (num != 0)
  {
    num = mkdir (self->root, 0750);
    if (num != 0)
    {
      i_printf (1, "Error: failed to create data path (%s: %s)", self->root, strerror(errno));
      exit (2);
    }
  }

  /* Get configuration info */
  if ((i_configfile_check_exists(self, NODECONF_FILE)) == 1)
  {
    char *log_level_str;

    self->plexus = i_configfile_get (self, NODECONF_FILE, "id", "plexus", 0);
    self->node = i_configfile_get (self, NODECONF_FILE, "id", "node", 0);
    log_level_str = i_configfile_get (self, NODECONF_FILE, "logging", "level", 0);
    if (log_level_str) { self->log_level = atoi (log_level_str); free (log_level_str); }
    else { self->log_level = 1; }
  }
  if (!self->plexus) 
  {
    if (self->construct->quiet == 0)
    { printf ("Warning: plexus name not configured\n"); }
    self->plexus = strdup (hostname);
  }
  if (!self->node) 
  { 
    if (self->construct->quiet == 0)
    { printf ("Warning: node name not configured\n"); }
    self->node = strdup (hostname); 
  }

  /* Set module name */

  if (argc == optind) { i_printf (1, "construct_main called with no module specified\n"); c_usage (); }
  self->construct->module_name = strdup (argv[optind]);

  /* Info */
  
  if (self->construct->quiet == 0)
  {
    printf ("%s - Modular Execution Environment\n", PACKAGE);
    printf ("Copyright (C) 2002-2009, LithiumCorp Pty Ltd (ACN 101 271 201)\n");
    printf ("Copyright (C) 1998-2002, James C. Wilson\n\n");

    /* Version Information */
    printf ("Executable Version : %s\n", VERSION);
    printf ("Library Version    : %s\n\n", i_version());

    printf ("\tself->root        set to %s\n", self->root);
    printf ("\tself->config_path set to %s\n", self->construct->config_path);
    printf ("\tself->module_path set to %s\n", self->construct->module_path);
    printf ("\tself->log_path    set to %s\n", self->construct->log_path);
    printf ("\tself->log_level   set to %i\n", self->log_level);
    printf ("\tself->module_name set to %s\n\n", self->construct->module_name);

    printf ("%s initialisation complete.\n\n", PACKAGE);
  }
	
  /* Load the specified module */

  if (exec_option == 0)
  {
    /* Normal module startup */
    if (self->construct->quiet == 0) printf ("Spawning module %s\n\n", self->construct->module_name);
    num = i_construct_spawn (self, self, 0);
    if (num == -1) i_printf (1, "construct_main failed to i_construct_spawn()");				
  }
  else
  {
    /* Execute a specific functions */
    num = c_funcexec (self, node_ip, argc, argv, optind);
    free (node_ip);
    if (num != 0)
    { i_printf (1, "construct_main failed to call c_funcexec"); exit (2); }
    /* Enter loop */
    i_loop (self);
  }

  return 0;
}
