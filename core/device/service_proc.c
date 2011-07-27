#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/callback.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/list.h>
#include <induction/postgresql.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/incident.h>
#include <induction/xml.h>

#include "service.h"
#include "config.h"

#define PIPE_TIMEOUT_SECONDS 120

/* Process Struct Manipulation */

l_service_proc* l_service_proc_create ()
{
  l_service_proc *proc;

  proc = (l_service_proc *) malloc (sizeof(l_service_proc));
  if (!proc)
  { i_printf (1, "l_service_proc_create failed to malloc l_service_proc struct"); return NULL; }
  memset (proc, 0, sizeof(l_service_proc));

  return proc;
}

void l_service_proc_free (void *procptr)
{
  l_service_proc *proc = procptr;

  if (!proc) return;

  if (proc->pid > 0) kill (proc->pid, SIGKILL);
  if (proc->readfd > 0) close (proc->readfd);
  if (proc->writefd > 0) close (proc->writefd);
  if (proc->sockcb) i_socket_callback_remove (proc->sockcb);
  if (proc->sock) { proc->sock->sockfd = -1; i_socket_free (proc->sock); }
  if (proc->timeout_timer) i_timer_remove (proc->timeout_timer);
  if (proc->script_file) free (proc->script_file);
  if (proc->output_str) free (proc->output_str);
  if (proc->command_str) free (proc->command_str);
  if (proc->cb) i_callback_free (proc->cb);
  if (proc->temp_config_file)
  {
    unlink (proc->temp_config_file);
    free (proc->temp_config_file);
  }

  free (proc);
}

/* Script Execution */

l_service_proc* l_service_exec (i_resource *self, char *script_file, char *command_str, l_service *service, int (*cbfunc) (), void *passdata)
{
  l_service_proc *proc;

  /* Create process struct */
  proc = l_service_proc_create ();
  if (!proc)
  { i_printf (1, "l_service_exec failed to create proc struct"); return NULL; }
  proc->script_file = strdup (script_file);
  proc->command_str = strdup (command_str);
  if (service)
  { proc->serviceid = service->id; }

  /* Add callback if specified */
  if (cbfunc)
  {
    proc->cb = i_callback_create ();
    if (!proc->cb)
    { i_printf (1, "l_service_exec failed to create proc->cb"); l_service_proc_free (proc); return NULL; }
    proc->cb->func = cbfunc;
    proc->cb->passdata = passdata;
  }

  /* Load config variables */
  if (service)
  {    
    proc->configvar_cb = l_service_sql_configvar_load_list (self, service->id, l_service_exec_configvar_cb, proc);
    if (!proc->configvar_cb)
    { i_printf (1, "l_service_exec failed to load config variables for service %li", service->id); return NULL; }
  }
  else
  {
    /* No need to load config variables, go straight to script
     * execution
     */
    l_service_exec_configvar_cb (self, NULL, proc);
  }

  return proc;
}

int l_service_exec_configvar_cb (i_resource *self, i_list *list, void *passdata)
{
  /* Called when the list of config variables
   * has been loaded or if there is no config 
   * variables to load.
   *
   * Returns -1 to ensure list is disposed of 
   *
   * This function calls 'sh' to execute the script
   */
  int num;
  int fd1[2];
  int fd2[2];
  l_service_proc *proc = passdata;

  /* Write config out to file */
  if (list)
  {
    /* Set temp filename */
    struct timeval now;
    gettimeofday (&now, NULL);
    char *prefix;
    asprintf (&prefix, "service-%li-%i-", now.tv_sec, (int)now.tv_usec);
    proc->temp_config_file = tempnam(NULL, prefix);
    free (prefix);
    
    /* Create XML */
    i_xml *xml = i_xml_create ();
    xml->doc = xmlNewDoc (BAD_CAST "1.0");
    xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "config_variables");
    xmlDocSetRootElement (xml->doc, root_node);
    l_service_configvar *var;
    for (i_list_move_head(list); (var=i_list_restore(list))!=NULL; i_list_move_next(list))
    {
      xmlNodePtr varNode = xmlNewNode (NULL, BAD_CAST "variable");
      xmlNewChild (varNode, NULL, BAD_CAST "name", BAD_CAST var->name_str);
      xmlNewChild (varNode, NULL, BAD_CAST "value", BAD_CAST var->value_str);
      xmlAddChild (root_node, varNode);
    }

    /* 
     * Add device config info
     */
    xmlNodePtr devNode;

    /* Device Name */
    devNode = xmlNewNode (NULL, BAD_CAST "variable");
    xmlNewChild (devNode, NULL, BAD_CAST "name", BAD_CAST "dev_name");
    xmlNewChild (devNode, NULL, BAD_CAST "value", BAD_CAST self->hierarchy->dev->name_str);
    xmlAddChild (root_node, devNode);

    /* Device Desc */
    devNode = xmlNewNode (NULL, BAD_CAST "variable");
    xmlNewChild (devNode, NULL, BAD_CAST "name", BAD_CAST "dev_desc");
    xmlNewChild (devNode, NULL, BAD_CAST "value", BAD_CAST self->hierarchy->dev->desc_str);
    xmlAddChild (root_node, devNode);
    
    /* Device IP */
    devNode = xmlNewNode (NULL, BAD_CAST "variable");
    xmlNewChild (devNode, NULL, BAD_CAST "name", BAD_CAST "dev_ip");
    xmlNewChild (devNode, NULL, BAD_CAST "value", BAD_CAST self->hierarchy->dev->ip_str);
    xmlAddChild (root_node, devNode);

    /* Device Community */
    devNode = xmlNewNode (NULL, BAD_CAST "variable");
    xmlNewChild (devNode, NULL, BAD_CAST "name", BAD_CAST "dev_community");
    xmlNewChild (devNode, NULL, BAD_CAST "value", BAD_CAST self->hierarchy->dev->snmpcomm_str);
    xmlAddChild (root_node, devNode);

    /* Device Username */
    devNode = xmlNewNode (NULL, BAD_CAST "variable");
    xmlNewChild (devNode, NULL, BAD_CAST "name", BAD_CAST "dev_username");
    xmlNewChild (devNode, NULL, BAD_CAST "value", BAD_CAST self->hierarchy->dev->username_str);
    xmlAddChild (root_node, devNode);

    /* Device Password */
    devNode = xmlNewNode (NULL, BAD_CAST "variable");
    xmlNewChild (devNode, NULL, BAD_CAST "name", BAD_CAST "dev_password");
    xmlNewChild (devNode, NULL, BAD_CAST "value", BAD_CAST self->hierarchy->dev->password_str);
    xmlAddChild (root_node, devNode);

    /* Device Profile */
    devNode = xmlNewNode (NULL, BAD_CAST "variable");
    xmlNewChild (devNode, NULL, BAD_CAST "name", BAD_CAST "dev_profile");
    xmlNewChild (devNode, NULL, BAD_CAST "value", BAD_CAST self->hierarchy->dev->profile_str);
    xmlAddChild (root_node, devNode);

    /* Write XML to temp file */
    xmlSaveFile (proc->temp_config_file, xml->doc);
    i_xml_free (xml);
  }
  
  /* Create pipes */
  num = pipe (fd1);
  if (num < 0)
  { i_printf (1, "l_service_exec failed to call pipe for fd1"); l_service_proc_free (proc); return -1; }
  proc->writefd = fd1[1];
  num = pipe (fd2);
  if (num < 0)
  { i_printf (1, "l_service_exec failed to call pipe for fd2"); l_service_proc_free (proc); return -1; }
  proc->readfd = fd2[0];

  /* Fork */
  proc->pid = fork ();
  if (proc->pid < 0)
  { i_printf (1, "l_service_exec failed to call fork()"); l_service_proc_free (proc); return -1; }

  if (proc->pid > 0)
  {
    /* Parent Process */
    close(fd1[0]);
    close(fd2[1]);

    /* Create 'socket' struct for the readfd */
    proc->sock = i_socket_create ();
    if (!proc->sock)
    { i_printf (1, "l_service_exec failed to create blank socket"); l_service_proc_free (proc); return -1; }
    proc->sock->sockfd = proc->readfd;
    fcntl (proc->readfd, F_SETFL, O_NONBLOCK);

    /* Install readfd socket callback */
    proc->sockcb = i_socket_callback_add (self, SOCKET_CALLBACK_READ, proc->sock, l_service_exec_socketcb, proc);
    if (!proc->sockcb)
    { i_printf (1, "l_service_exec failed to add socket callback"); l_service_proc_free (proc); return -1; }

    /* Install timeout */
    proc->timeout_timer = i_timer_add (self, PIPE_TIMEOUT_SECONDS, 0, l_service_exec_timeoutcb, proc);
    if (!proc->timeout_timer)
    { i_printf (1, "l_service_exec failed to add timeout callback"); l_service_proc_free (proc); return -1; }
  }
  else
  {
    /* Action Child */
    close(fd1[1]);
    close(fd2[0]);
    
    if (fd1[0] != STDIN_FILENO)
    {
      if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
        fprintf (stdout, "dup2 error to stdin");
      close(fd1[0]);
    } 
    if (fd2[1] != STDOUT_FILENO)
    {
      if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
        fprintf (stdout, "dup2 error to stdout");
      close(fd2[1]);
    } 

    /* Create file path */
    char *fullpath;
    if (self->type == RES_DEVICE)
    { asprintf (&fullpath, "%s/../../service_scripts/%s", self->root, proc->script_file); }
    else if (self->type == RES_CUSTOMER)
    { asprintf (&fullpath, "%s/service_scripts/%s", self->root, proc->script_file); }

    /* Exec */
#ifdef OS_DARWIN
    char *perlenv = "PERL5LIB=/Library/Lithium/LithiumCore.app/Contents/Resources/Perl";    
#else
    char *perlenv = "";    
#endif
    if (proc->temp_config_file && strlen(proc->temp_config_file) > 0)
    { num = execlp ("env", "env", perlenv, fullpath, proc->command_str, proc->temp_config_file, NULL); }
    else
    { num = execlp ("/usr/local/perl5.12/bin/perl", "perl", "-X", fullpath, proc->command_str, NULL); }
    if (num == -1)
    {
      fprintf (stdout, "execlp error");
    } 
    
    exit (1);
  } 
  
  return -1;    /* Return -1 to ensure configvar list is disposed of */
}

int l_service_exec_socketcb (i_resource *self, i_socket *sock, void *passdata)
{
  /* Called when there is something to read from the 
   * script child process.
   *
   * Only return -1 when the socket callback is to be
   * removed.
   * Return 0 to keep the callback active
   */

  int num;
  size_t readcount;
  char *output;
  l_service_proc *proc = passdata;

  /* Read response */
  output = (char *) malloc (1024);
  readcount = read (sock->sockfd, output, 1023);
  if (readcount == 1023)
  {
    /* There could be more to be read */
    char temp_buf[1024];
    num = readcount;
    while (num == 1023)
    {
      num = read (sock->sockfd, temp_buf, 1023);
      output = realloc (output, readcount + num + 1);
      memcpy (output + readcount, temp_buf, num);
      readcount += num;
    }
  }

  /* NULL Terminate output string */ 
  output[readcount] = '\0';

  /* Check result */
  if (readcount < 1)
  { 
    /* Failed to read from child, command must be complete */

    /* Run callback */
    if (proc->cb && proc->cb->func)
    { proc->cb->func (self, proc, proc->cb->passdata); }

    /* Log it */
    l_service_sql_log_insert (self, proc->serviceid, proc->output_str);
    
    /* Cleanup */
    l_service_proc_free (proc);
  }
  else
  {
    /* Append output to command */
    if (proc->output_str)
    {
      /* Append */
      int oldsize = strlen (proc->output_str);
      int newsize = oldsize + readcount + 1;
      proc->output_str = realloc (proc->output_str, newsize);
      memcpy (proc->output_str+oldsize, output, readcount);
      proc->output_str[newsize-1] = '\0';
    }
    else
    {
      /* New string */
      proc->output_str = strdup (output);
    }
  }

  /* Cleanup */
  free (output);

  return 0;
}

int l_service_exec_timeoutcb (i_resource *self, i_timer *timer, void *passdata)
{
  l_service_proc *proc = passdata;

  /* NULL Timer */
  proc->timeout_timer = NULL;

  /* Execute callback for current command */
  if (proc->cb && proc->cb->func)
  { 
    /* Run callback */
    proc->cb->func (self, proc, proc->cb->passdata); 
    
    /* Log it */
    char *str;
    asprintf (&str, "%s SCRIPT_TIMEOUT", proc->output_str);
    l_service_sql_log_insert (self, proc->serviceid, str);
  }

  /* Terminate the current process */    
  l_service_proc_free (proc);

  return -1;    /* Don't keep timer alive */
}


