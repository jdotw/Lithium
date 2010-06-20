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
#include "gsmdevice_socket.h"
#include "sms_config.h"
#include "atcommand.h"
#include "sigstrength.h"
#include "sms.h"

static l_gsmdevice *static_gsmdevice = NULL;
static i_timer *static_retry_timer = NULL;

/* Struct Manipulation */

l_gsmdevice *l_gsmdevice_create ()
{
  l_gsmdevice *device;

  device = (l_gsmdevice *) malloc (sizeof(l_gsmdevice));
  if (!device)
  { i_printf (1, "l_gsmdevice_create failed to malloc device struct"); return NULL; }
  memset (device, 0, sizeof(l_gsmdevice));

  device->smsq = i_list_create ();
  if (!device->smsq)
  { i_printf (1, "l_gsmdevice_create failed to create device->smsq list"); l_gsmdevice_free (device); return NULL; }
  i_list_set_destructor (device->smsq, l_sms_send_cbdata_free);

  return device;
}

void l_gsmdevice_free (void *deviceptr)
{
  l_gsmdevice *device = deviceptr;

  if (!device) return;
  if (device->socket) i_socket_free (device->socket);
  if (device->smsq) i_list_free (device->smsq);

  free (device);
}

/* Pointer fetching */

l_gsmdevice* l_gsmdevice_device ()
{ return static_gsmdevice; }

/* Device Initialisation */

int l_gsmdevice_init (i_resource *self, l_gsmdevice *device)
{
  /* Initialise the SMS hardware 
   *
   * If a failure occurs, l_gsmdevice_init_failed is called and
   * a retry timer is installed. Hence, even failures return 0 
   * because it will be auto-retried
   */

  int num;

  /* Set/Check State */

  static_gsmdevice = device;

  if (device->state == GSM_STATE_INIT)
  { return 0; }
    
  if (device->socket)
  { i_socket_free (device->socket); device->socket = NULL; }

  device->state = GSM_STATE_INIT;
  
  device->socket = l_gsmdevice_socket_open (self);
  if (!device->socket)
  { i_printf (1, "l_gsmdevice_init failed to open SMS hardware device"); l_gsmdevice_init_failed (self, device); return 0; }

  /* Load config */
  
  /* Reset and Disable echo */

  num = l_atcommand_exec (self, device, "ATZ;E0");
  if (num != 0)
  { i_printf (1, "l_gsmdevice_init failed to execute initialisation string"); l_gsmdevice_init_failed (self, device); return 0; }
  
  num = l_atcommand_readln_expect (self, device, "OK", l_gsmdevice_init_reset_callback, device);
  if (num != 0)
  { i_printf (1, "l_gsmdevice_init failed to call l_atcommand_readln_expect for reset command result"); l_gsmdevice_init_failed (self, device); return 0; }

  return 0;
}

int l_gsmdevice_init_reset_callback (i_resource *self, char *line, void *passdata)
{
  /* Called when the result of the reset AT command
   * is received from the GSM hardware
   */
  
  int num;
  char *command;
  l_sms_config *config;
  l_gsmdevice *device = passdata;

  if (!line)
  {
    i_printf (1, "l_gsmdevice_init_reset_callback a failure occurred during execution of the reset command");
    l_gsmdevice_init_failed (self, device); 
    return 0; 
  }

  config = l_sms_config_file_to_struct (self);
  if (!config)
  { i_printf (1, "l_gsmdevice_init failed to read in config"); l_gsmdevice_init_failed (self, device); return 0; }


  if (config->pin_str)
  {
    /* Send the PIN
     *
     * This will return an ERROR if the PIN was already sent and
     * the modem is already registered. Hence the error checking
     * done here is minimal.
     */

    asprintf (&command, "AT+CPIN=\"%s\"", config->pin_str);
    l_sms_config_free (config);

    num = l_atcommand_exec (self, device, command);
    free (command);
    if (num != 0)
    { i_printf (1, "l_gsmdevice_init failed to send PIN number to GSM modem"); l_gsmdevice_init_failed (self, device); return -1; }

    num = l_atcommand_readln_expect (self, device, "OK", l_gsmdevice_init_pin_callback, device);
    if (num != 0)
    { i_printf (1, "l_gsmdevice_init failed to call l_atcommand_readln_expect for PIN command result"); l_gsmdevice_init_failed (self, device); return -1; }
  }
  else
  {
    /* Skip sending of the PIN, call the
     * pin callback function as if sending of the PIN
     * was successful
     */

    l_sms_config_free (config);
    l_gsmdevice_init_pin_callback (self, line, device);
  }

  return 0;
}

int l_gsmdevice_init_pin_callback (i_resource *self, char *line, void *passdata)
{
  int num;
  l_gsmdevice *device = passdata;

  if (!line)
  {
    i_printf (1, "l_gsmdevice_init_pin_callback a failure occurred during execution of the PIN command");
    l_gsmdevice_init_failed (self, device); 
    return 0; 
  }

  /* Check GSM network registration */

  num = l_atcommand_exec (self, device, "AT+CREG?");
  if (num != 0)
  { i_printf (1, "l_gsmdevice_init_pin_callback failed to check GSM network registration"); l_gsmdevice_init_failed (self, device); return -1; }

  num = l_atcommand_readln_expect (self, device, "CREG", l_gsmdevice_init_registration_callback, device);
  if (num != 0)
  { i_printf (1, "l_gsmdevice_init_pin_callback failed to call l_atcommand_readln_expect for registration command result"); l_gsmdevice_init_failed (self, device); return -1; }

  return 0;
}

int l_gsmdevice_init_registration_callback (i_resource *self, char *line, void *passdata)
{
  int status;
  char *strptr;
  l_gsmdevice *device = passdata;

  if (!line)
  {
    i_printf (1, "l_gsmdevice_init_registration_callback a failure occurred during execution of the registration command");
    l_gsmdevice_init_failed (self, device);
    return 0;
  }

  strptr = strchr (line, ',');
  if (!strptr)
  { i_printf (1, "l_gsmdevice_init_registration_callback failed to interpret registration response"); l_gsmdevice_init_failed (self, device); return -1; }
  strptr++;
  status = atoi (strptr);

  if (status != 1)
  { i_printf (1, "l_gsmdevice_init_registration_callback GSM modem failed to register with home network"); l_gsmdevice_init_failed (self, device); return -1; }

  /* Signal strength checker */

  l_sigstrength_timer_callback (self, NULL, device);     /* Initial run */
  device->sigstrength_timer = i_timer_add (self, 300, 0, l_sigstrength_timer_callback, device);

  /* Set state */

  device->state = GSM_STATE_READY;

  /* Check smsq */

  if (device->smsq && device->smsq->size > 0)
  { l_sms_send_next (self, device); }
  
  return 0;
} 

/* Failure Handling */

int l_gsmdevice_init_failed (i_resource *self, l_gsmdevice *device)
{
  if (static_retry_timer) 
  { i_timer_remove (static_retry_timer); }
  
  static_retry_timer = i_timer_add (self, DEFAULT_RETRY_SECONDS, 0, l_gsmdevice_init_retry_callback, device);
  if (!static_retry_timer)
  { i_printf (1, "l_gsmdevice_init_failed failed to add retry timer"); return -1; }

  return 0;
}

int l_gsmdevice_init_retry_callback (i_resource *self, i_timer *timer, void *passdata)
{
  l_gsmdevice *device = passdata;

  static_retry_timer = NULL;
  l_gsmdevice_init (self, device);

  return -1;
}

