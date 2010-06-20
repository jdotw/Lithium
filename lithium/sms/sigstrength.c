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
#include "sigstrength.h"

int l_sigstrength_timer_callback (i_resource *self, i_timer *timer, void *passdata)
{
  /* Called regularly to check and record the GSM signal strength 
   *
   * This is a persistent callback, always return 0
   */

  int num;
  l_gsmdevice *device = passdata;

  num = l_atcommand_exec (self, device, "AT+CSQ");
  if (num != 0)
  { i_printf (1, "l_sigstrength_timer_callback failed to execute AT+CSQ command"); return 0; }

  num = l_atcommand_readln_expect (self, device, "OK", l_sigstrength_ok_callback, device);   /* Get OK response */
  if (num != 0)
  { i_printf (1, "l_sigstrength_timer_callback failed to call l_atcommand_readln_expect to read AT+CSQ ok response"); return 0; }

  return 0;
}

int l_sigstrength_ok_callback (i_resource *self, char *line, void *passdata)
{
  /* Called when the 'OK' response to AT+CSQ is recvd */

  int num;
  l_gsmdevice *device = passdata;
  
  num = l_atcommand_readln_expect (self, device, "CSQ", l_sigstrength_reading_callback, device);  /* Get reading */
  if (num != 0)
  { i_printf (1, "l_sigstrength_ok_callback failed to call l_atcommand_readln_expect to read AT+CSQ reading"); return -1; }

  return 0;
}

int l_sigstrength_reading_callback (i_resource *self, char *line, void *passdata)
{
  int strength = 0;
  int ber = 0;
  char *strength_str = NULL;
  char *ber_str = NULL;

  if (!line || !strcmp(line, "ERROR"))
  { 
    i_printf (1, "l_sigstrength_reading_callback failed to check signal strength");
    return -1;
  }
  else
  {
    strength_str = strchr (line, ':');
    if (!strength_str) 
    { 
      i_printf (1, "l_sigstrength_timer_callback strength indicator not found in result output"); 
      return -1;
    }
    else strength_str++;

    ber_str = strchr (line, ',');
    if (!ber_str)
    { 
      i_printf (1, "l_sigstrength_timer_callback BER indicator not found in result output"); 
      return -1;
    }
    else
    {
      *ber_str = '\0';  /* Terminates the strength_str part */
      ber_str++;
    }

    if (strength_str) strength = atoi (strength_str);
    if (ber_str) ber = atoi (ber_str);
  }
  
  i_printf (0, "l_sigstrength_reading_callback has GSM field strength at %i and B.E.R at %i", strength, ber);

  return 0;
}
