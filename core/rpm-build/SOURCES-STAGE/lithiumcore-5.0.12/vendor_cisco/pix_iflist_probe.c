#include <stdlib.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/hashtable.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/threshold.h"
#include "induction/status.h"
#include "induction/hierarchy.h"
#include "induction/device.h"
#include "induction/postgresql.h"
#include "induction/callback.h"
#include "device/snmp.h"

#include "pix.h"

static int static_reqid = 0;
static l_snmp_session *static_snmp_session = NULL;

int v_pix_iflist_probe (i_resource *self)
{
  /* Open SNMP session */

  static_snmp_session = l_snmp_session_open_device (self, self->hierarchy->dev);
  if (!static_snmp_session)
  { i_printf (1, "v_pix_iflist_probe failed to establish snmp session to %s", self->hierarchy->device->ip_str); return -1; }

  /* Get sysDescr */

  static_reqid = l_snmp_get_oid (self, static_snmp_session, "sysDescr.0", v_pix_iflist_probe_snmp_callback, NULL);
  if (static_reqid == -1)
  {
    i_printf (1, "v_pix_iflist_probe failed to request sysDescr.0");
    l_snmp_session_close (static_snmp_session);
    static_snmp_session = NULL;
    return -1;
  }

  return 0;
}

int v_pix_iflist_probe_snmp_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata)
{
  static_reqid = 0;

  if (pdu && pdu->variables)
  {
    char *descr_str;

    /* Check if the device is a pix */
    descr_str = l_snmp_get_string_from_pdu (pdu);
    if (descr_str && strstr (descr_str, "PIX"))
    { v_pix_iflist_refresh_enable (self); }
    if (descr_str)
    { free (descr_str); }
  }
  else
  {
    /* Failed, device does not support sysDescr.0 */
    if (session->error == SNMP_ERROR_TIMEOUT)
    { i_printf (1, "v_pix_iflist_probe_snmp_callback PIX IfList probe failed; SNMP timeout occurred"); }
    else
    { i_printf (1, "v_pix_iflist_probe_snmp_callback PIX IfList probe failed; SNMP error (non-timeout) occurred"); }
  }

  if (static_snmp_session)
  { l_snmp_session_close (static_snmp_session); static_snmp_session = NULL; }

  return 0;
}
