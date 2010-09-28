#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/str.h>
#include <induction/hierarchy.h>

#include <lithium/snmp.h>

#include "supplies.h"

int v_supplies_level_refcb (i_resource *self, i_metric *met, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the callback alive */
  v_supplies_item *item = passdata;

  /* Refresh remaining_pc */
  float curval = i_metric_valflt (item->current_level, NULL);
  if (curval >= 0)
  {
    if (item->remaining_pc) i_entity_refresh (self, ENTITY(item->remaining_pc), REFFLAG_AUTO, NULL, NULL);
    if (item->fill_pc) i_entity_refresh (self, ENTITY(item->fill_pc), REFFLAG_AUTO, NULL, NULL);
  }

  return 0;
}

int v_supplies_class_refcb (i_resource *self, i_metric *met, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the callback alive */
  v_supplies_item *item = passdata;
  i_triggerset *tset;

  /* Check if triggers have been applied */
  if (item->tset_applied == 0)
  {
    i_metric_value *val = i_metric_curval(item->class);
    if (val && val->integer > 0)
    {
      if (val->integer == 3)
      {
        /* Consumed */
        item->remaining_pc = i_metric_acpcent_create (self, met->obj, "remaining_pc", "Supply Remaining", RECMETHOD_RRD, item->current_level, item->max_capacity, 0);
        item->remaining_pc->record_defaultflag = 1;
        item->remaining_pc->unit_str = strdup ("%");

        tset = i_triggerset_create ("remaining_pc", "Supply Remaining", "remaining_pc");
        i_triggerset_addtrg (self, tset, "low", "Low", VALTYPE_FLOAT, TRGTYPE_RANGE, 1, NULL, 15, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
        i_triggerset_addtrg (self, tset, "empty", "Empty", VALTYPE_FLOAT, TRGTYPE_EQUAL, 0, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
        i_triggerset_assign (self, met->obj->cnt, tset);
      }
      else if (val->integer == 4)
      {
        /* Receptacle */
        item->fill_pc = i_metric_acpcent_create (self, met->obj, "fill_pc", "Fill Level", RECMETHOD_RRD, item->current_level, item->max_capacity, 0);
        item->fill_pc->record_defaultflag = 1;
        item->fill_pc->unit_str = strdup ("%");

        tset = i_triggerset_create ("fill_pc", "Fill Level", "fill_pc");
        i_triggerset_addtrg (self, tset, "high", "High", VALTYPE_FLOAT, TRGTYPE_RANGE, 80, NULL, 99, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
        i_triggerset_addtrg (self, tset, "full", "Full", VALTYPE_FLOAT, TRGTYPE_GT, 99, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
        i_triggerset_assign (self, met->obj->cnt, tset);
      }
    }
    item->tset_applied = 1;
  }
  return 0;
}
