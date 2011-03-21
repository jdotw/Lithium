#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include <induction.h>
#include <induction/configfile.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/site.h>
#include <induction/list.h>
#include <induction/hierarchy.h>
#include <induction/version.h>

#include "dhcp.h"

/* 
 * ISC DHCP Management Sub-System
 */

/* Daemon Manipulation */

int l_dhcp_daemon_start (i_resource *self)
{
  int num;
  char *str;

  /* Get filename */
  str = i_configfile_get (self, DHCPCONF_FILE, "dhcp", "commandline", 0);
  if (!str)
  { str = strdup ("dhcpd"); }

  /* Execute */
  num = system (str);
  if (num != 0)
  { i_printf (1, "l_dhcp_daemon_start failed to execute %s", str); free (str); return -1; }
  free (str);
  
  return 0;
}

int l_dhcp_daemon_stop (i_resource *self)
{
  int num;
  pid_t pid;

  /* Retrieve PID */
  num = l_dhcp_daemon_pid (self, &pid);
  if (num == -1)
  { i_printf (1, "l_dhcp_daemon_stop failed to retrieve PID for dhcpd"); return -1; }

  /* Kill */
  num = kill (pid, SIGTERM);
  if (num == -1)
  { i_printf (1, "l_dhcp_daemon_stop failed to kill PID %lu", pid); return -1; }

  return 0;
}

int l_dhcp_daemon_restart (i_resource *self)
{
  int num;

  /* Stop */
  num = l_dhcp_daemon_stop (self);
  if (num != 0)
  { i_printf (1, "l_dhcp_daemon_restart warning, failed to stop daemon"); }

  /* Start */
  num = l_dhcp_daemon_start (self);
  if (num != 0)
  { i_printf (1, "l_dhcp_daemon_restart failed to start daemon"); return -1; }

  return 0;
}

int l_dhcp_daemon_update (i_resource *self)
{
  int num;
  
  /* Write config */
  num = l_dhcp_daemon_writeconf (self);
  if (num != 0)
  { i_printf (1, "l_dhcp_daemon_update warning, failed to write DHCP daemon config"); }

  /* Restart daemon */
  num = l_dhcp_daemon_restart (self);
  if (num != 0)
  { i_printf (1, "l_dhcp_daemon_update failed to restart DHCP daemon"); return -1; }

  return 0;
}

/* Daemon PID */

int l_dhcp_daemon_pid (i_resource *self, pid_t *pidptr)
{
  int fd;
  char *str;
  off_t size;
  pid_t pid;

  /* Init pid */
  memset (pidptr, 0, sizeof(pid_t));
  
  /* Get filename */
  str = i_configfile_get (self, DHCPCONF_FILE, "dhcp", "pid_file", 0);
  if (!str)
  { str = strdup ("/var/run/dhcpd.pid"); }

  /* Open pid file */
  fd = open (str, O_RDONLY);
  if (fd == -1)
  { i_printf (1, "l_dhcp_daemon_pid failed to open PID file %s", str); free (str); return -1; }
  free (str);

  /* Get file size */
  size = lseek (fd, 0, SEEK_END);
  if (size == -1)
  { i_printf (1, "l_dhcp_daemon_pid failed to obtain PID file size"); close (fd); return -1; }
  lseek (fd, 0, SEEK_SET);

  /* Malloc */
  str = (char *) malloc (((size_t) size) + 1);
  if (!str)
  { i_printf (1, "l_dhcp_daemon_pid failed to malloc str (%i bytes)", size); close (fd); return -1; }
  memset (str, 0, ((size_t) size) + 1);

  /* Read */
  size = read (fd, str, (size_t) size);
  close (fd);
  if (size == -1)
  { i_printf (1, "l_dhcp_daemon_pid failed to read PID string from file"); free (str); return -1; }

  /* Convert str */
  pid = strtoul (str, NULL, 10);
  free (str);

  /* Copy PID */
  *pidptr = pid;

  return 0;
}

/* Daemon Configuration */

int l_dhcp_daemon_writeconf (i_resource *self)
{
  /* 
   * Write dhcpd.conf file based on Lithium DHCP management system 
   */

  char *str;
  FILE *conf;
  char *file_str;
  l_dhcp_subnet *subnet;
  i_list *subnet_list = l_dhcp_subnet_list();

  /* Obtain config file string */
  file_str = i_configfile_get (self, DHCPCONF_FILE, "dhcp", "conf_file", 0);
  if (!file_str)
  { file_str = strdup ("/etc/dhcpd.conf"); }

  /* Open conf file */
  conf = fopen (file_str, "w+");
  if (!conf)
  { i_printf (1, "l_dhcp_daemon_writeconf failed to open config file %s", file_str); free (file_str); return -1; }
  free (file_str);
  
  /* Defaults */
  fprintf (conf, "# Lithium %s DHCP Management System\n# \n", i_version());
  fprintf (conf, "# This file is created by Lithium, do not edit it.\n");

  /* DNS Servers */
  fprintf (conf, "\n# Default DNS Servers\n");
  str = i_configfile_get (self, DHCPCONF_FILE, "dhcp", "domain-name-servers", 0);
  if (str)
  { fprintf (conf, "option domain-name-servers %s;\n", str); free (str); }
  
  /* Default Lease Time */
  fprintf (conf, "\n# Default Lease Time\n");
  str = i_configfile_get (self, DHCPCONF_FILE, "dhcp", "default-lease-time", 0);
  if (str)
  { fprintf (conf, "default-lease-time %s;\n", str); free (str); }
  else
  { fprintf (conf, "default-lease-time 604800;\n"); }
  
  /* Maximum Lease Time */
  fprintf (conf, "\n# Maximum Lease Time\n");
  str = i_configfile_get (self, DHCPCONF_FILE, "dhcp", "max-lease-time", 0);
  if (str)
  { fprintf (conf, "max-lease-time %s;\n", str); free (str); }
  else
  { fprintf (conf, "max-lease-time 604800;\n"); }
  
  /* Authoritative */
  fprintf (conf, "\n# Authoritative\n");
  str = i_configfile_get (self, DHCPCONF_FILE, "dhcp", "authoritative", 0);
  if (str && !strcasecmp (str, "yes"))
  { fprintf (conf, "authoritative;\n"); }
  if (str) free (str);
  
  /* Logging config */
  fprintf (conf, "\n# Logging config\n");
  str = i_configfile_get (self, DHCPCONF_FILE, "dhcp", "log-facility", 0);
  if (str)
  { fprintf (conf, "log-facility %s;\n", str); free (str); }
  else
  { fprintf (conf, "log-facility local0;\n"); }

  /* DDNS Update */
  fprintf (conf, "\n# DDNS Update\n");
  str = i_configfile_get (self, DHCPCONF_FILE, "dhcp", "ddns-update-style", 0);
  if (str)
  { fprintf (conf, "ddns-update-style %s;\n", str); free (str); }
  else
  { fprintf (conf, "ddns-update-style none;\n"); }
  
  /* Loop through each subnet */
  for (i_list_move_head(subnet_list); (subnet=i_list_restore(subnet_list))!=NULL; i_list_move_next(subnet_list))
  {
    /* Process subnet */
    l_dhcp_range *range;

    /* Start subnet def */
    fprintf (conf, "\n# %s\n", subnet->site->name_str);
    fprintf (conf, "subnet %s netmask %s\n{\n", subnet->network_str, subnet->mask_str); 

    /* Loop through each range */
    for (i_list_move_head(subnet->range_list); (range=i_list_restore(subnet->range_list))!=NULL; i_list_move_next(subnet->range_list))
    {
      /* Process range */
      fprintf (conf, "\trange %s %s;\n", range->start_str, range->end_str); 
    }

    /* Router */
    if (subnet->router_str)
    { fprintf (conf, "\toption routers %s;\n", subnet->router_str); }    

    /* Domain Name */
    if (subnet->domain_str)
    { fprintf (conf, "\toption domain-name \"%s\";\n", subnet->domain_str); }    

    /* Options */
    if (subnet->options_str)
    { fprintf (conf, "\t# Options\n%s\n\t# End options\n", subnet->options_str); }

    /* End subnet def */
    fprintf (conf, "}\n"); 

    /* End subnet processing */
  }

  /* Close config file */
  fflush (conf);
  fclose (conf);

  return 0;
}
