#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <netdb.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/device.h>
#include <induction/inventory.h>
#include <induction/construct.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/hierarchy.h>
#include <induction/vendor.h>
#include <induction/configfile.h>
#include <induction/files.h>
#include <induction/xml.h>

#include "vendor.h"
#include "modb.h"
#include "snmp.h"

int l_snmp_xsnmp_detect_oid_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata);
int l_snmp_xsnmp_detect_retry_callback (i_resource *self, i_timer *timer, void *passdata);

static int static_xsnmp_enabled = 0;
static i_timer *static_retry_timer = NULL;

int l_snmp_xsnmp_enabled ()
{ 
  return static_xsnmp_enabled; 
}

void l_snmp_xsnmp_set_enabled (int value)
{
  static_xsnmp_enabled = value;
}

/* Xsnmp Presence Detection */

int l_snmp_xsnmp_detect_retry_callback (i_resource *self, i_timer *timer, void *passdata)
{
  /* Must return -1 to destroy timer */
  int reqid;
  l_snmp_session* session;

  session = l_snmp_session_open_device (self, self->hierarchy->dev);
  if (!session)
  { i_printf (1, "l_snmp_xsnmp_detect failed to open SNMP session to device"); return -1; }

  reqid = l_snmp_get_oid_str (self, session, "enterprises.20038.2.1.5.1.0", l_snmp_xsnmp_detect_oid_callback, NULL);
  if (reqid == -1)
  { i_printf (1, "l_snmp_xsnmp_detect failed to send SNMP_GET for xsnmpVersion"); return -1; }    

  return -1;    /* Destroy timer */
}

int l_snmp_xsnmp_detect (i_resource *self)
{
  l_snmp_xsnmp_detect_retry_callback (self, NULL, NULL);
  return 0;
}

int l_snmp_xsnmp_detect_oid_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata)
{ 
  /* Called when the response to the SNMP_GET on xsnmpVersion.0
   * for the purpose of vendor auto-detection 
   */

  int num;

  if (!pdu && session->error == SNMP_ERROR_TIMEOUT)
  {
    /* Timeout occurred */
    static_retry_timer = i_timer_create (self, 30, 0, l_snmp_xsnmp_detect_retry_callback, NULL);
    return -1;
  }
  else if (pdu && pdu->variables->type == ASN_GAUGE)
  {
    /* Xsnmp Enabled */
    char *version_str = l_snmp_get_string_from_pdu (pdu);
    l_snmp_session_close (session);

    i_printf (0, "l_snmp_xsnmp_detect_oid_callback Found Xsnmp version %s -- enabling Xsnmp monitoring extension", version_str);
    l_snmp_xsnmp_set_enabled(1);    
  }
  else
  {
    /* XSnmp not enabled */
    i_printf (1, "l_snmp_xsnmp_detect_oid_callback SNMP error (non-timeout) occurred whilst retrieving xsnmpVersion.0 OID from host -- assuming Xsnmp is not used"); 
    l_snmp_xsnmp_set_enabled(0);    
  }

  /*
   * Xsnmp Detection complete, continue with vendor/modb load 
   */
   
  /* Module Builder init */
  num = l_modb_init (self);
  if (num != 0)
  { i_printf (1, "l_snmp_xsnmp_detect_oid_callback failed to initialise module builder sub-system"); }

  /* Vendor initialisation */
  num = l_vendor_init (self);
  if (num != 0)
  { i_printf (1, "l_snmp_xsnmp_detect_oid_callback failed to initialise vendor module sub-system"); }

  return 0;
}

