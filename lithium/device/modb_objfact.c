#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/name.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/interface.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/device.h>
#include <induction/metric.h>
#include <induction/navtree.h>
#include <induction/str.h>

#include "navtree.h"
#include "snmp.h"
#include "objform.h"
#include "modb.h"

/* 
 * Module Builder - Object Factory Functions 
 */

/* Object Factory Fabrication */

int l_modb_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  l_snmp_objfact *fact = (l_snmp_objfact *) cnt->objfactptr;

  /* Object Configuration */
  if (fact->enable_dupname_handling)
  { 
    char *pdu_desc = l_snmp_get_string_from_pdu (pdu);
    asprintf (&obj->desc_str, "%s %s", pdu_desc, index_oidstr); 
    free (pdu_desc);
  }
  else
  { obj->desc_str = l_snmp_get_string_from_pdu (pdu); }
  obj->mainform_func = l_objform_generic;

  /* Load/Apply Refresh config */
  i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);

  /* 
   * Metric Creation 
   */

  l_modb_metric *met_template;

  /* Create basic SNMP metrics first */
  for (i_list_move_head(cnt->met_templates); (met_template=i_list_restore(cnt->met_templates))!=NULL; i_list_move_next(cnt->met_templates))
  {
    if (met_template->type == 0)
    { l_modb_objfact_fab_met (self, cnt, obj, met_template, index_oidstr); }
  }

  /* Then Create auto-calculated/dynamic metrics */
  for (i_list_move_head(cnt->met_templates); (met_template=i_list_restore(cnt->met_templates))!=NULL; i_list_move_next(cnt->met_templates))
  {
    if (met_template->type != 0)
    { l_modb_objfact_fab_met (self, cnt, obj, met_template, index_oidstr); }
  }



  return 0;
}

i_metric* l_modb_objfact_fab_met (i_resource *self, i_container *cnt, i_object *obj, l_modb_metric *met_template, char *index_oidstr)
{
  /* Fabricate a metric */
  i_metric *met = NULL;
  if (met_template->type == 0)
  {
    /* SNMP Metric */
    met = l_snmp_metric_create (self, obj, met_template->name_str, met_template->desc_str,
      met_template->val_type, met_template->oid_str, index_oidstr, met_template->record_method, 0);
    if (met_template->unit_str) met->unit_str = strdup (met_template->unit_str);

    /* Add Enums */
    i_metric_enumstr *met_enum;
    for (i_list_move_head(met_template->enum_list); (met_enum=i_list_restore(met_template->enum_list))!=NULL; i_list_move_next(met_template->enum_list))
    {
      i_metric_enumstr_add (met, met_enum->val_int, met_enum->str);
    }

    /* Type-specific setup */
    switch (met_template->val_type)
    {
      /* For numeric values, use the i_string_volume_metric formatter by 
       * default to present strings scaled to k,M,T, etc. 
       * The i_string_volume_metric formatter will use k=1024 if the
       * string 'byte' is present in the units
       */
      case METRIC_INTEGER:
      case METRIC_GAUGE:
      case METRIC_COUNT64:
      case METRIC_COUNT:
        if (!met->enumstr_list || met->enumstr_list->size < 1) met->valstr_func = i_string_volume_metric;
    }

  }
  else if (met_template->type == 1)
  {
    /* Rate Metric */
    if (!met_template->countermet_desc)
    { i_printf (1, "l_modb_objfact_fab_met error, no countermet_desc for metric %s", met_template->desc_str); return NULL; }
    char *countmet_name = strdup (met_template->countermet_desc);
    i_name_parse (countmet_name);
    i_metric *countmet = (i_metric *) i_entity_child_get (ENTITY(obj), countmet_name);
    char *unit_str;
    if (met_template->unit_str) unit_str = met_template->unit_str;
    else unit_str = "";
    met = i_metric_acrate_create (self, obj, met_template->name_str, met_template->desc_str, unit_str, met_template->record_method, countmet, 0);
    free (countmet_name);
  }
  else if (met_template->type == 2)
  {
    /* Percent Metric */
    if (!met_template->gaugemet_desc)
    { i_printf (1, "l_modb_objfact_fab_met error, no gaugemet_desc for metric %s", met_template->desc_str); return NULL; }
    char *gaugemet_name = strdup (met_template->gaugemet_desc);
    i_name_parse (gaugemet_name);
    i_metric *gaugemet = (i_metric *) i_entity_child_get (ENTITY(obj), gaugemet_name);
    i_metric *maxmet = NULL;
    if (met_template->maxmet_desc)
    {
      char *maxmet_name = strdup (met_template->maxmet_desc);
      i_name_parse (maxmet_name);
      maxmet = (i_metric *) i_entity_child_get (ENTITY(obj), maxmet_name);
      free (maxmet_name);
    }
    met = i_metric_acpcent_create (self, obj, met_template->name_str, met_template->desc_str, met_template->record_method, gaugemet, maxmet, ACPCENT_REFCB_GAUGE);
    free (gaugemet_name);
  }

  /* Set Recording Default */
  if (met && met->record_method != 0)
  { met->record_defaultflag = 1; }

  /* Set other properties */
  if (met && met_template) met->summary_flag = met_template->summary_flag;
  if (met && met_template) met->multiply_by = met_template->multiply_by;
  if (met && met_template) met->kbase = met_template->kbase;

  return met;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int l_modb_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int l_modb_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{

  return 0;
}
