#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "data.h"
#include "form.h"
#include "socket.h"
#include "message.h"
#include "msgproc.h"
#include "cement.h"
#include "entity.h"
#include "hierarchy.h"
#include "incident.h"

/** \addtogroup incident Incident Reporting
 * @ingroup backend
 * @{
 */

/*
 * Incident Clearing
 */

/* Clear all incidents. Used by the core process
 * when terminating a resource
 */

int i_incident_clearall (i_resource *self, i_resource_address *custresaddr, i_resource_address *devresaddr)
{
  /* Clears all incidents on the customer at custresaddr 
   * belonging to the device at devresaddr
   */
  long msgid;
  char *addrstr;

  /* Convert resaddr to string */
  addrstr = i_resource_address_struct_to_string (devresaddr);
  if (!addrstr)
  { i_printf (1, "i_incident_clearall failed to convert devresaddr to string"); return -1; }
  
  /* Send incident clear request */
  msgid = i_message_send (self, MSG_INC_CLEARALL, addrstr, strlen(addrstr)+1, custresaddr, MSG_FLAG_REQ, 0);
  free (addrstr);
  if (msgid == -1)
  { i_printf (1, "i_incident_clearall warning, failed to send MSG_INC_CLEARALL"); return -1; }

  return 0;
}


/* @} */
