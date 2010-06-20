#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/hierarchy.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>

#include "gsmdevice.h"
#include "atcommand.h"
#include "sms_config.h"

#define BUF_SIZE 1024

static i_timer *static_write_timer = NULL;

/* Command Execution */

int l_atcommand_exec (i_resource *self, l_gsmdevice *device, char *command_raw)
{
  /* Executes an AT command and returns the result */

  char *command;
  i_socket_data *write_op;

  if (!self || !device || !command_raw) return -1;

  /* Send the command */
  
  asprintf (&command, "%s\n", command_raw);

  write_op = i_socket_write (self, device->socket, command, strlen(command), l_atcommand_exec_write_callback, device);
  free (command);
  if (!write_op)
  { i_printf (1, "l_atcommand_exec failed to send command %s to socket", command_raw); return -1; }

  if (!static_write_timer)
  { 
    l_sms_config *config;

    config = l_sms_config_file_to_struct (self);
    if (!config)
    { i_printf (1, "l_atcommand_exec failed to load config"); i_socket_write_cancel (write_op); return -1; }
  
    static_write_timer = i_timer_add (self, config->gsm_timeout_sec, 0, l_atcommand_exec_timeout_callback, device); 

    l_sms_config_free (config);
  }

  return 0;
}

int l_atcommand_exec_write_callback (i_resource *self, i_socket *sock, int result, void *passdata)
{
  if (result != SOCKET_RESULT_SUCCESS)
  { i_printf (1, "l_atcommand_exec_write_callback error occurred writing a command to the GSM hardware"); }

  if (!sock->write_pending || sock->write_pending->size < 1)
  {
    /* No more pending writes, remove timeout */
    i_timer_remove (static_write_timer);
    static_write_timer = NULL;
  }
  else
  {
    /* Still pending writes, reset timer */
    i_timer_reset (static_write_timer);
  } 

  return 0;
}

int l_atcommand_exec_timeout_callback (i_resource *self, i_timer *timer, void *passdata)
{
  /* Timeout has occurred */
  l_gsmdevice *device = passdata;

  i_printf (1, "l_atcommand_exec_timeout_callback a timeout occurred writing a command to the GSM hardware. Resetting GSM hardware");

  /* Reinitialise GSM hardware, this frees the socket,
   * so all pending writes are taken care of
   */

  l_gsmdevice_init (self, device);
  
  return -1;    /* Remove the timer */
}

/* Readline */

l_atcommand_readln_cbdata* l_atcommand_readln_cbdata_create ()
{
  l_atcommand_readln_cbdata *cbdata;

  cbdata = (l_atcommand_readln_cbdata *) malloc (sizeof(l_atcommand_readln_cbdata));
  if (!cbdata)
  { i_printf (1, "l_atcommand_readln_cbdata_create failed to malloc cbdata struct"); return NULL; }
  memset (cbdata, 0, sizeof(l_atcommand_readln_cbdata));

  return cbdata;
}

void l_atcommand_readln_cbdata_free (void *cbdataptr)
{
  l_atcommand_readln_cbdata *cbdata = cbdataptr;

  if (!cbdata) return;
  if (cbdata->read_op) i_socket_read_cancel (cbdata->read_op);
  if (cbdata->timeout_timer) i_timer_remove (cbdata->timeout_timer);
  if (cbdata->data) free (cbdata->data);

  free (cbdata);
}

int l_atcommand_readln (i_resource *self, l_gsmdevice *device, int (*callback_func) (i_resource *self, char *line, void *passdata), void *passdata)
{
  l_sms_config *config;
  l_atcommand_readln_cbdata *cbdata;

  cbdata = l_atcommand_readln_cbdata_create ();
  if (!cbdata) 
  { i_printf (1, "l_atcommand_readln failed to create cbdata struct"); return -1; }
  cbdata->callback_func = callback_func;
  cbdata->passdata = passdata;
  cbdata->device = device;
  
  /* Start the read operation for the first byte */
  
  cbdata->read_op = i_socket_read (self, device->socket, 1, l_atcommand_readln_read_callback, cbdata);
  if (!cbdata->read_op)
  { i_printf (1, "l_atcommand_readln failed to perform read operation"); l_atcommand_readln_cbdata_free (cbdata); return -1; }

  /* Set the timeout */

  config = l_sms_config_file_to_struct (self);
  if (!config)
  { i_printf (1, "l_atcommand_readln failed to load confif"); l_atcommand_readln_cbdata_free (cbdata); return -1; }
  
  cbdata->timeout_timer = i_timer_add (self, config->gsm_timeout_sec, 0, l_atcommand_readln_timeout_callback, cbdata);
  l_sms_config_free (config);
  if (!cbdata->timeout_timer)
  { i_printf (1, "l_atcommand_readln failed to add timeout timer"); l_atcommand_readln_cbdata_free (cbdata); return -1; }

  return 0;
}

int l_atcommand_readln_read_callback (i_resource *self, i_socket *sock, i_socket_data *data, int result, void *passdata)
{
  /* Called when the read has been completed */

  char *x;
  char *dataptr;
  char *line;
  l_atcommand_readln_cbdata *cbdata = passdata;

  cbdata->read_op = NULL;
  
  if (result != SOCKET_RESULT_SUCCESS)
  {
    i_printf (1, "l_atcommand_readln_read_callback encountered failure reading from GSM hardware. Resetting GSM hardware");
    l_atcommand_readln_failed (self, cbdata);
    l_gsmdevice_init (self, cbdata->device);
    return -1;
  }

  if (cbdata->data)
  {
    /* Existing buffer */
    x = realloc (cbdata->data, cbdata->datasize+1);
    if (!x)
    { 
      i_printf (1, "l_atcommand_readln_read_callback failed to realloc cbdata->data to %i bytes", cbdata->datasize+1);
      l_atcommand_readln_failed (self, cbdata);
      return -1;
    }
    free (cbdata->data);
    cbdata->data = x;
    dataptr = cbdata->data + cbdata->datasize;
    cbdata->datasize++;
  }
  else
  {
    /* New buffer */
    cbdata->data = (char *) malloc (1);
    if (!cbdata->data)
    {
      i_printf (1, "l_atcommand_readln_read_callback failed to alloc cbdata->data (1 byte)");
      l_atcommand_readln_failed (self, cbdata);
      return -1;
    }
    dataptr = cbdata->data;
    cbdata->datasize = 1;
  }

  memcpy (dataptr, data->data, 1);

  if (*dataptr == '\n')
  {
    /* End of line */
    line = (char *) malloc (cbdata->datasize + 1);
    if (!line)
    {
      i_printf (1, "l_atcommand_readln_read_callback failed to alloc line to %i bytes", cbdata->datasize+1);
      l_atcommand_readln_failed (self, cbdata);
      return -1;
    }
    memcpy (line, cbdata->data, cbdata->datasize);
    dataptr = line + cbdata->datasize;
    *dataptr = '\0';

    if (cbdata->callback_func)
    { cbdata->callback_func (self, line, cbdata->passdata); }

    l_atcommand_readln_cbdata_free (cbdata);

    return 0;
  }

  /* Perform next read */

  cbdata->read_op = i_socket_read (self, sock, 1, l_atcommand_readln_read_callback, cbdata);
  if (!cbdata->read_op)
  { 
    i_printf (1, "l_atcommand_readln_read_callback failed to perform next read operation");
    l_atcommand_readln_failed (self, cbdata);
    return -1;
  }

  return 0;
}

int l_atcommand_readln_timeout_callback (i_resource *self, i_timer *timer, void *passdata)
{
  /* ALWAYS RETURN -1 .. non-persistent timer */
  
  l_atcommand_readln_cbdata *cbdata = passdata;

  cbdata->timeout_timer = NULL;

  i_printf (1, "l_atcommand_readline_timeout_callback timeout occurred reading a line from the GSM hardware. Resetting hardware"); 
  l_atcommand_readln_failed (self, cbdata);

  return -1;
}

int l_atcommand_readln_failed (i_resource *self, l_atcommand_readln_cbdata *cbdata)
{
  if (cbdata->callback_func)
  { cbdata->callback_func (self, NULL, cbdata->passdata); }
  
  l_atcommand_readln_cbdata_free (cbdata);

  return 0;
}

/* Readline Expect */

int l_atcommand_readln_expect (i_resource *self, l_gsmdevice *device, char *expect, int (*callback_func) (i_resource *self, char *line, void *passdata), void *passdata)
{
  int num;
  l_atcommand_readln_cbdata *cbdata;

  if (!self || !device || !expect) return -1;
  
  cbdata = l_atcommand_readln_cbdata_create ();
  if (!cbdata)
  { i_printf (1, "l_atcommand_readln_expect failed to create cbdata struct"); return -1; }
  cbdata->callback_func = callback_func;
  cbdata->passdata = passdata;
  cbdata->data = strdup (expect);
  cbdata->device = device;

  num = l_atcommand_readln (self, device, l_atcommand_readln_expect_read_callback, cbdata);
  if (num != 0) 
  { i_printf (1, "l_atcommand_readln_expect failed to call l_atcommand_readln"); l_atcommand_readln_cbdata_free (cbdata); return -1; }

  return 0;
}

int l_atcommand_readln_expect_read_callback (i_resource *self, char *line, void *passdata)
{
  int num;
  l_atcommand_readln_cbdata *cbdata = passdata;

  if (!line || strstr(line, "OK") || strstr(line, "ERROR") || strstr(line, cbdata->data))
  {
    /* Callback-worth result */
    if (cbdata->callback_func)
    { cbdata->callback_func (self, line, cbdata->passdata); }
    l_atcommand_readln_cbdata_free (cbdata);
    return 0;
  }

  /* Read next line */

  num = l_atcommand_readln (self, cbdata->device, l_atcommand_readln_expect_read_callback, cbdata);
  if (num != 0)
  { 
    i_printf (1, "l_atcommand_readln_expect_read_callback failed to perform next readln operation");
    l_atcommand_readln_failed (self, cbdata);
    return -1;
  }

  return 0; 
}

