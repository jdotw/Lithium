#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/data.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/opstate.h>
#include <induction/incident.h>

#include "case.h"
#include "action.h"
#include "devnormal.h"

/* Device Normal Report Handler */

int l_devnormal_handler_report (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* Handle an inbound incident report
   *
   * ALWAYS RETURN 0 to keep handler active
   */
  i_entity_descriptor *entdesc;

  /* Convert msg data to incident */
  entdesc = i_entity_descriptor_struct (msg->data, msg->datasize);
  if (!entdesc) 
  { 
    i_printf (1, "l_devnormal_handler_report failed to convert msg->data to entdesc");
    return 0;
  }

  /* Get entity */
  i_device *dev = (i_device *) i_entity_local_get (self, (i_entity_address *)entdesc);
  i_entity_descriptor_free (entdesc);
  if (!dev)
  { 
    i_printf (1, "l_devnormal_handler_report failed to get local entity"); 
    return 0;
  }

  /* Check/Set State */
  if (dev->opstate < ENTSTATE_NORMAL)
  { i_opstate_change (self, ENTITY(dev), ENTSTATE_NORMAL); }

  return 0;
}

