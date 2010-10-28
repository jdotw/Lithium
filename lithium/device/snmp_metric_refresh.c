#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/ip.h>
#include <induction/entity.h>
#include <induction/cement.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>

#include "snmp.h"
#include "avail.h"

/*
 * Refresh Related Functions
 */


void check_overflow_signed (long *x) 
{
  if (sizeof(long) > 4)
  {
    if (*x > INT32_MAX) *x &= 0xffffffff;
    else if (*x < INT32_MIN) *x = 0 - (*x & 0xffffffff);
  }
}

void check_overflow_unsigned (unsigned long *x)
{
  if (sizeof(unsigned long) > 4)
  {
    if (*x > UINT32_MAX) *x &= 0xffffffff;
  }
}

/* Refresh data struct manipulation */

l_snmp_metric_refresh_data* l_snmp_metric_refresh_data_create ()
{
  l_snmp_metric_refresh_data *data;

  data = (l_snmp_metric_refresh_data *) malloc (sizeof(l_snmp_metric_refresh_data));
  if (!data)
  { i_printf (1, "l_snmp_metric_refresh_data_create failed to malloc l_snmp_metric_refresh_data struct"); return NULL; }
  memset (data, 0, sizeof(l_snmp_metric_refresh_data));
  data->name_len = MAX_OID_LEN;

  return data;
}

void l_snmp_metric_refresh_data_free (void *dataptr)
{
  l_snmp_metric_refresh_data *data = dataptr;

  if (!data) return;

  if (data->oid_str) free (data->oid_str);

  free (data);
}

/* Cement SNMP Metric Refresh */

int l_snmp_metric_refresh (i_resource *self, i_metric *met, int opcode)
{ 
  l_snmp_metric_refresh_data *data = met ? met->refresh_data : NULL;

  /* Check data */
  if (!data)
  {
    /* This should never happen */ 
    if (met && met->obj && met->obj->cnt)
    {
      i_printf (1, "l_snmp_metric_refresh called with NULL met->refresh data for metric %s:%s:%s", 
        met->obj->cnt->name_str, met->obj->name_str, met->name_str);
    }
    else
    {
      i_printf (1, "l_snmp_metric_refresh called with MULL met, obj or container");
    }
    return -1;
  }

  /* Process refresh */
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:       /* Begin the refresh process */
      /* Open Session */
      data->session = l_snmp_session_open_device (self, met->obj->cnt->dev);
      if (!data || !data->session)
      { 
        i_printf (2, "l_snmp_metric_refresh failed to open SNMP session to %s for %s %s",
          met->obj->cnt->dev->name_str, i_entity_typestr (met->ent_type), met->name_str);
        return -1;
      }

      /* Get OID */
      data->reqid = l_snmp_get_oid (self, data->session, data->name, data->name_len, l_snmp_metric_refresh_getcb, met);
      if (data->reqid == -1)
      { 
        i_printf (2, "l_snmp_metric_refresh failed to send SNMP GET request for OID %s belonging to %s %s", 
          data->oid_str, i_entity_typestr (met->ent_type), met->name_str);
        l_snmp_session_close (data->session);
        data->session = NULL;
        return -1;
      }
      
      break;
    case REFOP_COLLISION:     /* Handle a refresh collision */

      break;
    case REFOP_TERMINATE:     /* Terminate an existing refresh */
      if (data->reqid > 0)
      { 
        /* SNMP Request was still in progress */
        i_printf (2, "l_snmp_metric_refresh (%s) REFOP_TERMINATE called with SNMP request %i in progress (session %p)", met->desc_str, data->reqid, data->session);
        l_snmp_pducallback_remove_by_reqid (data->session, data->reqid); 
        data->reqid = 0; 
        met->refresh_result = REFRESULT_TOTAL_FAIL;
      }
      if (data->session)
      { l_snmp_session_close (data->session); data->session = NULL; }
      break;
    case REFOP_CLEANDATA:     /* Cleanup the refresh_data struct */
      i_printf (2, "l_snmp_metric_refresh REFOP_CLEANDATA called");
      if (met->refresh_data) 
      { l_snmp_metric_refresh_data_free (met->refresh_data); met->refresh_data = NULL; }
      break;
    default:
      i_printf (1, "l_snmp_metric_refresh unknown opcode received (%i)", opcode);
  }

  return 0;
}

/* SNMP Get Callback */

int l_snmp_metric_refresh_getcb (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata)
{
  int num;
  i_metric *met = passdata;
  l_snmp_metric_refresh_data *data = met->refresh_data;
  i_metric_value *val;

  /* Set the reqid in data to 0 */
  data->reqid = 0;

  /* Check Session Error / PDU Type */
  if (session->error != SNMP_ERROR_NOERROR || !pdu || !pdu->variables)
  {
    /* Error occurred or NULL PDU received */
    if (session->error != SNMP_ERROR_NOERROR)
    { 
      i_printf (2, "l_snmp_metric_refresh_getcb failed, SNMP %s occurred during refresh of %s %s",
        l_snmp_session_error_str (session), i_entity_typestr (met->ent_type), met->name_str);
    }
    else
    {
      i_printf (2, "l_snmp_metric_refresh_getcb failed, no SNMP PDU received for %s %s",
        i_entity_typestr (met->ent_type), met->name_str);
    }

    /* Set refresh result */
    if (session->error == SNMP_ERROR_TIMEOUT)
    { met->refresh_result = REFRESULT_TIMEOUT; }
    else
    { met->refresh_result = REFRESULT_TOTAL_FAIL; }

    /* Terminate the refresh */
    i_entity_refresh_terminate (ENTITY(met));
  
    return 0;
  }

  /* Set Refresh State */
  met->refresh_result = REFRESULT_OK;
  
  /* If variable is not ASN_NULL, perform
   * metric_type-specific handling of the 
   * value
   */

  if (pdu->variables->type != ASN_NULL)
  {
    int wrongtype_flag = 0;
    char *wrongtype_str = NULL;
    
    /* Create the value struct */
    val = i_metric_value_create ();

    /* Set the value according to the metrics type */
    switch (met->met_type)
    {
      case METRIC_INTEGER:
        if (pdu->variables->val.integer)
        { 
          val->integer = (long) *pdu->variables->val.integer; 
          check_overflow_signed (&val->integer);
        }
        else
        { wrongtype_flag = 1; wrongtype_str = "NULL val.integer in PDU"; }
        break;
      
      case METRIC_COUNT:
        if (pdu->variables->val.integer)
        { 
          val->count = (unsigned long) *pdu->variables->val.integer; 
          check_overflow_unsigned (&val->count);
        }
        else
        { wrongtype_flag = 1; wrongtype_str = "NULL val.integer in PDU"; }
        break;
      
      case METRIC_COUNT_HEX64:
        if (pdu->variables->type == ASN_OCTET_STR || pdu->variables->val_len == 8)
        {
          char *num_str = l_snmp_var_to_hexnumstr (pdu->variables);
          if (num_str) 
          {
            val->count64 = strtoull (num_str, NULL, 16);
            free (num_str);
          }
        }
        else
        { wrongtype_flag = 1; wrongtype_str = "No hex string found in PDU"; }
        break;

      case METRIC_COUNT64:
        if (pdu->variables->type == ASN_COUNTER64)
        {
          val->count64 = ((unsigned long long) pdu->variables->val.counter64->high << 32);
          val->count64 += (unsigned long long) pdu->variables->val.counter64->low;
        }
        else
        { wrongtype_flag = 1; wrongtype_str = "Wrong PDU type for Counter64"; }
        break;
      
      case METRIC_GAUGE:
        if (pdu->variables->val.integer)
        { 
          val->gauge = (unsigned long) *pdu->variables->val.integer;
//          if (strcmp(met->name_str, "size")==0 && strcmp(met->obj->cnt->name_str, "storage")==0) 
//          { val->gauge = 2684270646L; }
          check_overflow_unsigned (&val->gauge);
        }
        else
        { wrongtype_flag = 1; wrongtype_str = "NULL val.integer in PDU"; }
        break;

      case METRIC_GAUGE_HEX64:
        if (pdu->variables->type == ASN_OCTET_STR || pdu->variables->val_len == 8)
        { 
          char *num_str = l_snmp_var_to_hexnumstr (pdu->variables); 
          val->gauge64 = strtoull (num_str, NULL, 16);
          free (num_str);
        }
        else
        { wrongtype_flag = 1; wrongtype_str = "No hex string found in PDU"; }
        break;

      case METRIC_INTERVAL:
        if (pdu->variables->val.integer)
        {
          val->tv.tv_sec = (unsigned long) *pdu->variables->val.integer / 100;
          val->tv.tv_usec = (unsigned long) (*pdu->variables->val.integer % 100) * 10000;
        }  
        else
        { wrongtype_flag = 1; wrongtype_str = "NULL val.integer in PDU"; }
        break;

      case METRIC_STRING:
        val->str = l_snmp_var_to_str (pdu->variables);
        if (!val->str)
        { wrongtype_flag = 1; wrongtype_str = "No string found in PDU"; }
        break;

      case METRIC_HEXSTRING:
        val->str = l_snmp_var_to_hexstr (pdu->variables);
        if (!val->str)
        { wrongtype_flag = 1; wrongtype_str = "No hex string found in PDU"; }

      case METRIC_OID:
        if (pdu->variables->val.objid)
        {
          val->oid = malloc (pdu->variables->val_len);
          memcpy (val->oid, pdu->variables->val.objid, pdu->variables->val_len);
          val->oid_len = pdu->variables->val_len;
        }
        else
        { wrongtype_flag = 1; wrongtype_str = "No OID found in PDU"; }
        break;

    case METRIC_IP:
        if (pdu->variables->val.string)
        {
          val->ip = i_ip_create ();
          asprintf (&val->ip->str, "%d.%d.%d.%d", 
            (u_char) pdu->variables->val.string[0], (u_char) pdu->variables->val.string[1],
            (u_char) pdu->variables->val.string[2], (u_char) pdu->variables->val.string[3]);
          val->ip->addr.s_addr = inet_addr (val->ip->str);
        }
        else
        { wrongtype_flag = 1; wrongtype_str = "No IP address found in PDU"; }
        break;

    default:
        wrongtype_flag = 1; wrongtype_str = "Unsupported metric type"; 
    }

    if (wrongtype_flag == 1)
    {
      i_printf (2, "l_snmp_metric_refresh_getcb warning, met_type (%s) for %s %s may not be correct; %s",
        i_metric_typestr (met->met_type), i_entity_typestr (met->ent_type), met->name_str, wrongtype_str);
      i_metric_value_free (val);
      val = NULL;
    }

    /* If the value still exists enqueue it */ 
    if (val)
    {
      /* Enqueue */
      num = i_metric_value_enqueue (self, met, val);
      if (num != 0)
      { 
        i_printf (1, "l_snmp_metric_refresh_getcb failed to enqueue value to %s %s",
          i_entity_typestr (met->ent_type), met->name_str);
        i_metric_value_free (val);
        met->refresh_result = REFRESULT_TOTAL_FAIL;
      }
    }
  }

  /* Terminate the refresh */
  i_entity_refresh_terminate (ENTITY(met));
  
  return 0;
}
