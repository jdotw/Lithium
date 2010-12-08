#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/cement.h>
#include <induction/hierarchy.h>
#include <induction/container.h>

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
  
  /* Setup defaults */
  memset (&defconfig, 0, sizeof(i_entity_refresh_config));
  defconfig.refresh_method = REFMETHOD_TIMER;
  defconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* Initially, set refresh interval to be 15sec */
  defconfig.refresh_int_sec = 15;

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

  /* Check to see if 3 refreshes have been done */
  if (dev->refresh_count == 3)
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

  /* Reset snmp session timeouts */
  l_snmp_session_reset_timeout ();

  return 0;   /* Keep CB alive */
}
