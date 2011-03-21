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

#include "vsvr.h"

static int static_enabled = 0;
static i_timer *static_refresh_timer = NULL;
static int static_refresh_progress = 0;
static int static_refresh_inprogress = 0;
static int static_refresh_collisions = 0;
static l_snmp_session *static_snmp_session = NULL;
static v_vsvr *static_current_vsvr = NULL;

#define DEFAULT_REFRESH_SEC 30

#define REFRESH_PROGRESS_ADMIN_STATE 1
#define REFRESH_PROGRESS_COMPLETE 1

#define PORT_REFRESH_PROGRESS_ADMIN_STATE 1
#define PORT_REFRESH_PROGRESS_COMPLETE 1

/* Enable / Disable */

int v_vsvr_refresh_enable (i_resource *self)
{
  if (static_enabled == 1)
  { i_printf (1, "v_vsvr_refresh_enable warning, v_vsvr_refresh already enabled"); return 0; }

  static_enabled = 1;

  static_refresh_timer = i_timer_add (self, DEFAULT_REFRESH_SEC, 0, v_vsvr_refresh_timer_callback, NULL);
  if (!static_refresh_timer)
  { i_printf (1, "v_vsvr_refresh_enable failed to add refresh timer"); v_vsvr_refresh_disable (self); return -1; }

  return 0;
}

int v_vsvr_refresh_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "v_vsvr_refresh_disable warning, v_vsvr_refresh already disabled"); return 0; }

  static_enabled = 0;

  if (static_refresh_timer)
  { i_timer_remove (static_refresh_timer); static_refresh_timer = NULL; }

  if (static_refresh_inprogress == 1)
  { v_vsvr_refresh_terminate (self); }

  return 0;
}

/* Refresh timer callback */

int v_vsvr_refresh_timer_callback (i_resource *self, i_timer *timer, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the timer active */
  int num;
  i_list *vsvr_list;

  /* Check refresh collisions */
  if (static_refresh_inprogress == 1)
  {
    /* Refresh collision */
    static_refresh_collisions++;
    if (static_refresh_collisions == 3)
    {
      v_vsvr_refresh_failed (self, "3 refresh collisions occurred"); 
      return 0;
    }
    return 0;
  }
  else
  { static_refresh_collisions = 0; }

  /* Get vsvr list */
  vsvr_list = v_vsvr_list ();
  if (!vsvr_list)
  { v_vsvr_list_populate (self); return 0; }

  /* Open SNMP session */
  static_snmp_session = l_snmp_session_open (self, self->hierarchy->device->ip_str, self->hierarchy->device->snmp_community);
  if (!static_snmp_session)
  { v_vsvr_refresh_failed (self, "Failed to open SNMP session"); return 0; }

  /* Refresh next (first) */
  static_current_vsvr = NULL;
  num = v_vsvr_refresh_next (self);
  if (num != 0)
  { v_vsvr_refresh_failed (self, "Failed to refresh first vsvr"); return 0; }

  return 0;
}

/* Refresh next */

int v_vsvr_refresh_next (i_resource *self)
{
  /* Return -1 on error, 0 on refresh in progress 
   * and 1 on nothing more to refresh
   */
  int num;
  char *oid_str;
  v_vsvr *vsvr;
  v_vsvr_port *port;
  i_list *vsvr_list;

  static_refresh_progress = 0;

  /* Get vsvr list */
  vsvr_list = v_vsvr_list ();
  if (!vsvr_list)
  { i_printf (1, "v_vsvr_refresh_next failed to retrieve vsvr_list"); return -1; }

  /* Find next vsvr list */
  if (static_current_vsvr)
  {
    v_vsvr *temp_vsvr;
    temp_vsvr = i_list_restore (vsvr_list);
    if (temp_vsvr != static_current_vsvr)
    { 
      num = i_list_search (vsvr_list, static_current_vsvr);
      if (num != 0)
      { i_printf (1, "v_vsvr_refresh_next failed to find static_current_vsvr in vsvr_list"); static_current_vsvr = NULL; return -1; }
    }
    i_list_move_next (vsvr_list);

    if (vsvr_list->p == vsvr_list->end) 
    { static_current_vsvr = NULL; return 1; }   /* End reached, nothing to refresh */
  }
  else
  { i_list_move_head (vsvr_list); }

  vsvr = i_list_restore (vsvr_list);
  if (!vsvr)
  { i_printf (1, "v_vsvr_refresh_next failed to restore vsvr to refresh"); static_current_vsvr = NULL; return -1; }
  static_current_vsvr = vsvr;

  /* Perform SNMP get operations */

  /* Admin State */
  asprintf (&oid_str, "enterprises.1991.1.1.4.21.1.1.3.%s", inet_ntoa (vsvr->ip_addr));
  vsvr->admin_state_reqid = l_snmp_get_oid (self, static_snmp_session, oid_str, v_vsvr_refresh_snmp_callback, vsvr);
  free (oid_str);
  if (vsvr->admin_state_reqid == -1)
  { i_printf (1, "v_vsvr_refresh failed to send request for admin state for vsvr %s", vsvr->name_str); return -1; }

  /* Ports */
  for (i_list_move_head(vsvr->port_list); (port=i_list_restore(vsvr->port_list))!=NULL; i_list_move_next(vsvr->port_list))
  {
    port->refresh_progress = 0;
    port->refresh_inprogress = 1;

    /* Admin State */
    asprintf (&oid_str, "enterprises.1991.1.1.4.22.1.1.4.%s.%lu", inet_ntoa (vsvr->ip_addr), port->port);
    port->admin_state_reqid = l_snmp_get_oid (self, static_snmp_session, oid_str, v_vsvr_refresh_portsnmp_callback, port);
    free (oid_str);
    if (port->admin_state_reqid == -1)
    { i_printf (1, "v_vsvr_refresh failed to send request for port %lu admin state for vsvr %s", port->port, vsvr->name_str); return -1; }
  }

  return 0;
}

/* SNMP Callback */

int v_vsvr_refresh_snmp_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata)
{
  int num;
  char *str;
  char *long_str;
  v_vsvr *vsvr = passdata;
  
  if (reqid == vsvr->admin_state_reqid)
  {
    vsvr->admin_state_reqid = 0;
    if (pdu->variables->type == ASN_INTEGER && pdu->variables->val.integer)
    { 
      vsvr->admin_state = *pdu->variables->val.integer; 
      vsvr->admin_state_current = 1;
      asprintf (&str, "vsvr_%s_adminstate", vsvr->name_str);
      switch (vsvr->admin_state)
      {
        case 0: asprintf (&long_str, "Virtual server %s is administratively disabled", vsvr->name_str);
                i_status_set (self, STATUS_ALERT, str, long_str);
                free (long_str);
                break;
        case 1: i_status_clear (self, str);
                break;
        default: asprintf (&long_str, "Virtual server %s is in an unknown administrative state (%i)", vsvr->name_str, vsvr->admin_state);
                i_status_set (self, STATUS_ALERT, str, long_str);
                free (long_str);
                break;
      }
      free (str);
    }
    else
    { 
      if (session->error == SNMP_ERROR_TIMEOUT)
      { i_printf (1, "v_vsvr_refresh_portsnmp_callback SNMP timeout occurred for admin state of %s", vsvr->name_str); } 
      vsvr->admin_state_current = 0; 
    }
    static_refresh_progress += REFRESH_PROGRESS_ADMIN_STATE;
  }

  if (static_refresh_progress == REFRESH_PROGRESS_COMPLETE)
  {
    /* Rsvr refresh complete */
    num = v_vsvr_refresh_next (self);
    if (num != 0)
    {
      if (num == 1)
      { v_vsvr_refresh_finished (self); }
      else
      { 
        char *str;
        asprintf (&str, "Failed to refresh next virtual server after %s", vsvr->name_str);
        v_vsvr_refresh_failed (self, str);
        free (str);
        return 0;
      }
    }
  }

  return 0;
}

int v_vsvr_refresh_portsnmp_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata)
{
  char *str;
  char *long_str;
  v_vsvr_port *port = passdata;

  if (reqid == port->admin_state_reqid)
  {
    port->admin_state_reqid = 0;
    if (pdu->variables->type == ASN_INTEGER && pdu->variables->val.integer)
    { 
      port->admin_state = *pdu->variables->val.integer; 
      port->admin_state_current = 1; 
      asprintf (&str, "vsvr_%s_port_%lu_adminstate", port->vsvr->name_str, port->port);
      switch (port->admin_state)
      {
        case 0: asprintf (&long_str, "Virtual server %s port %lu is administratively disabled", port->vsvr->name_str, port->port);
                i_status_set (self, STATUS_ALERT, str, long_str);
                free (long_str);
                break;
        case 1: i_status_clear (self, str);
                break;
        default: asprintf (&long_str, "Virtual server %s port %lu is in an unknown administrative state (%i)", port->vsvr->name_str, port->port, port->vsvr->admin_state);
                i_status_set (self, STATUS_ALERT, str, long_str);
                free (long_str);
                break;
      }
      free (str);
    }
    else
    {
      if (session->error == SNMP_ERROR_TIMEOUT)
      { i_printf (1, "v_vsvr_refresh_portsnmp_callback SNMP timeout occurred for admin state of %s's port %lu", port->vsvr->name_str, port->port); } 
      port->admin_state_current = 0; 
    }
    port->refresh_progress += PORT_REFRESH_PROGRESS_ADMIN_STATE;
  }

  if (port->refresh_progress == PORT_REFRESH_PROGRESS_COMPLETE)
  {
    /* Port refresh complete */
    port->refresh_inprogress = 0;
  }

  return 0;
}

/* Finishied/Failed/Terminate */

int v_vsvr_refresh_terminate (i_resource *self)
{
  /* Terminate current refresh */
  static_refresh_inprogress = 0;
  static_refresh_progress = 0;
  
  if (static_current_vsvr)
  {
    if (static_current_vsvr->admin_state_reqid > 0)
    { l_snmp_pducallback_remove_by_reqid (static_snmp_session, static_current_vsvr->admin_state_reqid); static_current_vsvr->admin_state_current = 0; }
    static_current_vsvr = NULL; 
  }

  if (static_snmp_session)
  { l_snmp_session_close (static_snmp_session); static_snmp_session = NULL; }

  return 0;
}

int v_vsvr_refresh_finished (i_resource *self)
{
  i_status_clear (self, "vsvr_refresh");
  v_vsvr_refresh_terminate (self);
  return 0;
}

int v_vsvr_refresh_failed (i_resource *self, char *str)
{
  i_printf (1, "v_vsvr_refresh_failed %s", str);
  i_status_set (self, STATUS_CRITICAL, "vsvr_refresh", str);
  v_vsvr_refresh_terminate (self);
  return 0;
}

