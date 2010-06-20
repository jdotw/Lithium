#include <stdlib.h>

#include <induction.h>
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
#include <induction/str.h>
#include <induction/hierarchy.h>
#include <induction/list.h>

#include <lithium/snmp.h>

#include "inputphase.h"

/* 
 * UPS Devices - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_inputphase_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_inputphase_item *input;

  /* Object Configuration */
  char *id_str = l_snmp_get_string_from_pdu (pdu);
  asprintf (&obj->desc_str, "Input Phase %s", id_str);
  //obj->mainform_func = v_inputphase_objform;
  //obj->histform_func = v_inputphase_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_inputphase_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create input item struct */
  input = v_inputphase_item_create ();
  if (!input)
  { i_printf (1, "v_inputphase_objfact_fab failed to create input item for object %s", obj->name_str); return -1; }
  input->obj = obj;
  obj->itemptr = input;

  /* 
   * Metric Creation 
   */

  input->voltage = l_snmp_metric_create (self, obj, "voltage", "Line Voltage", METRIC_GAUGE, ".1.3.6.1.4.1.705.1.6.2.1.2", "0", RECMETHOD_RRD, 0);
  input->voltage->unit_str = strdup ("V");
  input->voltage->div_by = 10;
  input->voltage->valstr_func = i_string_divby_metric;
  input->voltage->record_defaultflag = 1;

  input->min_voltage = l_snmp_metric_create (self, obj, "min_voltage", "Minimum Voltage", METRIC_GAUGE, ".1.3.6.1.4.1.705.1.6.2.1.4", "0", RECMETHOD_RRD, 0);
  input->min_voltage->unit_str = strdup ("V");
  input->min_voltage->div_by = 10;
  input->min_voltage->valstr_func = i_string_divby_metric;
  input->min_voltage->record_defaultflag = 1;

  input->max_voltage = l_snmp_metric_create (self, obj, "max_voltage", "Maximum Voltage", METRIC_GAUGE, ".1.3.6.1.4.1.705.1.6.2.1.5", "0", RECMETHOD_RRD, 0);
  input->max_voltage->unit_str = strdup ("V");
  input->max_voltage->div_by = 10;
  input->max_voltage->valstr_func = i_string_divby_metric;
  input->max_voltage->record_defaultflag = 1;

  input->frequency = l_snmp_metric_create (self, obj, "frequency", "Frequency", METRIC_GAUGE, ".1.3.6.1.4.1.705.1.6.2.1.3", "0", RECMETHOD_RRD, 0);
  input->frequency->div_by = 10;
  input->frequency->unit_str = strdup ("Hz");
  input->frequency->valstr_func = i_string_divby_metric;
  input->frequency->record_defaultflag = 1;

  /* Enqueue the input item */
  num = i_list_enqueue (cnt->item_list, input);
  if (num != 0)
  { i_printf (1, "v_inputphase_objfact_fab failed to enqueue input for object %s", obj->name_str); v_inputphase_item_free (input); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_inputphase_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_inputphase_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the input and remove
   * it from the item_list
   */

  int num;
  v_inputphase_item *input = obj->itemptr;

  if (!input) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, input);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
