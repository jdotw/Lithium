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
#include <induction/object.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/hierarchy.h>
#include <induction/callback.h>

#include "../device/snmp.h"
#include "devtest.h"

extern i_resource *global_self;

int l_devtest_snmp (i_resource *self, i_device *dev, int (*cbfunc) (i_resource *self, int result, void *passdata), void *passdata)
{
  /* Send the SNMP_MSG_GET to the specified session
   * to request the given oid_str. If a callback func is
   * supplied then a snmp_pducallback will be installed
   * for it.
   */

  char *errstr;
  struct snmp_pdu *pdu;
  struct snmp_session ss;
  i_callback *cb;

  /* Create callback */
  cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;

  /* Init session */
  snmp_sess_init (&ss);
  ss.peername = dev->ip_str;
  if (dev->snmpcomm_str)
  { 
    ss.community = (unsigned char *) dev->snmpcomm_str;
    ss.community_len = strlen (dev->snmpcomm_str);
  } 
  if (dev->snmpversion == 1)
  { 
    ss.version = SNMP_VERSION_1;
  }   
  if (dev->snmpversion == 2)
  {
    ss.version = SNMP_VERSION_2c;
  }   
  if (dev->snmpversion == 3)  {
    ss.version = SNMP_VERSION_3;
    if (dev->snmpauthmethod == 0 && dev->snmpprivenc == 0)
    { 
      ss.securityLevel = SNMP_SEC_LEVEL_NOAUTH;
    }
    if (dev->snmpauthmethod != 0 && dev->snmpprivenc == 0)
    {
      ss.securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;
    }
    if (dev->snmpauthmethod != 0 && dev->snmpprivenc != 0)
    {
      ss.securityLevel = SNMP_SEC_LEVEL_AUTHPRIV;
    }
    if (dev->snmpauthmethod == 1)
    {
      ss.securityAuthProto = usmHMACMD5AuthProtocol;
      ss.securityAuthProtoLen = USM_AUTH_PROTO_MD5_LEN;
    }
    else if (dev->snmpauthmethod == 2)
    {
      ss.securityAuthProto = usmHMACSHA1AuthProtocol;
      ss.securityAuthProtoLen = USM_AUTH_PROTO_SHA_LEN;
    }
    if (dev->snmpprivenc == 1)
    {
      ss.securityPrivProto = usmDESPrivProtocol;
      ss.securityPrivProtoLen = USM_PRIV_PROTO_DES_LEN;
    }
    else if (dev->snmpprivenc == 2)
    {
#ifdef USM_PRIV_PROTO_AES_LEN
      ss.securityPrivProto = usmAESPrivProtocol;
      ss.securityPrivProtoLen = USM_PRIV_PROTO_AES_LEN;
#endif
    }
    ss.securityName = dev->username_str;
    if (dev->username_str) ss.securityNameLen = strlen (dev->username_str);
    if (dev->snmpauthpass_str)
    {
      ss.securityAuthKeyLen = USM_AUTH_KU_LEN;
      if (generate_Ku(ss.securityAuthProto, ss.securityAuthProtoLen,
          (u_char *) dev->snmpauthpass_str, strlen(dev->snmpauthpass_str), ss.securityAuthKey, &ss.securityAuthKeyLen) != SNMPERR_SUCCESS)
      {
        snmp_perror("lithium");
      }
    }
    if (dev->snmpprivpass_str)
    {
      ss.securityPrivKeyLen = USM_PRIV_KU_LEN;
      generate_Ku(ss.securityAuthProto, ss.securityAuthProtoLen,
        (u_char *) dev->snmpprivpass_str, strlen(dev->snmpprivpass_str), ss.securityPrivKey, &ss.securityPrivKeyLen);
    }
  }
  ss.callback = l_devtest_snmp_process_pdu;
  ss.callback_magic = cb;
  ss.timeout = 1000000L;
  ss.retries = 5;

  /* Open Session */
  cb->data = snmp_open (&ss);
  if (!cb->data)
  { i_printf (1, "l_devtest_snmp failed to open snmp session"); return -1; }

  /* Parse the OID */
  oid name[MAX_OID_LEN];
  size_t name_size = MAX_OID_LEN;
  l_devtest_snmp_parse (".1.3.6.1.2.1.1.1.0", name, &name_size);

  /* Create PDU and add OID */
  pdu = snmp_pdu_create (SNMP_MSG_GET);
  if (!pdu)
  { i_printf (1, "l_snmp_get_oid failed to create request pdu"); return -1; }
  snmp_add_null_var (pdu, name, name_size);

  /* Sent PDU */
  cb->id_int = snmp_send (cb->data, pdu);
  if (cb->id_int == 0)   /* 0 on error for snmp_send */
  { 
    /* Error sending */
    snmp_error (cb->data, NULL, NULL, &errstr);
    i_printf (2, "l_snmp_get_oid failed to send request pdu (%s)", errstr); 
    free (errstr);
    snmp_free_pdu (pdu); 

    return -1; 
  }

  return 0;
}

int l_devtest_snmp_parse (char *oid_str, oid *name, size_t *name_size)
{
  int num;
  size_t orig_size = *name_size;

  /* Attempt read_objid method */
  num = read_objid (oid_str, name, name_size);
  if (num == 1)                                 /* Returns 1 on success */
  { return 0; }

  /* Attempt get_node method */
  *name_size = orig_size;
  num = get_node (oid_str, name, name_size);
  if (num == 1)                                 /* Returns 1 on success */
  { return 0; }

  /* Attempt get_wild_node method */
  *name_size = orig_size;
  num = get_wild_node (oid_str, name, name_size);
  if (num == 1)                                 /* Returns 1 on success */
  { return 0; }

  /* All methods have failed */
  return -1;
}

int l_devtest_snmp_process_pdu (int operation, struct snmp_session *ss, int reqid, struct snmp_pdu *pdu, void *magic)
{
  /* Called by the SNMP lib when a PDU is recieved.
   *
   * Return 1  (Keep libsnmp happy)
   */

  i_callback *cb = magic;

#ifdef NET_SNMP
  if (operation == NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE)
#else
  if (operation == RECEIVED_MESSAGE)
#endif
  {
    cb->func (global_self, 1, cb->passdata);
  }
  else
  {
    cb->func (global_self, 0, cb->passdata);
  }

  i_callback_free (cb);
  
  return 1;
}

