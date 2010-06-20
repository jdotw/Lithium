#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>
#include <induction/name.h>
#include <induction/str.h>
#include <induction/xsanregistry.h>
#include <lithium/snmp.h>
#include <lithium/record.h>

#include "osx_server.h"
#include "data.h"
#include "xsaninfo.h"
#include "xsanvol.h"
#include "xsanvisdisk.h"

/* Xsan */

static int static_enabled = 0;
static int static_role = 0;   /* 0=Unknown, 1=Client, 2=Controller */
static int static_version = 1;  /* 1=Xsan 1.x 2=Xsan 2.x */

int v_xsan_enabled ()
{ return static_enabled; }

int v_xsan_role ()
{ return static_role; }

int v_xsan_role_set (int role)
{ 
  static_role = role;
  return 0; 
}

int v_xsan_version ()
{ return static_version; }

int v_xsan_version_set (int version)
{ 
  static_version = version;
  return 0; 
}

/* Enable */

int v_xsan_enable (i_resource *self)
{
  /* Enable Xsan Monitoring */
  int num;

  if (self->hierarchy->dev->limited == 1) return 0;

  num = v_xsaninfo_enable (self);
  if (num != 0)
  { i_printf (1, "v_xsan_enable failed to enable the xsaninfo sub-system"); return -1; }

  static_enabled = 1;

  return 0;
}

int v_xsan_enable_controller (i_resource *self)
{
  /* Enable monitoring of Xsan Controller */
  int num;

  if (self->hierarchy->dev->limited == 1) return 0;

  num = v_xsanvol_enable (self);
  if (num != 0)
  { i_printf (1, "v_xsan_enable failed to enable the xsanvol sub-system"); return -1; }
  
  num = v_xsanvisdisk_enable (self);
  if (num != 0)
  { i_printf (1, "v_xsan_enable failed to enable the xsanvol sub-system"); return -1; }

  num = v_xsaninfo_enable_history (self);
  if (num != 0)
  { i_printf (1, "v_xsan_enable failed to enable the xsaninfo sub-system"); return -1; }

  return 0;
}

int v_xsan_enable_client (i_resource *self)
{
  /* Enable monitoring of Xsan Client */
  int num;

  if (self->hierarchy->dev->limited == 1) return 0;

  num = v_xsanvisdisk_enable (self);
  if (num != 0)
  { i_printf (1, "v_xsan_enable failed to enable the xsanvol sub-system"); return -1; }

  num = v_xsaninfo_enable_history (self);
  if (num != 0)
  { i_printf (1, "v_xsan_enable failed to enable the xsaninfo sub-system"); return -1; }

  return 0;
}

