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

/** \addtogroup incident Incident Transition
 * @ingroup backend
 * @{
 */

/*
 * Incident Transition (Change in severity)
 */

int i_incident_transition (i_resource *self, i_incident *inc, i_entity *ent)
{
  /* Transitions the given incident to the 
   * condition reported by the entity specified
   */

  int datasize;
  char *data;
  
  /* Update the incident struct */
  if (inc->ent) i_entity_descriptor_free (inc->ent);
  inc->ent = i_entity_descriptor_duplicate(i_entity_descriptor_struct_static (ent));
  if (ent->ent_type == ENT_TRIGGER && ent->parent)
  {
    /* Update active trigger info */
    i_metric *met = (i_metric *) ent->parent;
    inc->met = i_entity_descriptor_duplicate(i_entity_descriptor_struct_static (ENTITY(met)));
    i_trigger *trg = (i_trigger *) ent;
    inc->trg_type = trg->trg_type;
    if (inc->trg_xval_str) 
    { free (inc->trg_xval_str); inc->trg_xval_str = NULL; }
    inc->trg_xval_str = i_value_valstr (trg->trg_type, trg->val, met->unit_str, met->enumstr_list);
    if (inc->trg_yval_str) 
    { free (inc->trg_yval_str); inc->trg_yval_str = NULL; }
    if (trg->yval) inc->trg_yval_str = i_value_valstr (trg->trg_type, trg->yval, met->unit_str, met->enumstr_list);
    inc->local_trg = trg;
  }
  gettimeofday (&inc->last_transition_tv, NULL);

  /* Create incident data */
  data = i_incident_data (inc, &datasize);
  if (!data)
  { 
    i_printf (1, "i_incident_transition failed to create incident data for incident %li on %s %s", 
      inc->id, i_entity_typestr (ent->ent_type), ent->name_str); 
    return -1;
  }

  /* Send incident transition report */
  int msgid = i_message_send (self, MSG_INC_TRANSITION, data, datasize, self->hierarchy->cust_addr, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid == -1)
  {
    i_printf (1, "i_incident_transition failed to send MSG_INC_TRANSITION message to customer resource for incident %li on %s %s",
      inc->id, i_entity_typestr (ent->ent_type), ent->name_str);
    return -1;
  }

  return 0;
}

/* @} */
