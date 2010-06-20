#include <stdio.h>
#include <stdlib.h>

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
#include "sms.h"

/* Struct manipulation */

l_sms_send_cbdata* l_sms_send_cbdata_create ()
{
  l_sms_send_cbdata *cbdata;

  cbdata = (l_sms_send_cbdata *) malloc (sizeof(l_sms_send_cbdata));
  if (!cbdata)
  { i_printf (1, "l_sms_send_cbdata_create failed to malloc cbdata struct"); return NULL; }
  memset (cbdata, 0, sizeof(l_sms_send_cbdata));

  return cbdata;
}

void l_sms_send_cbdata_free (void *cbdataptr)
{
  l_sms_send_cbdata *cbdata = cbdataptr;

  if (!cbdata) return;
  if (cbdata->number) free (cbdata->number);
  if (cbdata->message) free (cbdata->message);

  free (cbdata);
}

/* SMS Sending */

int l_sms_send (i_resource *self, l_gsmdevice *device, char *number, char *message)
{
  int num;
  l_sms_send_cbdata *cbdata;

  if (!self || !number || !message) return -1;

  /* Create/Enqueue cbdata */
  
  cbdata = l_sms_send_cbdata_create ();
  if (!cbdata) 
  { i_printf (1, "l_sms_send failed to create cbdata struct"); return -1; }
  cbdata->number = strdup (number);
  cbdata->message = strdup (message);
  cbdata->device = device;

  num = i_list_enqueue (device->smsq, cbdata);
  if (num != 0)
  { i_printf (1, "l_sms_send failed to enqueue message to device->smsq"); l_sms_send_cbdata_free (cbdata); return -1; }

  /* Check/Set State */
  
  if (device->state != GSM_STATE_READY)
  { return 0; } /* Device is busy, return now, message will remain in the smsq */

  device->state = GSM_STATE_BUSY;

  /* Send next */
  
  num = l_sms_send_next (self, device);
  if (num != 0)
  { i_printf (1, "l_sms_send failed to call l_sms_send_next"); l_sms_send_failed (self, cbdata); return -1; }

  return 0;
}

int l_sms_send_next (i_resource *self, l_gsmdevice *device)
{ 
  int num;
  l_sms_send_cbdata *cbdata;

  if (!device->smsq || device->smsq->size < 1)
  { i_printf (1, "l_sms_send_next failed, device->smsq is empty or non-existant"); return -1; }
  
  i_list_move_head (device->smsq);
  cbdata = i_list_restore (device->smsq);
  if (!cbdata)
  { i_printf (1, "l_sms_send_next failed to retrieve first queued cbdata (device->smsq->size == %i)", device->smsq->size); return -1; }
  
  num = l_atcommand_exec (self, device, "AT+CMGF=1");
  if (num != 0)
  { i_printf (1, "l_sms_send_next failed to send AT+CMGF=1"); return -1; }

  num = l_atcommand_readln_expect (self, device, "OK", l_sms_send_msgmode_callback, cbdata);
  if (num != 0)
  { i_printf (1, "l_sms_send_next failed to recieve response from AT+CMGF=1"); return -1; }
  
  return 0;
}

int l_sms_send_msgmode_callback (i_resource *self, char *line, void *passdata)
{
  int num;
  char *command;
  l_sms_send_cbdata *cbdata = passdata;
  
  if (!line || strstr(line, "ERROR"))
  { i_printf (1, "l_sms_send_msgmode_callback recieved ERROR response from AT+CMGF=1 (%s)", line); l_sms_send_failed (self, cbdata); return -1; }

  asprintf (&command, "AT+CMGS=%s\n%s %c", cbdata->number, cbdata->message, 26);    /* 26 == Ctrl-Z */
  num = l_atcommand_exec (self, cbdata->device, command);
  free (command);
  if (num != 0)
  { i_printf (1, "l_sms_send_msgmode_callback failed to execute AT+CMGS"); l_sms_send_failed (self, cbdata); return -1; }

  num = l_atcommand_readln_expect (self, cbdata->device, "CMGS", l_sms_send_cmgs_callback, cbdata);
  if (num != 0)
  { i_printf (1, "l_sms_send_msgmode_callback failed to recieve CMGS response to AT+CMGS"); l_sms_send_failed (self, cbdata); return -1; }

  return 0;
}

int l_sms_send_cmgs_callback (i_resource *self, char *line, void *passdata)
{
  int num;
  l_sms_send_cbdata *cbdata = passdata;

  if (!line || strstr(line, "ERROR"))
  { i_printf (1, "l_sms_send_cmgs_callback recieved ERROR response from AT+CMGS (%s)", line); l_sms_send_failed (self, cbdata); return -1; }
  
  num = l_atcommand_readln_expect (self, cbdata->device, "OK", l_sms_send_sendok_callback, cbdata);
  if (num != 0)
  { i_printf (1, "l_sms_send_cmgs_callback failed to recieve OK response to AT+CMGS"); l_sms_send_failed (self, cbdata); return -1; }

  return 0;
}

int l_sms_send_sendok_callback (i_resource *self, char *line, void *passdata)
{
  int num;
  l_sms_send_cbdata *cbdata = passdata;
  
  if (!line || strstr(line, "ERROR"))
  { i_printf (1, "l_sms_send_sendok_callback recieved error from AT+CMGS command"); l_sms_send_failed (self, cbdata); return -1; }

  num = i_list_search (cbdata->device->smsq, cbdata);
  if (num == 0)
  { i_list_delete (cbdata->device->smsq); }

  return 0;
}

int l_sms_send_failed (i_resource *self, l_sms_send_cbdata *cbdata)
{
  /* Initialise GSM hardware. Dont free the current cbdata, 
   * it will remain in the smsq and will be the first to be
   * send when the GSM hardware is reinitialised
   */

  l_gsmdevice_init (self, cbdata->device);

  return 0;
}
