#include <stdlib.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/device.h>
#include <induction/object.h>
#include <induction/path.h>
#include <induction/hierarchy.h>

#include "avail.h"
#include "snmp.h"

static i_list *static_session_list = NULL;
static i_list *static_close_queue = NULL;

static l_snmp_session *static_device_session = NULL;

/* Variable/Pointer Fetching */

i_list* l_snmp_session_list ()
{ return static_session_list; }

i_list* l_snmp_session_close_queue ()
{ return static_close_queue; }

/* Struct Manipulation */

l_snmp_session* l_snmp_session_create ()
{
  l_snmp_session *session;

  session = (l_snmp_session *) malloc (sizeof(l_snmp_session));
  if (!session)
  { i_printf (1, "l_snmp_session failed to malloc session struct"); return NULL; }
  memset (session, 0, sizeof(l_snmp_session));

  return session;
}

void l_snmp_session_free (void *session_ptr)
{
  l_snmp_session *session = session_ptr;

  if (!session) return;

  if (session->host_str) free (session->host_str);
  if (session->community_str) free (session->community_str);
  /* Dont free/close the session. This func is only concerned
   * with freeing non-SNMP-lib memory
   */

  free (session);
}

/* Session manipulation */

l_snmp_session* l_snmp_session_open (i_resource *self, char *host_str, char *community_str, int version, int authmethod, int privenc, char *username, char *authpass_str, char *privpass_str)
{
  /* Should dynamicly create the static_session_list */
  int num;
  struct snmp_session ss;
  l_snmp_session *session;
  i_object *availobj = l_snmp_availobj ();

  if (!host_str || strlen(host_str) < 1)
  { 
    i_printf (1, "l_snmp_session_open failed to open session due to blank host_str(%s)", host_str); 
    l_avail_record_fail (availobj);
    return NULL; 
  }

  /* Create and setup session struct */
  session = l_snmp_session_create ();
  if (!session)
  { 
    i_printf (1, "l_snmp_session_open failed to create l_snmp_session struct"); 
    l_avail_record_fail (availobj);
    return NULL; 
  }
  session->host_str = strdup (host_str);
  if (community_str) session->community_str = strdup (community_str);

  /* Initialise session */
  snmp_sess_init (&ss);
  ss.peername = host_str;
  if (community_str)
  {
    ss.community = (unsigned char *) community_str;
    ss.community_len = strlen (community_str);
  }
  if (version == 1)
  {
    ss.version = SNMP_VERSION_1;
  }
  if (version == 2)
  {
    ss.version = SNMP_VERSION_2c;
  }
  if (version == 3)
  {
    ss.version = SNMP_VERSION_3;
    if (authmethod == 0 && privenc == 0) 
    {
      i_printf (1, "l_snmp_session_open using NO AUTH");
      ss.securityLevel = SNMP_SEC_LEVEL_NOAUTH;
    }
    if (authmethod != 0 && privenc == 0) 
    {
      i_printf (1, "l_snmp_session_open using AUTH NO PRIV");
      ss.securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;
    }
    if (authmethod != 0 && privenc != 0) 
    {
      i_printf (1, "l_snmp_session_open using AUTH AND PRIV");
      ss.securityLevel = SNMP_SEC_LEVEL_AUTHPRIV;
    }
    if (authmethod == 1)
    { 
      i_printf (1, "l_snmp_session_open using MD5 privacy");
      ss.securityAuthProto = usmHMACMD5AuthProtocol; 
      ss.securityAuthProtoLen = USM_AUTH_PROTO_MD5_LEN;
    }
    else if (authmethod == 2)
    {
      i_printf (1, "l_snmp_session_open using SHA privacy");
      ss.securityAuthProto = usmHMACSHA1AuthProtocol;
      ss.securityAuthProtoLen = USM_AUTH_PROTO_SHA_LEN;
    }
    if (privenc == 1)
    {
      i_printf (1, "l_snmp_session_open using DES privacy");
      ss.securityPrivProto = usmDESPrivProtocol;
      ss.securityPrivProtoLen = USM_PRIV_PROTO_DES_LEN;
    }
    else if (privenc == 2)
    {
#ifdef USM_PRIV_PROTO_AES_LEN
      i_printf (1, "l_snmp_session_open using AES privacy");
      ss.securityPrivProto = usmAESPrivProtocol;
      ss.securityPrivProtoLen = USM_PRIV_PROTO_AES_LEN;
#endif
    }
    i_printf (1, "l_snmp_session_open using username %s", username);
    ss.securityName = username;
    if (username) ss.securityNameLen = strlen (username);
    if (authpass_str)
    { 
      ss.securityAuthKeyLen = USM_AUTH_KU_LEN;
      i_printf (1, "l_snmp_session_open using authpass %s", authpass_str);
      if (generate_Ku(ss.securityAuthProto, ss.securityAuthProtoLen,
          (u_char *) authpass_str, strlen(authpass_str), ss.securityAuthKey, &ss.securityAuthKeyLen) != SNMPERR_SUCCESS)
      { 
        i_printf (1, "l_snmp_session_open ****** Failed to generate auth key ********* %p %i", ss.securityAuthProto, ss.securityAuthProtoLen); 
        snmp_perror("lithium");
      }
      else
      { i_printf (1, "l_snmp_session_open #### key generated!! ####"); }
    }
    if (privpass_str)
    { 
      ss.securityPrivKeyLen = USM_PRIV_KU_LEN;
      i_printf (1, "l_snmp_session_open using privpass %s", privpass_str);
      generate_Ku(ss.securityAuthProto, ss.securityAuthProtoLen,
        (u_char *) privpass_str, strlen(privpass_str), ss.securityPrivKey, &ss.securityPrivKeyLen); 
    }
  }
  ss.callback = l_snmp_pducallback_process_pdu;
  ss.callback_magic = session;
  l_snmp_session_reset_timeout (self, &ss);

  /* Open SNMP session */
  session->ss = snmp_open (&ss);
  if (!session->ss)
  { 
    i_printf (1, "l_snmp_session_open failed to open SNMP session to %s", host_str); 
    l_snmp_session_free (session); 
    l_avail_record_fail (availobj);
    return NULL; 
  }

  /* Enqueue */
  if (!static_session_list)
  { static_session_list = i_list_create (); }
  num = i_list_enqueue (static_session_list, session);
  if (num != 0)
  { 
    i_printf (1, "l_snmp_session_open failed to enqueue session to static_session_list"); 
    l_snmp_session_free (session); 
    l_avail_record_fail (availobj);
    return NULL; 
  }
  
  return session;
}

l_snmp_session* l_snmp_session_open_device (i_resource *self, i_device *device)
{
  l_snmp_session *sess;

  if (static_device_session) return static_device_session;

  /* Open Session */
  sess = l_snmp_session_open (self, device->ip_str, device->snmpcomm_str, device->snmpversion, device->snmpauthmethod, device->snmpprivenc, device->username_str, device->snmpauthpass_str, device->snmpprivpass_str);
  if (!sess)
  { i_printf (1, "l_snmp_session_open_device failed to open session"); return NULL; }

  /* Set Device Flag */
  sess->device_session = 1;
  static_device_session = sess;

  return sess;
}

int l_snmp_session_close (l_snmp_session *session)
{
  int num; 

  if (l_snmp_state() == 0 || !session) return -1;

  if (session == static_device_session) return 0;

  /* Remove from session list */
  num = i_list_search (static_session_list, session);
  if (num == 0)
  { i_list_delete (static_session_list); }
  else
  { i_printf (1, "l_snmp_session_close warning, called for session %p which is not in the session list", session); }
  if (static_session_list && static_session_list->size == 0)
  { i_list_free (static_session_list); static_session_list = NULL; }

  /* Place the session->ss in the close queue */
  if (!static_close_queue)
  { 
    static_close_queue = i_list_create ();
    if (!static_close_queue)
    { i_printf (1, "l_snmp_session_close failed to create static_close_queue"); return -1; }
  }
  i_list_enqueue (static_close_queue, session->ss);

  /* Free the sesession structt */
  l_snmp_session_free (session);

  return 0;
}

int l_snmp_session_reset_timeout (i_resource *self, struct snmp_session *ss)
{
  /* Called to restart the timeout of the device SNMP session */
  if (!ss && static_device_session) ss = static_device_session->ss;
  if (ss)
  {
    ss->retries = 0;
    ss->timeout = (self->hierarchy->dev->refresh_interval / 5) * 1000000L;
    if (ss->timeout < 3000000L) ss->timeout = 3000000L;
  }
  return 0;
}

int l_snmp_session_close_queue_free ()
{
  if (static_close_queue)
  { i_list_free (static_close_queue); static_close_queue = NULL; }

  return 0;
}

/* Error strings */

char* l_snmp_session_error_str (l_snmp_session *session)
{
  switch (session->error)
  {
    case SNMP_ERROR_NOERROR: return "No SNMP error";
    case SNMP_ERROR_NULLPDU: return "Null SNMP PDU received error";
    case SNMP_ERROR_SENDFAILED: return "SNMP PDU send error";
    case SNMP_ERROR_TIMEOUT: return "SNMP time-out error";
    case SNMP_ERROR_OIDNOTINCREASING: return "SNMP walk OID not increasing error";
    default: return "Other SNMP error";
  } 

  return NULL;
}
