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

/** \addtogroup incident Incident Reporting
 * @{
 */

/*
 * Incident Clearing
 */

int i_incident_clear (i_resource *self, i_entity *ent, i_incident *inc)
{
  /* Clears the specified incident.
   *
   * NOTE: A catch 22 exists here whereby the incident
   *       may not yet have an ID received from the customer.
   *       If the incident has no ID, it is assumed that one
   *       is on the way and when the ID arrived, the appropriate
   *       message callback will be called to handle the ID. 
   *       This function needs to capture the entity descriptor 
   *       data (as it describes the state of the entity hierarchy
   *       at the time of the incident clearing) and then either send
   *       the cleared message immediately, or store the entity
   *       descriptor data until the ID is received, at which point
   *       this function will be called again after the ID has been 
   *       set, causing the incident cleared message to be sent
   *       as normal. 
   */
  
  /* Update incident */
  if (inc->state == INCSTATE_ACTIVE)
  {
    inc->state = INCSTATE_INACTIVE;
    gettimeofday (&inc->end_tv, NULL);
    if (ent)
    {
      if (inc->ent) i_entity_descriptor_free (inc->ent);
      inc->ent = i_entity_descriptor_duplicate(i_entity_descriptor_struct_static (ent));
    }
    if (ent && ent->ent_type == ENT_METRIC)
    {
      i_metric *met = (i_metric *) ent;
      inc->cleared_valstr = i_metric_valstr (met, NULL);
      if (inc->met) i_entity_descriptor_free (inc->met);
      inc->met = i_entity_descriptor_duplicate(i_entity_descriptor_struct_static (ENTITY(met)));
    }
  }

  /* Handle incident clear reporting 
   *
   * NOTE: If the ID is -1, no ID has been assigned. Providing a 
   *       msgproc callback (for the ID message) is present, the
   *       inc->ent entity descriptor is converted to data and 
   *       stored as inc->ent_data. Once the ID is received, the
   *       message callback will set the ID and call this func again
   */
  if (inc->id == -1)
  {
    /* Check that a msgcb is present */    
    if (!inc->msgcb)
    {
      /* Dead incident, no ID, no msgcb */
      i_printf (1, "i_incident_clear encountered dead incident with no ID and no msgcb, freeing");
      i_incident_free (inc);
      return -1;
    }
    
    /* DO NOT FREE INCIDENT, it is the passdata for the msgcb */
  }
  else
  {
    /* Incident has a valid ID, the incident is converted
     * to data and sent to the customer resource and the
     * incident struct is then freed.
     */
    long msgid;
    char *inc_data;
    int inc_datasize;

    /* Convert incident to data */
    inc_data = i_incident_data (inc, &inc_datasize);
    if (!inc_data)
    {
      i_printf (1, "i_incident_clear failed to convert incident to data"); 
      i_incident_free (inc);
      return -1;
    }

    /* Send incident clear report */
    msgid = i_message_send (self, MSG_INC_CLEAR, inc_data, inc_datasize, self->hierarchy->cust_addr, MSG_FLAG_REQ, 0);
    free (inc_data);
    if (msgid == -1)
    { i_printf (1, "i_incident_clear warning, failed to send MSG_INC_CLEAR for incident %li", inc->id); }

    /* Free the incident */
    i_incident_free (inc);
  }

  return 0;
}

