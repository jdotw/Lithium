#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "hashtable.h"
#include "message.h"
#include "timer.h"
#include "cement.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "navtree.h"
#include "opstate.h"
#include "value.h"
#include "incident.h"
#include "trigger.h"

/** \addtogroup trigger Triggers
 * @ingroup entity
 * @{
 */

/* 
 * CEMent Trigggers
 */

/* Struct manipulation */

i_trigger* i_trigger_create (char *name_str, char *desc_str, unsigned short val_type, unsigned short trg_type, unsigned short effect)
{
  i_trigger *trg;

  trg = (i_trigger *) i_entity_create (name_str, desc_str, ENT_TRIGGER, sizeof(i_trigger));
  if (!trg)
  { i_printf (1, "i_trigger_create failed to create trigger entity"); return NULL; }
  trg->val_type = val_type;
  trg->trg_type = trg_type;
  trg->effect = effect;
  trg->navtree_expand = NAVTREE_EXP_NEVER;
  trg->authorative = 1;

  return trg;
}

void i_trigger_free (void *trgptr)
{
  /* Just free the trigger-specific 
   * portions of the struct. The struct
   * and everything else will be freed
   * by i_entity_free.
   */
  i_trigger *trg = trgptr;

  if (!trg) return;
  
  if (trg->val) i_value_free (trg->val);

  /* DO NOT FREE STRUCT */
}

i_trigger* i_trigger_duplicate (i_trigger *trg)
{
  /* Duplicate a trigger */
  i_trigger *dup;

  dup = i_trigger_create (trg->name_str, trg->desc_str, trg->val_type, trg->trg_type, trg->effect);
  if (!dup)
  { i_printf (1, "i_trigger_duplicate failed to create duplicate i_trigger struct"); return NULL; }
  dup->duration_sec = trg->duration_sec;

  return dup;
}

/* 
 * Trigger Specific Registration/Deregistration 
 * (Should only be called by i_entity_register and i_entity_register)
 */

int i_trigger_register (i_resource *self, i_metric *met, i_trigger *trg)
{
  return 0;
}

int i_trigger_deregister (i_resource *self, i_trigger *trg)
{
  if (trg->opstate != ENTSTATE_NORMAL)
  {    
    /* Change opstate of trigger to normal value */
    i_opstate_change (self, ENTITY(trg), ENTSTATE_NORMAL);
  }

  return 0;
}

/* Type String */

char* i_trigger_typestr (unsigned short trg_type)
{
  switch (trg_type)
  {
    case TRGTYPE_LT: return "Less than";
    case TRGTYPE_GT: return "Greater than";
    case TRGTYPE_EQUAL: return "Equal to";
    case TRGTYPE_NOTEQUAL: return "Not equal to";
    case TRGTYPE_RANGE: return "Range";
    default: return "Unknown";
  }

  return NULL;
}

/* @} */
