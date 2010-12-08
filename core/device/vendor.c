#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/device.h>
#include <induction/inventory.h>
#include <induction/construct.h>
#include <induction/hierarchy.h>
#include <induction/vendor.h>

#include "vendor.h"

/* Vendor sub-system */

#define RETRY_TIMER_SEC 60

static i_timer* static_retry_timer = NULL;

/* Sub-System Initialisation */

int l_vendor_init (i_resource *self)
{
  /* Return 0 where this function wil be recalled
   * by way of the retry timer (by invoking l_vendor_init_failed) 
   */

  int num;

  /* Initialise the vendor code in induction */
  num = i_vendor_init ();
  if (num != 0)
  { i_printf (1, "l_vendor_init failed to initialise i_vendor sub-system"); l_vendor_init_failed (self, "Failed to initialise i_vendor sub-system"); return 0; }
  
  /* Check to see if we're using a modbuilder */
  if (strstr(self->hierarchy->dev->vendor_str, ".xml"))
  { return 0; }
  
  /* Configuration is present */
  num = l_vendor_load (self, self->hierarchy->dev->vendor_str);
  if (num != 0)
  { 
    i_printf (1, "l_vendor_init failed to load vendor module for vendor_id %s", self->hierarchy->dev->vendor_str);
    l_vendor_init_failed (self, "Failed to load vendor module");
  }
  else
  { i_printf (2, "l_vendor_init loaded %s vendor module", self->hierarchy->dev->vendor_str); }
  
  return 0;
}

/* Failure handling / Retrying */

int l_vendor_init_failed (i_resource *self, char *error)
{
  /* Called when the vendor_init operation has failed */
  static_retry_timer = i_timer_add (self, RETRY_TIMER_SEC, 0, l_vendor_init_retry_callback, NULL);
  if (!static_retry_timer)
  { i_printf (1, "l_vendor_init_failed failed to add retry timer"); return -1; }

  return 0;
}

int l_vendor_init_retry_callback (i_resource *self, struct i_timer_s *timer, void *data)
{
  /* Called to retry vendor system initialisation
   *
   * Non-persistent timer, always return -1
   */

  l_vendor_init (self);

  return -1;
}

