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
#include "trigger.h"
#include "value.h"
#include "hierarchy.h"
#include "incident.h"

/** \addtogroup incident Incident Reporting
 * @ingroup backend
 * @{
 */

/*
 * Incident Reporting
 */

i_incident* i_incident_report (i_resource *self, unsigned short type, i_entity *ent)
{
  /* Creates an incident report based on the type
   * of incident specified (admin state change, 
   * op state change, etc) for the given entity
   */

  int datasize;
  char *data;
  i_incident *inc;

  /* Create the incident struct */
  inc = i_incident_create ();
  inc->ent = i_entity_descriptor_duplicate(i_entity_descriptor_struct_static (ent));
  inc->type = type;
  inc->state = INCSTATE_ACTIVE;
  if (ent->ent_type == ENT_TRIGGER && ent->parent)
  {
    i_metric *met = (i_metric *) ent->parent;
    i_trigger *trg = (i_trigger *) ent;
    inc->met = i_entity_descriptor_duplicate(i_entity_descriptor_struct_static (ENTITY(met)));
    inc->raised_valstr = i_metric_valstr (met, NULL);
    inc->trg_type = trg->trg_type;
    inc->trg_xval_str = i_value_valstr (trg->trg_type, trg->val, met->unit_str, met->enumstr_list);
    if (trg->yval)
    { inc->trg_yval_str = i_value_valstr (trg->trg_type, trg->yval, met->unit_str, met->enumstr_list); }
    inc->local_trg = trg;
  }
  gettimeofday (&inc->start_tv, NULL);

  /* Create incident data */
  data = i_incident_data (inc, &datasize);
  if (!data)
  { 
    i_printf (1, "i_incident_report failed to create incident data for %s incident on %s %s", 
      i_incident_typestr (type), i_entity_typestr (ent->ent_type), ent->name_str); 
    i_incident_free (inc);
    return NULL;
  }

  /* Send incident report */
  inc->msgid = i_message_send (self, MSG_INC_REPORT, data, datasize, self->hierarchy->cust_addr, MSG_FLAG_REQ, 0);
  free (data);
  if (inc->msgid == -1)
  {
    i_printf (1, "i_incident_report failed to send MSG_INC_REPORT message to customer resource for %s incident on %s %s",
      i_incident_typestr (type), i_entity_typestr (ent->ent_type), ent->name_str);
    i_incident_free (inc);
    return NULL;
  }

  /* Add msgproc callback */
  inc->msgcb = i_msgproc_callback_add (self, self->core_socket, inc->msgid, 600, 0, i_incident_report_msgcb, inc);
  if (!inc->msgcb)
  {
    i_printf (1, "i_incident_report failed to add msgproc callack for MSG_INC_REPORT message for %s incident on %s %s",
      i_incident_typestr (type), i_entity_typestr (ent->ent_type), ent->name_str);
    i_incident_free (inc);
    return NULL;
  }

  return inc;
}

int i_incident_report_msgcb (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* Called when a response to an incident report 
   * message is received from the customer resource.
   *
   * Non-persistent callback, always return -1
   */
  i_incident *inc = passdata;

  /* NULLify msgcb */
  inc->msgcb = NULL;

  /* Check message */
  if (msg && msg->data && msg->datasize == sizeof(long))
  {
    /* 
     * Valid Response Received 
     *
     * If passdata is not NULL, it is a pointer to an incident
     * that is still valid and hence the received incident ID
     * is copied to the struct. However, it is possible that the
     * incident was cleared before the incident ID was received. In 
     * that incident, the passdata will be NULL and the incident should
     * be immediately cleared.
     */
    i_incident *inc = passdata;

    if (inc)
    {
      /* Copy the value from the message */
      memcpy (&inc->id, msg->data, sizeof(long));
    
      if (inc->state == INCSTATE_INACTIVE)
      {
        /* Incident no longer valid, clear it immediately now that the ID is known */
        i_incident_clear (self, NULL, inc);
      }
    }
  }
  else
  {
    /* Invalid Response Received 
     *
     * NOTE: The incident struct should NOT be freed as it would now exist as
     *       as pointer in the trigger's entity struct. The incident should be
     *       left as is, with the incident ID still set to -1. The lack of a
     *       msgcb struct in the incident will denote no outstanding report
     *       operating is in progress 
     */
    i_printf (1, "i_incident_report_msgcb received an invalid response to a MSG_INC_REPORT message");
  }

  return -1;
}

/* @} */
