#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/loop.h>
#include <induction/callback.h>
#include <induction/object.h>
#include <induction/hierarchy.h>

#include "avail.h"
#include "snmp.h"

/* Static pointers / variables */

static int static_snmp_enabled = 0;

static i_callback* static_fdset_preprocessor_cb = NULL;
static i_callback* static_fdset_postprocessor_cb = NULL;

static i_object *static_availobj = NULL;

/* Pointer/Variable fetching */

int l_snmp_state ()
{ return static_snmp_enabled; }

i_object* l_snmp_availobj ()
{ return static_availobj; }

/* SNMP sub-system manipulation */

int l_snmp_enable (i_resource *self)
{
  /* Called to initialise the the SNMP sub-system */

  /* Check/Set State */
  if (static_snmp_enabled == 1)
  { i_printf (1, "l_snmp_enabled warning, l_snmp sub-system already enabled"); return 0; }
  static_snmp_enabled = 1;

  /* Add i_loop_fdset pre/post processor functions */
  static_fdset_preprocessor_cb = i_loop_fdset_preprocessor_add (self, l_snmp_fdset_preprocessor, NULL);
  if (!static_fdset_preprocessor_cb)
  { i_printf (1, "l_snmp_enable failed to add fdset preprocessor"); l_snmp_disable (self); return -1; }
  static_fdset_postprocessor_cb = i_loop_fdset_postprocessor_add (self, l_snmp_fdset_postprocessor, NULL);
  if (!static_fdset_postprocessor_cb)
  { i_printf (1, "l_snmp_enable failed to add fdset postprocessor"); l_snmp_disable (self); return -1; }

  /* Initialise snmp library */
  const char *type = "lithium_device";
#ifdef NETSNMP_DS_LIB_DONT_PERSIST_STATE
  netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_DONT_PERSIST_STATE, 1);
#endif
  init_snmp (type);

  /* Add Availability Object */
  static_availobj = l_avail_object_add (self, "snmp", "SNMP");
  if (!static_availobj)
  { i_printf (1, "l_snmp_enable warning, failed to add availability object"); }

  return 0;
}

int l_snmp_disable (i_resource *self)
{
  /* Disable the SNMP sub-system */

  if (static_snmp_enabled == 0)
  { i_printf (1, "l_snmp_disable warning, l_snmp sub-system already disabled"); return 0; }
  static_snmp_enabled = 0;

  /* Remove i_loop_fdset pre/post processors */
  if (static_fdset_preprocessor_cb)
  { i_loop_fdset_preprocessor_remove (self, static_fdset_preprocessor_cb); }
  if (static_fdset_postprocessor_cb)
  { i_loop_fdset_postprocessor_remove (self, static_fdset_postprocessor_cb); }

  /* Shutdown snmp library */
  snmp_shutdown ("lithium_device");

  /* Remove Availability Object */
  if (static_availobj)
  { l_avail_object_remove (self, static_availobj); static_availobj = NULL; }

  return 0;
}

