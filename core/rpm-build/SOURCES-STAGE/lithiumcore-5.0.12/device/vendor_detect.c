#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <netdb.h>
#include <string.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/device.h>
#include <induction/inventory.h>
#include <induction/construct.h>
#include <induction/hierarchy.h>
#include <induction/vendor.h>
#include <induction/configfile.h>
#include <induction/files.h>

#include "snmp.h"
#include "vendor.h"

int l_vendor_detect_sysDescr_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata);

/* Vendor Auto-Detection */

int l_vendor_detect (i_resource *self)
{
  int reqid;
  l_snmp_session* session;

  session = l_snmp_session_open_device (self, self->hierarchy->dev);
  if (!session)
  { i_printf (1, "l_vendor_detect failed to open SNMP session to device"); return -1; }

  reqid = l_snmp_get_oid_str (self, session, "sysDescr.0", l_vendor_detect_sysDescr_callback, NULL);
  if (reqid == -1)
  { i_printf (1, "l_vendor_detect failed to send SNMP_GET for sysDescr.0"); return -1; }

  return 0;
}

int l_vendor_detect_sysDescr_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata)
{ 
  /* Called when the response to the SNMP_GET on sysDescr.0
   * for the purpose of vendor auto-detection 
   */

  int num;
  int i = 0;
  char *vendor_id;
  char *sysdescr;

  if (!pdu)
  {
    /* Timeout occurred */
    if (session->error == SNMP_ERROR_TIMEOUT)
    { l_vendor_init_failed (self, "Timed out waiting for sysDescr.0 SNMP request"); }
    else
    { l_vendor_init_failed (self, "SNMP error (non-timeout) occurred whilst retrieving sysDescr.0 OID from host"); }
    return -1;
  }
  
  if (!pdu->variables || pdu->variables->type != ASN_OCTET_STR)
  {
    /* Error/Wrong Response */
    l_vendor_init_failed (self, "Received invalid response to sysDescr.0 SNMP request");
    return -1; 
  }

  sysdescr = l_snmp_get_string_from_pdu (pdu);
  l_snmp_session_close (session);

  while ((vendor_id = i_configfile_get (self, VENDORCONF_FILE, "vendors", "vendor_id", i))!=NULL)
  {
    /* Loop through each configured vendor module */

    char *detect_str;

    i++;

    detect_str = i_configfile_get (self, VENDORCONF_FILE, vendor_id, "detect_string", 0);
    if (!detect_str) continue;

    if (strstr(sysdescr, detect_str))
    {
      /* detect_str found in sysDescr.0 output
       * load the vendor module for this vendor
       */
      
      l_vendor_load (self, vendor_id);

      free (detect_str);
      free (vendor_id); 
      free (sysdescr);

      return 0;
    }

    free (detect_str);
    free (vendor_id);
  }
  free (sysdescr);

  /* Auto-detection failed, use generic vendor code */

  num = l_vendor_load (self, "generic");
  if (num != 0)
  { 
    l_vendor_init_failed (self, "Failed to load generic vendor module"); 
    return -1; 
  }

  /* Finished */

  return 0;
}

