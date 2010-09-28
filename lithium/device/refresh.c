#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/entity.h>
#include <induction/cement.h>
#include <induction/device.h>
#include <induction/form.h>
#include <induction/hierarchy.h>
#include <induction/container.h>

#include "snmp.h"
#include "avail.h"
#include "refresh.h"

/*
 * Device-wide refresh sub-system
 *
 * This sub-system simply perform a refresh
 * config load and apply for the device entity
 */

/* Initialisation */

int l_refresh_init (i_resource *self, i_device *dev)
{
  int num;
  static i_entity_refresh_config defconfig;

  /* Check marking */
  if (dev->mark == ENTSTATE_OUTOFSERVICE) 
  {
    /* Do not enable refresh */
    return 0;
  }
  
  /* Setup defaults */
  memset (&defconfig, 0, sizeof(i_entity_refresh_config));
  defconfig.refresh_method = REFMETHOD_TIMER;
  defconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* Initially, set refresh interval to be 15sec */
  if (!strstr(self->hierarchy->dev->vendor_str, "xserve") && !strstr(self->hierarchy->dev->vendor_str, "osx") && !strstr(self->hierarchy->dev->vendor_str, "xsan"))
  { defconfig.refresh_int_sec = 15; }
  else
  { defconfig.refresh_int_sec = dev->refresh_interval; }

  /* Load and apply */
  num = i_entity_refresh_config_loadapply (self, ENTITY(dev), &defconfig);
  if (num != 0)
  { i_printf (1, "l_refresh_init failed to call i_entity_refresh_config_loadapply for device entity"); return -1; }

  /* Add refresh call back */
  i_entity_refreshcb_add (ENTITY(dev), l_refresh_device_refcb, NULL);

  return 0;
}

int l_refresh_device_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when the device entity has been refresed */
  i_device *dev = (i_device *) ent;
  dev->refresh_count++;

  /* Reset snmp session timeouts */
  l_snmp_session_reset_timeout (self, NULL);

  /* Check to see if first refresh has been done */
  if (dev->refresh_count == 1 && !strstr(self->hierarchy->dev->vendor_str, "xserve") && !strstr(self->hierarchy->dev->vendor_str, "osx") && !strstr(self->hierarchy->dev->vendor_str, "xsan"))
  {
    /* Set to configured refresh */
    static i_entity_refresh_config defconfig;
    memset (&defconfig, 0, sizeof(i_entity_refresh_config));
    defconfig.refresh_method = REFMETHOD_TIMER;
    defconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
    defconfig.refresh_int_sec = dev->refresh_interval;
    int num = i_entity_refresh_config_loadapply (self, ENTITY(dev), &defconfig);
    if (num != 0)
    { i_printf (1, "l_refresh_device_refcb failed to call i_entity_refresh_config_loadapply for device entity"); }
  }

  /* Attempt to close the SNMP session used for the device */
  if (l_snmp_state()) l_snmp_session_close_device (self);

  return 0;   /* Keep CB alive */
}

int l_refresh_backtoback_timercb (i_resource *self, i_timer *timer, void *passdata)
{
  /* Always return -1 */
  i_entity *ent = passdata;
  i_entity_refresh (self, ent, REFFLAG_MANUAL, NULL, NULL);
  return -1;
}
