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
#include "metric.h"
#include "hierarchy.h"
#include "incident.h"

/*
 * Report to the customer that we have reached 'normal' status
 *
 * This is only sent once
 */

int i_devnormal_report (i_resource *self)
{
  /* Creates an incident report based on the type
   * of incident specified (admin state change, 
   * op state change, etc) for the given entity
   */

  int datasize;
  char *data;

  data = i_entity_descriptor_data (NULL, ENTITY(self->hierarchy->dev), &datasize);
  if (!data)
  { i_printf (1, "i_devnormal_report failed to create entity descriptor data"); return -1; }

  /* Send incident report */
  i_message_send (self, MSG_DEVNORMAL_REPORT, data, datasize, self->hierarchy->cust_addr, MSG_FLAG_REQ, 0);
  free (data);

  return 0;
}
