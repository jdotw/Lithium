#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/module.h>
#include <induction/auth.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/construct.h>
#include <induction/list.h>

int l_restart_timercb (i_resource *self, i_timer *timer, void *passdata)
{
  /* Kill */
  i_printf (0, "l_restart_timercb restarting process on command");
  exit (0);
}
