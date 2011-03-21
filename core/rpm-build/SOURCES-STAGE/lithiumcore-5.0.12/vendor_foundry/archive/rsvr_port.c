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

#include "rsvr.h"

/* Struct Manipulation */

v_rsvr_port* v_rsvr_port_create ()
{
  v_rsvr_port *port;

  port = (v_rsvr_port *) malloc (sizeof(v_rsvr_port));
  if (!port)
  { i_printf (1, "v_rsvr_port_create failed to malloc port struct"); return NULL; }
  memset (port, 0, sizeof(v_rsvr_port));

  return port;
}

void v_rsvr_port_free (void *portptr)
{
  v_rsvr_port *port = portptr;

  if (!port) return;

  free (port);
}
