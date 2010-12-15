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
#include <induction/hashtable.h>
#include <induction/list.h>
#include <induction/postgresql.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/customer.h>
#include <induction/device.h>
#include <induction/incident.h>
#include <induction/xml.h>

#include "case.h"
#include "incident.h"
#include "action.h"
#include "config.h"

#define PIPE_TIMEOUT_SECONDS 120

/* Process Struct Manipulation */

l_action_proc* l_action_proc_create ()
{
  l_action_proc *proc;

  proc = (l_action_proc *) malloc (sizeof(l_action_proc));
  if (!proc)
  { i_printf (1, "l_action_proc_create failed to malloc l_action_proc struct"); return NULL; }
  memset (proc, 0, sizeof(l_action_proc));

  return proc;
}

void l_action_proc_free (void *procptr)
{
  l_action_proc *proc = procptr;

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
  if (proc->entaddr_str) free (proc->entaddr_str);
  if (proc->cust_desc) free (proc->cust_desc);
  if (proc->site_desc) free (proc->site_desc);
  if (proc->dev_desc) free (proc->dev_desc);
  if (proc->cnt_desc) free (proc->cnt_desc);
  if (proc->obj_desc) free (proc->obj_desc);
  if (proc->met_desc) free (proc->met_desc);
  if (proc->trg_desc) free (proc->trg_desc);
  if (proc->prev_trg_desc) free (proc->prev_trg_desc);
  if (proc->entity_url) free (proc->entity_url);
  if (proc->metric_url) free (proc->metric_url);
  if (proc->incident_url) free (proc->incident_url);
  if (proc->entdesc) i_entity_descriptor_free (proc->entdesc);
  if (proc->cb) i_callback_free (proc->cb);
  if (proc->configvar_cb) proc->configvar_cb->func = NULL;
  if (proc->temp_config_file) 
  {
    unlink (proc->temp_config_file);
    free (proc->temp_config_file);
  }

  free (proc);
}

/* Script Execution */

l_action_proc* l_action_exec (i_resource *self, char *script_file, char *command_str, l_action *action, i_incident *inc, int (*cbfunc) (), void *passdata)
{
  l_action_proc *proc;

  /* Create process struct */
  proc = l_action_proc_create ();
  if (!proc)
  { i_printf (1, "l_action_exec failed to create proc struct"); return NULL; }
  proc->script_file = strdup (script_file);
  proc->command_str = strdup (command_str);
  if (inc)
  {
    proc->incid = inc->id;
    proc->entaddr_str = i_entity_address_string (NULL, (i_entity_address *)inc->ent);
    proc->entdesc = i_entity_descriptor_duplicate (inc->ent);
    if (strstr(command_str, "clear")) proc->opstate = 0;
    else proc->opstate = inc->ent->opstate;
    proc->cust_desc = strdup (inc->ent->cust_desc);
    proc->site_desc = strdup (inc->ent->site_desc);
    proc->dev_desc = strdup (inc->ent->dev_desc);
    proc->cnt_desc = strdup (inc->ent->cnt_desc);
    proc->obj_desc = strdup (inc->ent->obj_desc);
    proc->met_desc = strdup (inc->ent->met_desc);
    proc->trg_desc = strdup (inc->ent->trg_desc);
    proc->start_sec = inc->start_tv.tv_sec;
    proc->end_sec = inc->end_tv.tv_sec;
    proc->highest_opstate = inc->highest_opstate;
    proc->lowest_opstate = inc->lowest_opstate;
    proc->last_transition_sec = inc->last_transition_tv.tv_sec;
    proc->last_occurrence_sec = inc->last_occurrence_tv.tv_sec;
    proc->occurrences = inc->occurrences;
    if (inc->prev_ent)
    {
      proc->prev_trg_desc = strdup (inc->prev_ent->trg_desc);
      proc->prev_opstate = inc->prev_ent->opstate;
    }
    proc->entity_url = i_entity_address_url (NULL, (i_entity_address *)inc->ent);
    if (inc->met) proc->metric_url = i_entity_address_url (NULL, (i_entity_address *)inc->met);
    i_list *inc_list = l_incident_list();
    proc->incident_count = inc_list->size;
    proc->incident_url = i_incident_url (inc);
  }
  if (action)
  {
    proc->log_output = action->log_output;
    proc->run_count = action->run_count;
    action->run_count++;
    proc->actionid = action->id;
  }

  /* Add callback if specified */
  if (cbfunc)
  {
    proc->cb = i_callback_create ();
    if (!proc->cb)
    { i_printf (1, "l_action_exec failed to create proc->cb"); l_action_proc_free (proc); return NULL; }
    proc->cb->func = cbfunc;
    proc->cb->passdata = passdata;
  }

  /* Load config variables */
  if (action)
  {    
    proc->configvar_cb = l_action_sql_configvar_load_list (self, action->id, l_action_exec_configvar_cb, proc);
    if (!proc->configvar_cb)
    { i_printf (1, "l_action_exec failed to load config variables for action %li", action->id); return NULL; }
  }
  else
  {
    /* No need to load config variables, go straight to script
     * execution
     */
    l_action_exec_configvar_cb (self, NULL, proc);
  }

  return proc;
}

int l_action_exec_configvar_cb (i_resource *self, i_list *list, void *passdata)
{
  /* Called when the list of config variables
   * has been loaded or if there is no config 
   * variables to load.
   *
   * This function calls 'sh' to execute the script
   *
   * Alway Return -1 to free the variable list!!
   */
  int num;
  int fd1[2];
  int fd2[2];
  l_action_proc *proc = passdata;

  /* Un-set configvar_cb */
  proc->configvar_cb = NULL;

  /* Write config out to file */
  if (list)
  {
    /* Set temp filename */
    struct timeval now;
    gettimeofday (&now, NULL);
    char *prefix;
    asprintf (&prefix, "action-%li-%li-%li-", proc->incid, now.tv_sec, now.tv_usec);
    proc->temp_config_file = tempnam(NULL, prefix);
    free (prefix);
    
    /* Create XML */
    i_xml *xml = i_xml_create ();
    xml->doc = xmlNewDoc (BAD_CAST "1.0");
    xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "config_variables");
    xmlDocSetRootElement (xml->doc, root_node);

    /* Add user-configured variables */
    l_action_configvar *var;
    for (i_list_move_head(list); (var=i_list_restore(list))!=NULL; i_list_move_next(list))
    {
      if (var->value_str && strlen(var->value_str) > 0)
      {
        xmlNodePtr varNode = xmlNewNode (NULL, BAD_CAST "variable");
        xmlNewChild (varNode, NULL, BAD_CAST "name", BAD_CAST var->name_str);
        xmlNewChild (varNode, NULL, BAD_CAST "value", BAD_CAST var->value_str);
        xmlAddChild (root_node, varNode);
      }
    }
    
    /* Add Defaults */
    if (self->hierarchy->cust->baseurl_str && strlen(self->hierarchy->cust->baseurl_str) > 0)
    {
      /* Check for configured baseurl */
      xmlNodePtr varNode = xmlNewNode (NULL, BAD_CAST "variable");
      xmlNewChild (varNode, NULL, BAD_CAST "name", BAD_CAST "customer_url");
      xmlNewChild (varNode, NULL, BAD_CAST "value", BAD_CAST self->hierarchy->cust->baseurl_str);
      xmlAddChild (root_node, varNode);
    }
    else 
    {
      /* Attempt to use hostname */
      char hostname[256];
      gethostname (hostname, 255);
      xmlNodePtr varNode = xmlNewNode (NULL, BAD_CAST "variable");
      xmlNewChild (varNode, NULL, BAD_CAST "name", BAD_CAST "customer_url");
      xmlNewChild (varNode, NULL, BAD_CAST "value", BAD_CAST hostname);
      xmlAddChild (root_node, varNode);
    }

    /* Write to file */
    xmlSaveFile (proc->temp_config_file, xml->doc);
    i_xml_free (xml);

    /* Read and log the config file is logging is enabled */
    if (proc->log_output)
    {
      int config_fd = open(proc->temp_config_file, O_RDONLY);
      if (config_fd > 0)
      {
        off_t file_size = lseek(config_fd, 0, SEEK_END);
        if (file_size > 0)
        {
          char *file_text = malloc(file_size);
          lseek(config_fd, 0, SEEK_SET);
          ssize_t bytes_read = read(config_fd, file_text, file_size);
          if (bytes_read > 0)
          {
            file_text[bytes_read] = '\0';
            i_printf (1, "l_action_exec (action output logging enabled) using config file: '%s'", file_text);
          }
          free (file_text);
        }
        close (config_fd);
      }
    }
  }
  
  /* Create pipes */
  num = pipe (fd1);
  if (num < 0)
  { i_printf (1, "l_action_exec failed to call pipe for fd1"); l_action_proc_free (proc); return -1; }
  proc->writefd = fd1[1];
  num = pipe (fd2);
  if (num < 0)
  { i_printf (1, "l_action_exec failed to call pipe for fd2"); l_action_proc_free (proc); return -1; }
  proc->readfd = fd2[0];

  /* Fork */
  proc->pid = fork ();
  if (proc->pid < 0)
  { i_printf (1, "l_action_exec failed to call fork()"); l_action_proc_free (proc); return -1; }

  /* Create fill path */
#ifdef OS_DARWIN
  char *perlenv = "PERL5LIB=/Library/Lithium/LithiumCore.app/Contents/Resources/Perl";
#else
  char *perlenv = "PERL5LIB=/usr/local/lib/perl/5.8:/usr/local/share/perl/5.8:/usr/local/lib/perl5:/usr/local/share/perl5";
#endif
  char *fullpath;
  asprintf (&fullpath, "%s/action_scripts/%s", self->root, proc->script_file);

  if (proc->pid > 0)
  {
    /* Parent Process */
    close(fd1[0]);
    close(fd2[1]);
    
    /* Create 'socket' struct for the readfd */
    proc->sock = i_socket_create ();
    if (!proc->sock)
    { i_printf (1, "l_action_exec failed to create blank socket"); l_action_proc_free (proc); return -1; }
    proc->sock->sockfd = proc->readfd;
    fcntl (proc->readfd, F_SETFL, O_NONBLOCK);

    /* Install readfd socket callback */
    proc->sockcb = i_socket_callback_add (self, SOCKET_CALLBACK_READ, proc->sock, l_action_exec_socketcb, proc);
    if (!proc->sockcb)
    { i_printf (1, "l_action_exec failed to add socket callback"); l_action_proc_free (proc); return -1; }

    /* Install timeout */
    proc->timeout_timer = i_timer_add (self, PIPE_TIMEOUT_SECONDS, 0, l_action_exec_timeoutcb, proc);
    if (!proc->timeout_timer)
    { i_printf (1, "l_action_exec failed to add timeout callback"); l_action_proc_free (proc); return -1; }
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

    /* Exec */
    char *shell_command;
    if (proc->entaddr_str)
    {
      /* Incident specified */
      char *incid_str;
      char *opstate_str;
      char *runcount_str;
      char *start_str;
      char *end_str;
      char *highest_opstate_str;
      char *lowest_opstate_str;
      char *prev_opstate_str;
      char *last_transition_str;
      char *last_occurrence_str;
      char *occurrences_str;
      char *inc_count_str;
      char *config_file_str;
      char uuid_str[37];
      uuid_unparse_lower (self->hierarchy->cust->uuid, uuid_str);

      asprintf (&incid_str, "%li", proc->incid);
      asprintf (&opstate_str, "%i", proc->opstate);
      asprintf (&runcount_str, "%i", proc->run_count);
      asprintf (&start_str, "%li", proc->start_sec);
      asprintf (&end_str, "%li", proc->end_sec);
      asprintf (&highest_opstate_str, "%i", proc->highest_opstate);
      asprintf (&lowest_opstate_str, "%i", proc->lowest_opstate);
      asprintf (&prev_opstate_str, "%i", proc->prev_opstate);
      asprintf (&last_transition_str, "%li", proc->last_transition_sec);
      asprintf (&last_occurrence_str, "%li", proc->last_occurrence_sec);
      asprintf (&occurrences_str, "%i", proc->occurrences);
      asprintf (&inc_count_str, "%i", proc->incident_count);
      if (proc->temp_config_file && strlen(proc->temp_config_file) > 0)
      { asprintf(&config_file_str, " '%s'", proc->temp_config_file); }
      else
      { config_file_str = strdup(""); }

      asprintf (&shell_command, "env '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s' '%s'%s 2>&1",
        perlenv, fullpath, proc->command_str, incid_str, proc->entaddr_str, 
        opstate_str, proc->cust_desc, proc->site_desc, proc->dev_desc, proc->cnt_desc, proc->obj_desc, 
        proc->met_desc, proc->trg_desc, runcount_str, start_str, end_str, highest_opstate_str, 
        lowest_opstate_str, prev_opstate_str, proc->prev_trg_desc ? : "N/A", 
        last_transition_str, last_occurrence_str, occurrences_str, 
        proc->entity_url ? : "None", proc->metric_url ? : "None", 
        inc_count_str, proc->incident_url ? : "None", uuid_str,
        config_file_str); 

      free (config_file_str);
      free (incid_str);
      free (opstate_str);
      free (runcount_str);
      free (start_str);
      free (end_str);
      free (highest_opstate_str);
      free (lowest_opstate_str);
      free (prev_opstate_str);
      free (last_transition_str);
      free (last_occurrence_str);
      free (occurrences_str);
      free (inc_count_str);
    } 
    else
    {
      /* No incident */
      asprintf (&shell_command, "env '%s' '%s' '%s'", perlenv, fullpath, proc->command_str);
    }

    if (proc->log_output)
    {
      i_printf (1, "l_action_exec_configvar_cb (action output logging enabled) executing: %s", shell_command);
    }

    num = execlp("sh", "sh", "-c", shell_command, NULL);
    if (num == -1)
    {
      fprintf (stdout, "execlp error");
    } 
    
    exit (1);
  } 

  free (fullpath);

  return -1;      /* Return -1 to destroy variable list */
}

int l_action_exec_socketcb (i_resource *self, i_socket *sock, void *passdata)
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
  l_action_proc *proc = passdata;

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
    
    /* Log output if enabled */
    if (proc->log_output)
    {
      i_printf (1, "l_action_exec_socketcb (action output logging enabled) received '%s'", proc->output_str);
    }

    /* Run callback */
    if (proc->cb && proc->cb->func)
    { proc->cb->func (self, proc, proc->cb->passdata); }

    /* Log it */
    l_action_sql_log_insert (self, proc->actionid, proc->entdesc, proc->output_str);
    
    /* Cleanup */
    l_action_proc_free (proc);
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

int l_action_exec_timeoutcb (i_resource *self, i_timer *timer, void *passdata)
{
  l_action_proc *proc = passdata;

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
    l_action_sql_log_insert (self, proc->actionid, proc->entdesc, str);
  }

  /* Terminate the current process */    
  l_action_proc_free (proc);

  return -1;    /* Don't keep timer alive */
}


