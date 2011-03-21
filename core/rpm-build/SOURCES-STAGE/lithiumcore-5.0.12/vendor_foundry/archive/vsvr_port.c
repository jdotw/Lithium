#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/threshold.h>
#include <induction/status.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/inventory.h>
#include <lithium/snmp.h>

#include "vsvr.h"

/* Struct Manipulation */

v_vsvr_port* v_vsvr_port_create ()
{
  v_vsvr_port *port;

  port = (v_vsvr_port *) malloc (sizeof(v_vsvr_port));
  if (!port)
  { i_printf (1, "v_vsvr_port_create failed to malloc port struct"); return NULL; }
  memset (port, 0, sizeof(v_vsvr_port));

  return port;
}

void v_vsvr_port_free (void *portptr)
{
  v_vsvr_port *port = portptr;

  if (!port) return;

  free (port);
}
