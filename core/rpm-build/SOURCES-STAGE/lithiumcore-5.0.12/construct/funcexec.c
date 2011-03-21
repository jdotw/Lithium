#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>

#include <induction.h>
#include <induction/construct.h>
#include <induction/module.h>
#include <induction/message.h>
#include <induction/socket.h>
#include <induction/msgproc.h>
#include <induction/init.h>

#include "usage.h"
#include "funcexec.h"

/* cbdata struct manipulation */

c_funcexec_cbdata* c_funcexec_cbdata_create ()
{
  c_funcexec_cbdata *cbdata;

  cbdata = (c_funcexec_cbdata *) malloc (sizeof(c_funcexec_cbdata));
  if (!cbdata)
  { i_printf (1, "c_funcexec_cbdata_create failed to malloc cbdata"); return NULL; }
  memset (cbdata, 0, sizeof(c_funcexec_cbdata));

  return cbdata;
}

void c_funcexec_cbdata_free (void *cbdataptr)
{
  c_funcexec_cbdata *cbdata = cbdataptr;

  if (!cbdata) return;

  if (cbdata->module)
  { i_module_free (cbdata->module); }

  free (cbdata);
}

/* Function Execution */

int c_funcexec (i_resource *self, char *node_ip, int argc, char *argv[], int optind)
{
  /* Execute a function in a particular module */

  int num;
  const char *errstr;
  c_funcexec_cbdata *cbdata;

  /* Check state */
  
  if (argc == (optind+1)) { printf ("Function execution specified without a function name\n"); c_usage (); }
  if (!node_ip) { printf ("Function execution specified without a node_ip provided\n"); c_usage(); }

  /* Initialise induction */

  num = i_init (self);
  if (num != 0)
  { printf ("Failed to initialise the induction library\n"); return -1; }

  /* Create cbdata */

  cbdata = c_funcexec_cbdata_create ();
  if (!cbdata)
  { printf ("Failed to create cbdata struct\n"); return -1; }
  cbdata->argc = argc;
  cbdata->argv = argv;
  cbdata->optind = optind;

  /* Open module, find function */
  
  cbdata->module = i_module_open (self->construct->module_path, self->construct->module_name);    /* Open the module */
  if (!cbdata->module) 
  { 
    printf ("Failed to open module %s (path: %s)\n", self->construct->module_name, self->construct->module_path); 
    c_funcexec_cbdata_free (cbdata); 
    return -1; 
  }

  cbdata->function = dlsym (cbdata->module->handle, argv[optind+1]);                  /* Resolve given symbol */
  errstr = dlerror ();
  if (errstr) 
  { printf ("Failed to resolve symbol %s\n", argv[optind+1]); c_funcexec_cbdata_free (cbdata); return -1; }

  /* Connect to client handler */

  self->core_socket = i_socket_create_tcp (self, node_ip, 51106, c_funcexec_connect_callback, cbdata); /* Attempt to connect to the client handler */
  if (!self->core_socket)
  {
    printf ("Failed to connect to Lithium Client Handler (%s:506)", node_ip);
    c_funcexec_cbdata_free (cbdata);
    exit (2);
    return -1;
  }

  return 0;
}

int c_funcexec_connect_callback (i_resource *self, i_socket *sock, void *passdata)
{
  /* Called when the process has connected to 
   * the client handler 
   */

  int num;
  c_funcexec_cbdata *cbdata = passdata;

  if (!sock)
  { printf ("Failed to connect to Lithium Client Handler\n"); exit (2); }

  num = i_msgproc_enable (self, sock);
  if (num != 0)
  { printf ("Failed to enable i_msgproc on socket to Lithium Client Handler\n"); exit (2); }
  i_msgproc_callback_set_default (self, sock, c_funcexec_msgproc_default, NULL); 

  num = cbdata->function (self, cbdata->argc, cbdata->argv, cbdata->optind);      /* Returning 1 will leave i_loop active. 0/-1 exit */
  if (num != 1)
  {
    if (self->core_socket)
    {
      i_message_send (self, MSG_TERMINATE, NULL, 0, NULL, MSG_FLAG_REQ, 0);  /* Signal our termination */
      close (self->core_socket->sockfd);
      i_socket_free (self->core_socket);
    }

    if (num == -1)
    { printf ("Calling of function %s in module %s failed.\n", cbdata->argv[cbdata->optind+1], self->construct->module_name); exit (2); }
    else
    { exit (0); }
  }

  return 0;
}

int c_funcexec_msgproc_default (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  i_printf (1, "c_funcexec_msgproc_default called (%i)", msg->type); 
  return 0;
}
