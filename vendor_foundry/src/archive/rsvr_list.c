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

/* Real server list */

static int static_populate_in_progress = 0;
static int static_walk_id = 0;
static i_list *static_rsvr_list = NULL;
static l_snmp_session *static_snmp_session = NULL;

/* Pointer fetching */

i_list* v_rsvr_list ()
{
  if (static_populate_in_progress == 1)
  { return NULL; }
  return static_rsvr_list;
}

/* List Manipulation */

int v_rsvr_list_populate (i_resource *self)
{
  if (static_populate_in_progress == 1)
  { i_printf (1, "v_rsvr_list_populate warning, populate operation in progress"); return 0; }

  static_populate_in_progress = 1;

  static_rsvr_list = i_list_create ();
  if (!static_rsvr_list)
  { i_printf (1, "v_rsvr_list_populate failed to malloc static_rsvr_list"); v_rsvr_list_populate_failed (self); return -1; }
  i_list_set_destructor (static_rsvr_list, v_rsvr_free);

  static_snmp_session = l_snmp_session_open (self, self->hierarchy->device->ip_str, self->hierarchy->device->snmp_community);
  if (!static_snmp_session)
  { i_printf (1, "v_rsvr_list_populate failed to establish SNMP session to device"); v_rsvr_list_populate_failed (self); return -1; }

  /* Walk for the real server names
   * from snL4RealServerCfgName 
   */
  
  static_walk_id = l_snmp_walk (self, static_snmp_session, "enterprises.1991.1.1.4.19.1.1.2", 0, v_rsvr_list_populate_serverwalk_callback, NULL); 
  if (static_walk_id == -1)
  { i_printf (1, "v_rsvr_list_populate failed to walk snL4RealServerCfgName OID tree"); v_rsvr_list_populate_failed (self); return -1; }

  return 0;
}

int v_rsvr_list_populate_serverwalk_callback (i_resource *self, l_snmp_session *session, struct snmp_pdu *pdu, void *passdata)
{
  /* ALWAYS RETURN 0
   *  returning -1 will result in the walk being terminated
   */

  int num;

  if (pdu)
  {
    /* Got a PDU, interpret it.
     * The PDU will contain both the IP address in 
     * its OID name (last 4 digits) and the server name
     * in the value
     */

    v_rsvr *rsvr;
    char *ip_str;

    /* Create struct */

    rsvr = v_rsvr_create ();
    if (!rsvr)
    { i_printf (1, "v_rsvr_list_populate_serverwalk_callback failed to create rsvr struct"); return 0; }
    
    /* Get name string */

    rsvr->name_str = l_snmp_get_string_from_pdu (pdu);
    if (!rsvr->name_str)
    { i_printf (1, "v_rsvr_list_populate_serverwalk_callback failed to get name_str from pdu"); v_rsvr_free (rsvr); return 0; }

    /* Get IP string */
    
    asprintf (&ip_str, "%li.%li.%li.%li", 
      pdu->variables->name[pdu->variables->name_length-4],
      pdu->variables->name[pdu->variables->name_length-3],
      pdu->variables->name[pdu->variables->name_length-2],
      pdu->variables->name[pdu->variables->name_length-1]);

    num = inet_aton (ip_str, &rsvr->ip_addr);
    free (ip_str);
    if (num != 1)
    { i_printf (1, "v_rsvr_list_populate_serverwalk_callback failed to convert ip_str to ip_addr struct"); v_rsvr_free (rsvr); return 0; }

    /* Enqueue */

    num = i_list_enqueue (static_rsvr_list, rsvr);
    if (num != 0)
    { i_printf (1, "v_rsvr_list_populate_serverwalk_callback failed to enqueue rsvr"); v_rsvr_free (rsvr); return 0; }
  }
  else
  {
    /* End of the walk */
    static_walk_id = 0;

    if (session->error == SNMP_ERROR_NOERROR)
    {
      /* Walk successful, now walk for the ports */
      i_list_move_head (static_rsvr_list);
      static_walk_id = l_snmp_walk (self, static_snmp_session, "enterprises.1991.1.1.4.20.1.1.2", 0, v_rsvr_list_populate_portwalk_callback, NULL);  
      if (static_walk_id == -1)
      { i_printf (1, "v_rsvr_list_populate_serverwalk_callback failed to walk snL4RealServerPortCfgServerName OID tree"); v_rsvr_list_populate_failed (self); return -1; }
    }
    else
    {
      /* Walk failed */
      if (session->error == SNMP_ERROR_TIMEOUT)
      { i_printf (1, "v_rsvr_list_populate_serverwalk_callback SNMP timeout occurred whilst walking the snL4RealServrCfgName OID tree"); }
      else
      { i_printf (1, "v_rsvr_list_populate_serverwalk_callback failed to walk snL4RealServerCfgname OID tree (non-timeout error)"); }
      v_rsvr_list_populate_failed (self);
    }
  }

  return 0;
}

int v_rsvr_list_populate_portwalk_callback (i_resource *self, l_snmp_session *session, struct snmp_pdu *pdu, void *passdata)
{
  /* ALWAYS RETURN 0
   *  returning -1 will result in the walk being terminated
   */

  int num;

  if (pdu)
  {
    /* Got a PDU, interpret it.
     * The PDU will contain both the real server IP
     * and the port number in its OID name and the 
     * real server name will be in the pdu value
     */

    v_rsvr *rsvr = NULL;
    v_rsvr *temp_rsvr;
    v_rsvr_port *port;
    char *name_str;

    /* Check if the port is 65535
     *
     * At this point in time, I think port 65535 represents default
     * settings. At the moment that's not needed
     */

    if (pdu->variables->name[pdu->variables->name_length-1] == 65535)
    { return 0; }

    /* Get real server name */

    name_str = l_snmp_get_string_from_pdu (pdu);
    if (!name_str)
    { i_printf (1, "v_rsvr_list_populate_portwalk_callback failed to get server name from the pdu"); return 0; }

    /* Find the right rsvr. The current list->p
     * should be the righr rsvr if this is the first servers port
     * or a port for the same server as the previous one. Otherwise
     * it should always be the net server in the list
     */

    /* Check current list->p rsvr */
    temp_rsvr = i_list_restore (static_rsvr_list);
    if (temp_rsvr)
    {
      if (!strcmp(temp_rsvr->name_str, name_str))
      { rsvr = temp_rsvr; }     /* Found correct rsvr */
    }

    /* Check next rsvr in the list, if the 
     * right one has not yet been found
     */
    if (!rsvr)
    {
      i_list_move_next (static_rsvr_list);
      temp_rsvr = i_list_restore (static_rsvr_list);
      if (!strcmp(temp_rsvr->name_str, name_str))
      { rsvr = temp_rsvr; }     /* Found correct rsvr */
    }

    /* Last resort, check them all if there is still is no match */
    if (!temp_rsvr)
    {
      for (i_list_move_head(static_rsvr_list); (temp_rsvr=i_list_restore(static_rsvr_list))!=NULL; i_list_move_next(static_rsvr_list))
      { 
        if (!strcmp(temp_rsvr->name_str, name_str))
        { rsvr = temp_rsvr; break; }
      }
    }

    /* Dont need name_str now */
    free (name_str);

    /* Give up if there is still no rsvr */
    if (!rsvr)
    {
      i_printf (1, "v_rsvr_list_populate_portwalk_callback got port entry for server %s but found no match real server", name_str);
      return 0;
    }

    /* Create port and enqueue it */
    port = v_rsvr_port_create ();
    if (!port)
    { i_printf (1, "v_rsvr_list_populate_portwalk_callback failed to create port struct"); return 0; }
    port->port = pdu->variables->name[pdu->variables->name_length-1];
    port->rsvr = rsvr;

    num = i_list_enqueue (rsvr->port_list, port);
    if (num != 0)
    { i_printf (1, "v_rsvr_list_populate_portwalk_callback failed to enqueue port into rsvr %s", rsvr->name_str); v_rsvr_port_free (port); return 0; }
  }
  else
  {
    /* End of the walk */
    static_walk_id = 0;

    if (session->error == SNMP_ERROR_NOERROR)
    {
      /* Walk successful, populate finished */
      v_rsvr_list_populate_finished (self);
    }
    else
    {
      /* Walk failed */
      if (session->error == SNMP_ERROR_TIMEOUT)
      { i_printf (1, "v_rsvr_list_populate_portwalk_callback SNMP timeout occurred whilst walking the snL4RealServerPortCfgServerName OID tree"); }
      else
      { i_printf (1, "v_rsvr_list_populate_portwalk_callback failed to walk snL4RealServerPortCfgServerName OID tree (non-timeout error)"); }
      v_rsvr_list_populate_failed (self);
    }
  }

  return 0;
}

/* Terminate/Finished/Failed */

int v_rsvr_list_populate_terminate (i_resource *self)
{
  /* End the populate, failed or successful */
  if (static_walk_id > 0)
  { l_snmp_walk_terminate (static_walk_id); static_walk_id = 0; }
  if (static_snmp_session)
  { l_snmp_session_close (static_snmp_session); static_snmp_session = NULL; }

  static_populate_in_progress = 0;
  
  return 0;
}

int v_rsvr_list_populate_finished (i_resource *self)
{
  v_rsvr_list_populate_terminate (self);
  i_status_clear (self, "rsvr_list_populate"); 
  
  return 0;
}

int v_rsvr_list_populate_failed (i_resource *self)
{
  /* List populate failed */

  v_rsvr_list_populate_terminate (self);
  
  if (static_rsvr_list)
  { i_list_free (static_rsvr_list); static_rsvr_list = NULL; }

  i_status_set (self, STATUS_CRITICAL, "rsvr_list_populate", "Failed to populate the real server list"); 

  return 0;
}
