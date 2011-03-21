#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/hierarchy.h>
#include <induction/callback.h>
#include <induction/object.h>

#include "config.h"
#include "snmp.h"
#include "avail.h"

extern i_resource *global_self;
i_list *static_pducallback_list = NULL;

int l_snmp_pducallback_add (i_resource *self, l_snmp_session *session, int reqid, int (*callback_func) (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata), void *passdata)
{
  /* Adds an SNMP PDU callback */

  int num;
  i_callback *cb;
  struct timeval *start_tv;

  /* Check/Create PDU callback list */
  if (!static_pducallback_list)
  {
    static_pducallback_list = i_list_create ();
    if (!static_pducallback_list)
    { i_printf (1, "l_snmp_pducallback_add failed to create static_pducallback_list"); return -1; }
    i_list_set_destructor (static_pducallback_list, i_callback_free);
  }

  /* Record start time */
  start_tv = (struct timeval *) malloc (sizeof(struct timeval));
  if (!start_tv)
  { i_printf (1, "l_snmp_pducallback_add failed to malloc start_tv struct"); return -1; }
  gettimeofday (start_tv, NULL);

  /* Create callback */
  cb = i_callback_create ();
  if (!cb)
  { i_printf (1, "l_snmp_pducallback_add failed to create callback struct"); return -1; }
  cb->id_int = reqid;
  cb->data = start_tv;
  cb->func = callback_func;
  cb->passdata = passdata;

  /* Enqueue the callback */ 
  num = i_list_enqueue (static_pducallback_list, cb);
  if (num != 0)
  { 
    i_printf (1, "l_snmp_pducallback_add failed to enqueue the callback");
    free (cb->data); 
    cb->data = NULL;
    i_callback_free (cb);
    return -1;
  }

  return 0;
}

int l_snmp_pducallback_remove_by_reqid (l_snmp_session *session, int reqid)
{
  int num_removed = 0;
  i_callback *cb; 

  /* Find and remove the specified reqid */
  for (i_list_move_head(static_pducallback_list); (cb=i_list_restore(static_pducallback_list))!=NULL; i_list_move_next(static_pducallback_list))
  {
    if (cb->id_int == reqid)
    {
      /* Matching callback */ 
      free (cb->data);
      cb->data = NULL;
      i_list_delete (static_pducallback_list); 
      num_removed++; 
    }
  }

  /* Free the list if it is empty */
  if (static_pducallback_list && static_pducallback_list->size < 1)
  { i_list_free (static_pducallback_list); static_pducallback_list = NULL; }

  if (num_removed == 0) return -1;

  return 0;
}

int l_snmp_pducallback_process_pdu (int operation, struct snmp_session *ss, int reqid, struct snmp_pdu *pdu, void *magic)
{
  /* Called by the SNMP lib when a PDU is recieved.
   *
   * Return 1  (Keep libsnmp happy)
   */

  int num;
  i_callback *cb;
  l_snmp_session *session = magic;

  /* Find callback */
  for (i_list_move_head(static_pducallback_list); (cb=i_list_restore(static_pducallback_list))!=NULL; i_list_move_next(static_pducallback_list))
  { if (cb->id_int == reqid) break; }
  if (!cb)
  { i_printf (2, "l_snmp_pducallback_process_pdu recieved a PDU (reqid=%i) without a matching callback", reqid); return 1; }

  if (operation == NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE)
  {
    /* Successfully recvd a message */
    session->error = SNMP_ERROR_NOERROR;

    /* Record availability success */
    if (session->device_session == 1)
    { 
      i_object *availobj;
      struct timeval *start_tv = cb->data;
      struct timeval end_tv;

      gettimeofday (&end_tv, NULL);
      availobj = l_snmp_availobj ();
      l_avail_record_ok (availobj, start_tv, &end_tv);
    }

    /* Set last_op_failed flag and reset timeout/retries */
    if (session->last_op_failed == 1)
    {
      session->last_op_failed = 0;
    }
  }
  else
  {
    /* Error occurred - set error code */
    switch (operation)
    {
      case NETSNMP_CALLBACK_OP_TIMED_OUT: session->error = SNMP_ERROR_TIMEOUT;
                                          break;
      case NETSNMP_CALLBACK_OP_SEND_FAILED: session->error = SNMP_ERROR_SENDFAILED;
                                            break;
      default: session->error = SNMP_ERROR_NULLPDU;
    }

    /* Record availability failure */
    if (session->device_session == 1)
    { 
      i_object *availobj;

      availobj = l_snmp_availobj ();
      l_avail_record_fail (availobj); 
    }

    /* Set last failed flag and adjust timeout/retries */
    session->last_op_failed = 1;
  }

  /* Run callback */
  if (cb->func) 
  { 
    if (session->error == SNMP_ERROR_NOERROR)
    { cb->func (global_self, session, reqid, pdu, cb->passdata); }
    else
    { cb->func (global_self, session, reqid, NULL, cb->passdata); }
  }

  /* Free callback data */
  if (cb->data)
  { free (cb->data); cb->data = NULL; }

  /* Remove callback */
  num = i_list_search (static_pducallback_list, cb);
  if (num == 0)
  { i_list_delete (static_pducallback_list); }

  /* Free list if necessary */
  if (static_pducallback_list && static_pducallback_list->size < 1)
  { i_list_free (static_pducallback_list); static_pducallback_list = NULL; }    

  return 1;
}

