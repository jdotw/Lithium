#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <netdb.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/object.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/hierarchy.h>

#include "snmp.h"
#include "avail.h"

int l_snmp_get_oid (i_resource *self, l_snmp_session *session, oid *name, size_t name_size, int (*cbfunc) (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata), void *passdata)
{
  /* Send the SNMP_MSG_GET to the specified session
   * to request the given oid_str. If a callback func is
   * supplied then a snmp_pducallback will be installed
   * for it.
   */

  int num;
  int reqid;
  char *errstr;
  struct snmp_pdu *pdu;

  /* Check marking */
  if (self->hierarchy->dev->mark == ENTSTATE_OUTOFSERVICE) return -1; 

  /* Create PDU and add OID */
  pdu = snmp_pdu_create (SNMP_MSG_GET);
  if (!pdu)
  { i_printf (1, "l_snmp_get_oid failed to create request pdu"); return -1; }
  snmp_add_null_var (pdu, name, name_size);

  /* Sent PDU */
  reqid = snmp_send (session->ss, pdu);
  if (reqid == 0)   /* 0 on error for snmp_send */
  { 
    i_object *availobj;

    /* Error sending */
    snmp_error (session->ss, NULL, NULL, &errstr);
    i_printf (2, "l_snmp_get_oid failed to send request pdu (%s)", errstr); 
    free (errstr);
    snmp_free_pdu (pdu); 

    /* Record avail */
    availobj = l_snmp_availobj ();
    l_avail_record_fail (availobj);

    return -1; 
  }

  num = l_snmp_pducallback_add (self, session, reqid, cbfunc, passdata);
  if (num != 0)
  { i_printf (1, "l_snmp_get_oid sent request but failed to add a pducallback"); return -1; }

  return reqid;
}

int l_snmp_get_oid_str (i_resource *self, l_snmp_session *session, char *oid_str, int (*cbfunc) (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata), void *passdata)
{
  oid name[MAX_OID_LEN];
  size_t name_size = MAX_OID_LEN;

  l_snmp_parse_oidstr (oid_str, name, &name_size);
  return l_snmp_get_oid (self, session, name, name_size, cbfunc, passdata);
}

