#include <stdlib.h>
#include <string.h>

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
#include "induction/interface.h"
#include "device/snmp.h"
#include "device/snmp_iflist.h"

#include "pix.h"

/* Fix for PIX OS 6.2 incorrect implementation
 * of the ifLastChange variable. The ifLastChange 
 * variable should contain the value of sysUpTime.0 
 * at the time of the last change. However, the PIX OS
 * at version 6.2 (if not others) sets this value to 
 * the absolute time since the last change. This system 
 * sets interface->lastchange_uptime_current to -1 such 
 * that the snmp_iflist system doesn't refresh the
 * lastchange value anymore. A timer is installed 
 * to update interface->lastchange_uptime manually.
 */

static int static_enabled = 0;
static int static_reqid = 0;
static int static_reqcount = 0;
static int static_refresh_inprogress = 0;
static int static_refresh_collisions = 0;
static i_timer *static_refresh_timer = NULL;
static l_snmp_session *static_snmp_session = NULL;

/* Enable/Disable */

int v_pix_iflist_refresh_enable (i_resource *self)
{
  l_snmp_iflist_config *config;
  
  if (static_enabled == 1)
  { i_printf (1, "v_pix_iflist_refresh_enable warning, sub-system already enabled"); return 0; }

  static_enabled = 1;

  config = l_snmp_iflist_config_load (self);
  if (!config)
  { i_printf (1, "v_pix_iflist_refresh_enable failed to load l_snmp_iflist config"); v_pix_iflist_refresh_disable (self); return -1; }

  static_refresh_timer = i_timer_add (self, config->refresh_seconds, 0, v_pix_iflist_refresh_timer_callback, NULL);
  l_snmp_iflist_config_free (config);
  if (!static_refresh_timer)
  { i_printf (1, "v_pix_iflist_refresh_enable failed to add refresh timer"); v_pix_iflist_refresh_disable (self); return -1; }

  return 0;
}

int v_pix_iflist_refresh_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "v_pix_iflist_refresh_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0; 

  if (static_refresh_timer)
  { i_timer_remove (static_refresh_timer); static_refresh_timer = NULL; }
  if (static_snmp_session)
  { l_snmp_session_close (static_snmp_session); static_snmp_session = NULL; }

  return 0;
}

/* Refresh */

int v_pix_iflist_refresh_timer_callback (i_resource *self, i_timer *timer, void *passdata)
{
  i_list *list;
  i_interface *interface;
  char *oid_str;

  /* Check for collision */
  if (static_refresh_inprogress == 1)
  {
    /* Collision */
    static_refresh_collisions++;
    if (static_refresh_collisions == 3)
    {
      /* 3 consecutive, reset refresh */ 
      i_printf (1, "v_pix_iflist_refresh_timer_callback 3 consecutive refresh collisions occurred");
      if (static_snmp_session)
      { l_snmp_session_close (static_snmp_session); }
      static_snmp_session = NULL;
      static_refresh_collisions = 0;
    }
  }
  else
  { static_refresh_inprogress = 1; static_refresh_collisions = 0; }
  
  /* Open SNMP session */
  static_snmp_session = l_snmp_session_open_device (self, self->hierarchy->dev);
  if (!static_snmp_session)
  { i_printf (1, "v_pix_iflist_refresh_timer_callback failed to open SNMP session to %s", self->hierarchy->device->ip_str); return -1; }

  /* Send requests */
  static_reqcount = 0;
  list = l_snmp_iflist_list ();
  for (i_list_move_head(list); (interface=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    interface->lastchange_uptime_current = -1;  /* Stop snmp_iflist from refreshing it */
    interface->lastchange_calculated = 1;       /* Stop snmp_iflist from calculating lastchange */
    if (interface->lastchange_uptime_reqid != 0)
    {
      /* Collision. snmp_iflist is refreshing this value, 
       * and we are trying to refresh it. Cancel snmp_iflists one
       * and be sure to update refresh->progress
       */
      l_snmp_pducallback_remove_by_reqid ((l_snmp_session *) interface->snmp_sessionptr, interface->lastchange_uptime_reqid);
      interface->lastchange_uptime_reqid = 0;
      interface->refresh_progress += 8192;  /* 8192 == GET_PROGRESS_LASTCHANGE */
    }

    /* Request lastchange */
    asprintf (&oid_str, "ifLastChange.%li", interface->id);
    static_reqid = l_snmp_get_oid (self, static_snmp_session, oid_str, v_pix_iflist_refresh_snmp_callback, interface);
    free (oid_str);
    if (static_reqid == -1)
    {
      i_printf (1, "v_pix_iflist_refresh_timer_callback failed to send a request (ifLastChange)");
      interface->lastchange_sec_current = 0;
      interface->lastchange_current = 0;
    }
    else
    { static_reqcount++; }
  }

  /* Check success count */
  if (static_reqcount == 0)
  { l_snmp_session_close (static_snmp_session); }

  return 0;
}

int v_pix_iflist_refresh_snmp_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata)
{
  i_interface *interface = passdata;
  
  static_reqid = 0;
  static_reqcount--;

  /* Interpret PDU */
  if (pdu && pdu->variables && pdu->variables->type == ASN_TIMETICKS)
  {
    struct timeval now;
    
    interface->lastchange_sec = *pdu->variables->val.integer / 100; 
    interface->lastchange_sec_current = 1;

    gettimeofday (&now, NULL);
    interface->lastchange.tv_sec = now.tv_sec - interface->lastchange_sec;
    interface->lastchange.tv_usec = 0;
    interface->lastchange_current = 1;
  }
  else
  {
    if (session->error == SNMP_ERROR_TIMEOUT)
    { i_printf (1, "v_pix_iflist_refresh_snmp_callback SNMP timeout occurred while refreshing last change time for interface %i (%s)", interface->id, interface->desc_str); }
    else if (pdu->errstat == 2) /* Unsupported OID */
    { 
      interface->lastchange_sec = 0;
      interface->lastchange_sec_current = -1;
      interface->lastchange.tv_sec = 0;
      interface->lastchange.tv_usec = 0;
      interface->lastchange_current = 0;
    }
    else      
    { 
      i_printf (1, "v_pix_iflist_refresh_snmp_callback SNMP error (non-timeout) occurred whle refreshing last change time for interface %i (%s)",  interface->id, interface->desc_str);
      interface->lastchange_sec_current = 0;
      interface->lastchange_current = 0;
    }  
  }     

  /* Check if all requests are finished */
  if (static_reqcount == 0)
  { 
    /* Finished all reqs */
    l_snmp_session_close (static_snmp_session); 
    static_snmp_session = NULL;
    static_refresh_inprogress = 0;
  }

  return 0;
}
