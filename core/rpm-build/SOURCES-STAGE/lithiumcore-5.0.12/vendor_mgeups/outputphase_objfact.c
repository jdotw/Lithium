#include <stdlib.h>

#include "induction.h"
#include "induction/timer.h"
#include "induction/timeutil.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/str.h"
#include "induction/hierarchy.h"
#include "induction/list.h"

#include "device/snmp.h"

#include "outputphase.h"

/* 
 * UPS Devices - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_outputphase_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_outputphase_item *output;

  /* Object Configuration */
  char *id_str = l_snmp_get_string_from_pdu (pdu);
  asprintf (&obj->desc_str, "Output Phase %s", id_str);
  //obj->mainform_func = v_outputphase_objform;
  //obj->histform_func = v_outputphase_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_outputphase_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create output item struct */
  output = v_outputphase_item_create ();
  if (!output)
  { i_printf (1, "v_outputphase_objfact_fab failed to create output item for object %s", obj->name_str); return -1; }
  output->obj = obj;
  obj->itemptr = output;

  /* 
   * Metric Creation 
   */

  output->voltage = l_snmp_metric_create (self, obj, "voltage", "Line Voltage", METRIC_GAUGE, ".1.3.6.1.4.1.705.1.7.2.1.2", "0", RECMETHOD_RRD, 0);
  output->voltage->unit_str = strdup ("V");
  output->voltage->div_by = 10;
  output->voltage->valstr_func = i_string_divby_metric;
  output->voltage->record_defaultflag = 1;

  output->frequency = l_snmp_metric_create (self, obj, "frequency", "Frequency", METRIC_GAUGE, ".1.3.6.1.4.1.705.1.7.2.1.3", "0", RECMETHOD_RRD, 0);
  output->frequency->div_by = 10;
  output->frequency->unit_str = strdup ("Hz");
  output->frequency->valstr_func = i_string_divby_metric;
  output->frequency->record_defaultflag = 1;

  output->load = l_snmp_metric_create (self, obj, "load", "Load", METRIC_GAUGE, ".1.3.6.1.4.1.705.1.7.2.1.4", "0", RECMETHOD_RRD, 0);
  output->load->unit_str = strdup ("%");
  output->load->record_defaultflag = 1;

  output->current = l_snmp_metric_create (self, obj, "current", "Current", METRIC_GAUGE, ".1.3.6.1.4.1.705.1.7.2.1.5", "0", RECMETHOD_RRD, 0);
  output->current->div_by = 10;
  output->current->unit_str = strdup ("Amp");
  output->current->valstr_func = i_string_divby_metric;
  output->current->record_defaultflag = 1;

  /* Enqueue the output item */
  num = i_list_enqueue (cnt->item_list, output);
  if (num != 0)
  { i_printf (1, "v_outputphase_objfact_fab failed to enqueue output for object %s", obj->name_str); v_outputphase_item_free (output); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_outputphase_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
{
  /* Check the result */
  if (result == SNMP_ERROR_NOERROR)
  {
    /* No errors, set item list state to NORMAL */
    cnt->item_list_state = ITEMLIST_STATE_NORMAL;
  }

  return 0;
}

/* Object Factory Clean Func
 *
 * Called when an object is obsolete prior to it being deregistered and free
 */

int v_outputphase_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the output and remove
   * it from the item_list
   */

  int num;
  v_outputphase_item *output = obj->itemptr;

  if (!output) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, output);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
