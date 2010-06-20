#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

static int static_enabled = 0;
static i_timer *static_refresh_timer = NULL;
static int static_refresh_progress = 0;
static int static_refresh_inprogress = 0;
static int static_refresh_collisions = 0;
static l_snmp_session *static_snmp_session = NULL;
static v_rsvr *static_current_rsvr = NULL;

#define DEFAULT_REFRESH_SEC 30

#define REFRESH_PROGRESS_ADMIN_STATE 1
#define REFRESH_PROGRESS_OP_STATE 2
#define REFRESH_PROGRESS_COMPLETE 3

#define PORT_REFRESH_PROGRESS_ADMIN_STATE 1
#define PORT_REFRESH_PROGRESS_OP_STATE 2
#define PORT_REFRESH_PROGRESS_COMPLETE 3

/* Enable / Disable */

int v_rsvr_refresh_enable (i_resource *self)
{
  if (static_enabled == 1)
  { i_printf (1, "v_rsvr_refresh_enable warning, v_rsvr_refresh already enabled"); return 0; }

  static_enabled = 1;

  static_refresh_timer = i_timer_add (self, DEFAULT_REFRESH_SEC, 0, v_rsvr_refresh_timer_callback, NULL);
  if (!static_refresh_timer)
  { i_printf (1, "v_rsvr_refresh_enable failed to add refresh timer"); v_rsvr_refresh_disable (self); return -1; }

  return 0;
}

int v_rsvr_refresh_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "v_rsvr_refresh_disable warning, v_rsvr_refresh already disabled"); return 0; }

  static_enabled = 0;

  if (static_refresh_timer)
  { i_timer_remove (static_refresh_timer); static_refresh_timer = NULL; }

  if (static_refresh_inprogress == 1)
  { v_rsvr_refresh_terminate (self); }

  return 0;
}

/* Refresh timer callback */

int v_rsvr_refresh_timer_callback (i_resource *self, i_timer *timer, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the timer active */
  int num;
  i_list *rsvr_list;

  /* Check refresh collisions */
  if (static_refresh_inprogress == 1)
  {
    /* Refresh collision */
    static_refresh_collisions++;
    if (static_refresh_collisions == 3)
    {
      v_rsvr_refresh_failed (self, "3 refresh collisions occurred"); 
      return 0;
    }
    return 0;
  }
  else
  { static_refresh_collisions = 0; }

  /* Get rsvr list */
  rsvr_list = v_rsvr_list ();
  if (!rsvr_list)
  { v_rsvr_list_populate (self); return 0; }

  /* Open SNMP session */
  static_snmp_session = l_snmp_session_open (self, self->hierarchy->device->ip_str, self->hierarchy->device->snmp_community);
  if (!static_snmp_session)
  { v_rsvr_refresh_failed (self, "Failed to open SNMP session"); return 0; }

  /* Refresh next (first) */
  static_current_rsvr = NULL;
  num = v_rsvr_refresh_next (self);
  if (num != 0)
  { v_rsvr_refresh_failed (self, "Failed to refresh first rsvr"); return 0; }

  return 0;
}

/* Refresh next */

int v_rsvr_refresh_next (i_resource *self)
{
  /* Return -1 on error, 0 on refresh in progress 
   * and 1 on nothing more to refresh
   */
  int num;
  char *oid_str;
  v_rsvr *rsvr;
  v_rsvr_port *port;
  i_list *rsvr_list;

  static_refresh_progress = 0;

  /* Get rsvr list */
  rsvr_list = v_rsvr_list ();
  if (!rsvr_list)
  { i_printf (1, "v_rsvr_refresh_next failed to retrieve rsvr_list"); return -1; }

  /* Find next rsvr list */
  if (static_current_rsvr)
  {
    v_rsvr *temp_rsvr;
    temp_rsvr = i_list_restore (rsvr_list);
    if (temp_rsvr != static_current_rsvr)
    { 
      num = i_list_search (rsvr_list, static_current_rsvr);
      if (num != 0)
      { i_printf (1, "v_rsvr_refresh_next failed to find static_current_rsvr in rsvr_list"); static_current_rsvr = NULL; return -1; }
    }
    i_list_move_next (rsvr_list);

    if (rsvr_list->p == rsvr_list->end) 
    { static_current_rsvr = NULL; return 1; }   /* End reached, nothing to refresh */
  }
  else
  { i_list_move_head (rsvr_list); }

  rsvr = i_list_restore (rsvr_list);
  if (!rsvr)
  { i_printf (1, "v_rsvr_refresh_next failed to restore rsvr to refresh"); static_current_rsvr = NULL; return -1; }
  static_current_rsvr = rsvr;

  /* Perform SNMP get operations */

  /* Admin State */
  asprintf (&oid_str, "enterprises.1991.1.1.4.19.1.1.3.%s", inet_ntoa (rsvr->ip_addr));
  rsvr->admin_state_reqid = l_snmp_get_oid (self, static_snmp_session, oid_str, v_rsvr_refresh_snmp_callback, rsvr);
  free (oid_str);
  if (rsvr->admin_state_reqid == -1)
  { i_printf (1, "v_rsvr_refresh failed to send request for admin state for rsvr %s", rsvr->name_str); return -1; }

  /* Operational State */
  asprintf (&oid_str, "enterprises.1991.1.1.4.23.1.1.8.%s", inet_ntoa (rsvr->ip_addr));
  rsvr->op_state_reqid = l_snmp_get_oid (self, static_snmp_session, oid_str, v_rsvr_refresh_snmp_callback, rsvr);
  free (oid_str);
  if (rsvr->op_state_reqid == -1)
  { i_printf (1, "v_rsvr_refresh failed to send request for op state for rsvr %s", rsvr->name_str); return -1; }

  /* Ports */
  for (i_list_move_head(rsvr->port_list); (port=i_list_restore(rsvr->port_list))!=NULL; i_list_move_next(rsvr->port_list))
  {
    port->refresh_progress = 0;
    port->refresh_inprogress = 1;

    /* Open SNMP session */
    port->snmp_session = l_snmp_session_open (self, self->hierarchy->device->ip_str, self->hierarchy->device->snmp_community);
    if (!port->snmp_session)
    { 
      i_printf (1, "v_rsvr_refresh failed to open SNMP session for server %s port %lu", rsvr->name_str, port->port);
      port->admin_state_current = 0;
      port->op_state_current = 0;
      continue;
    }

    /* Admin State */
    asprintf (&oid_str, "enterprises.1991.1.1.4.20.1.1.4.%s.%lu", inet_ntoa (rsvr->ip_addr), port->port);
    port->admin_state_reqid = l_snmp_get_oid (self, port->snmp_session, oid_str, v_rsvr_refresh_portsnmp_callback, port);
    free (oid_str);
    if (port->admin_state_reqid == -1)
    { i_printf (1, "v_rsvr_refresh failed to send request for port %lu admin state for rsvr %s", port->port, rsvr->name_str); return -1; }
    
    /* Operational State */
    asprintf (&oid_str, "enterprises.1991.1.1.4.24.1.1.5.%s.%lu", inet_ntoa (rsvr->ip_addr), port->port);
    port->op_state_reqid = l_snmp_get_oid (self, port->snmp_session, oid_str, v_rsvr_refresh_portsnmp_callback, port);
    free (oid_str);
    if (port->op_state_reqid == -1)
    { i_printf (1, "v_rsvr_refresh failed to send request for port %lu op state for rsvr %s", port->port, rsvr->name_str); return -1; }
  }

  return 0;
}

/* SNMP Callback */

int v_rsvr_refresh_snmp_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata)
{
  int num;
  char *str;
  char *long_str;
  v_rsvr *rsvr = passdata;
  
  if (reqid == rsvr->admin_state_reqid)
  {
    rsvr->admin_state_reqid = 0;
    if (pdu->variables->type == ASN_INTEGER && pdu->variables->val.integer)
    { 
      rsvr->admin_state = *pdu->variables->val.integer; 
      rsvr->admin_state_current = 1;
      asprintf (&str, "rsvr_%s_adminstate", rsvr->name_str);
      switch (rsvr->admin_state)
      {
        case 0: asprintf (&long_str, "Real server %s is administratively disabled", rsvr->name_str);
                i_status_set (self, STATUS_ALERT, str, long_str);
                free (long_str);
                break;
        case 1: i_status_clear (self, str);
                break;
        default: asprintf (&long_str, "Real server %s is in an unknown administrative state (%i)", rsvr->name_str, rsvr->admin_state);
                i_status_set (self, STATUS_ALERT, str, long_str);
                free (long_str);
                break;
      }
      free (str);
    }
    else
    { 
      if (session->error == SNMP_ERROR_TIMEOUT)
      { i_printf (1, "v_rsvr_refresh_portsnmp_callback SNMP timeout occurred for admin state of %s", rsvr->name_str); } 
      rsvr->admin_state_current = 0; 
    }
    static_refresh_progress += REFRESH_PROGRESS_ADMIN_STATE;
  }

  else if (reqid == rsvr->op_state_reqid)
  {
    rsvr->op_state_reqid = 0;
    if (pdu->variables->type == ASN_INTEGER && pdu->variables->val.integer)
    { 
      rsvr->op_state = *pdu->variables->val.integer; 
      rsvr->op_state_current = 1;
      asprintf (&str, "rsvr_%s_opstate", rsvr->name_str);
      switch (rsvr->op_state)
      {
        case 0: asprintf (&long_str, "Real server %s is disabled", rsvr->name_str);
                i_status_set (self, STATUS_ALERT, str, long_str);
                free (long_str);
                break;
        case 1: asprintf (&long_str, "Real server %s is configured by not connected", rsvr->name_str);
                i_status_set (self, STATUS_ALERT, str, long_str);
                free (long_str);
                break;
        case 2: asprintf (&long_str, "Real server %s has failed", rsvr->name_str);
                i_status_set (self, STATUS_CRITICAL, str, long_str);
                free (long_str);
                break;
        case 3: asprintf (&long_str, "Real server %s has failed and is being tested", rsvr->name_str);
                i_status_set (self, STATUS_CRITICAL, str, long_str);
                free (long_str);
                break;
        case 4: asprintf (&long_str, "Real server %s's performance is degraded and suspect of a failure", rsvr->name_str);
                i_status_set (self, STATUS_CRITICAL, str, long_str);
                free (long_str);
                break;
        case 5: asprintf (&long_str, "Real server %s is shutdown", rsvr->name_str);
                i_status_set (self, STATUS_ALERT, str, long_str);
                free (long_str);
                break;
        case 6: i_status_clear (self, str);
                break;
        default: asprintf (&long_str, "Real server %s is in an unknown state (%i)", rsvr->name_str, rsvr->op_state);
                i_status_set (self, STATUS_ALERT, str, long_str);
                free (long_str);
                break;
      }         
      free (str);
    }
    else
    { 
      if (session->error == SNMP_ERROR_TIMEOUT)
      { i_printf (1, "v_rsvr_refresh_portsnmp_callback SNMP timeout occurred for op state of %s", rsvr->name_str); } 
      rsvr->op_state_current = 0; 
    }
    static_refresh_progress += REFRESH_PROGRESS_OP_STATE;
  }

  if (static_refresh_progress == REFRESH_PROGRESS_COMPLETE)
  {
    /* Rsvr refresh complete */
    num = v_rsvr_refresh_next (self);
    if (num != 0)
    {
      if (num == 1)
      { v_rsvr_refresh_finished (self); }
      else
      { 
        char *str;
        asprintf (&str, "Failed to refresh next real server after %s", rsvr->name_str);
        v_rsvr_refresh_failed (self, str);
        free (str);
        return 0;
      }
    }
  }

  return 0;
}

int v_rsvr_refresh_portsnmp_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata)
{
  char *str;
  char *long_str;
  v_rsvr_port *port = passdata;

  if (reqid == port->admin_state_reqid)
  {
    port->admin_state_reqid = 0;
    if (pdu->variables->type == ASN_INTEGER && pdu->variables->val.integer)
    { 
      port->admin_state = *pdu->variables->val.integer; 
      port->admin_state_current = 1; 
      asprintf (&str, "rsvr_%s_port_%lu_adminstate", port->rsvr->name_str, port->port);
      switch (port->admin_state)
      {
        case 0: asprintf (&long_str, "Real server %s port %lu is administratively disabled", port->rsvr->name_str, port->port);
                i_status_set (self, STATUS_ALERT, str, long_str);
                free (long_str);
                break;
        case 1: i_status_clear (self, str);
                break;
        default: asprintf (&long_str, "Real server %s port %lu is in an unknown administrative state (%i)", port->rsvr->name_str, port->port, port->rsvr->admin_state);
                i_status_set (self, STATUS_ALERT, str, long_str);
                free (long_str);
                break;
      }
      free (str);
    }
    else
    {
      if (session->error == SNMP_ERROR_TIMEOUT)
      { i_printf (1, "v_rsvr_refresh_portsnmp_callback SNMP timeout occurred for admin state of %s's port %lu", port->rsvr->name_str, port->port); } 
      port->admin_state_current = 0; 
    }
    port->refresh_progress += PORT_REFRESH_PROGRESS_ADMIN_STATE;
  }

  else if (reqid == port->op_state_reqid)
  {
    port->op_state_reqid = 0;
    if (pdu->variables->type == ASN_INTEGER && pdu->variables->val.integer)
    { 
      port->op_state = *pdu->variables->val.integer; 
      port->op_state_current = 1; 
      asprintf (&str, "rsvr_%s_port_%lu_opstate", port->rsvr->name_str, port->port);
      switch (port->op_state)
      {
        case 0: asprintf (&long_str, "Real server %s port %lu is disabled", port->rsvr->name_str, port->port);
                i_status_set (self, STATUS_ALERT, str, long_str);
                free (long_str);
                break;
        case 1: asprintf (&long_str, "Real server %s port %lu is configured by not connected", port->rsvr->name_str, port->port);
                i_status_set (self, STATUS_ALERT, str, long_str);
                free (long_str);
                break;
        case 2: asprintf (&long_str, "Real server %s port %lu has failed", port->rsvr->name_str, port->port);
                i_status_set (self, STATUS_CRITICAL, str, long_str);
                free (long_str);
                break;
        case 3: asprintf (&long_str, "Real server %s port %lu has failed and is being tested", port->rsvr->name_str, port->port);
                i_status_set (self, STATUS_CRITICAL, str, long_str);
                free (long_str);
                break;
        case 4: asprintf (&long_str, "Real server %s port %lu's performance is degraded and suspect of a failure", port->rsvr->name_str, port->port);
                i_status_set (self, STATUS_CRITICAL, str, long_str);
                free (long_str);
                break;
        case 5: asprintf (&long_str, "Real server %s port %lu is shutdown", port->rsvr->name_str, port->port);
                i_status_set (self, STATUS_ALERT, str, long_str);
                free (long_str);
                break;
        case 6: i_status_clear (self, str);
                break;
        default: asprintf (&long_str, "Real server %s port %lu is in an unknown state (%i)", port->rsvr->name_str, port->port, port->rsvr->op_state);
                i_status_set (self, STATUS_ALERT, str, long_str);
                free (long_str);
                break;
      }
      free (str);
    }
    else
    { 
      if (session->error == SNMP_ERROR_TIMEOUT)
      { i_printf (1, "v_rsvr_refresh_portsnmp_callback SNMP timeout occurred for op state of %s's port %lu", port->rsvr->name_str, port->port); } 
      port->op_state_current = 0; 
    }
    port->refresh_progress += PORT_REFRESH_PROGRESS_OP_STATE;
  }

  if (port->refresh_progress == PORT_REFRESH_PROGRESS_COMPLETE)
  {
    /* Port refresh complete */
    port->refresh_inprogress = 0;
    if (port->snmp_session)
    { l_snmp_session_close (port->snmp_session); port->snmp_session = NULL; }
  }

  return 0;
}

/* Finishied/Failed/Terminate */

int v_rsvr_refresh_terminate (i_resource *self)
{
  /* Terminate current refresh */
  static_refresh_inprogress = 0;
  static_refresh_progress = 0;
  
  if (static_current_rsvr)
  {
    if (static_current_rsvr->admin_state_reqid > 0)
    { l_snmp_pducallback_remove_by_reqid (static_snmp_session, static_current_rsvr->admin_state_reqid); static_current_rsvr->admin_state_current = 0; }
    if (static_current_rsvr->op_state_reqid > 0)
    { l_snmp_pducallback_remove_by_reqid (static_snmp_session, static_current_rsvr->op_state_reqid); static_current_rsvr->op_state_current = 0; }
    static_current_rsvr = NULL; 
  }

  if (static_snmp_session)
  { l_snmp_session_close (static_snmp_session); static_snmp_session = NULL; }

  return 0;
}

int v_rsvr_refresh_finished (i_resource *self)
{
  i_status_clear (self, "rsvr_refresh");
  v_rsvr_refresh_terminate (self);
  return 0;
}

int v_rsvr_refresh_failed (i_resource *self, char *str)
{
  i_printf (1, "v_rsvr_refresh_failed %s", str);
  i_status_set (self, STATUS_CRITICAL, "rsvr_refresh", str);
  v_rsvr_refresh_terminate (self);
  return 0;
}

