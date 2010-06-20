#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/entity.h>
#include <induction/cement.h>
#include <induction/device.h>
#include <induction/object.h>
#include <induction/metric.h>

#include "snmp.h"

/*
 * Metric Manipulation Functions
 */

i_metric* l_snmp_metric_create (i_resource *self, i_object *obj, char *name_str, char *desc_str, int type, char *base_oid_str, char *index_oid_str, unsigned short record_method, unsigned short flags)
{
  /* l_snmp_metric_create creates and registers a metric
   * based on the specified oid_str and metric type.
   *
   * The newly created/registered metric is then returned.
   */

  int num;
  i_metric *met;
  l_snmp_metric_refresh_data *refdata;
  static i_entity_refresh_config refconfig;

  /* Create/configure the metric */
  met = i_metric_create (name_str, desc_str, type);
  if (!met)
  { 
    i_printf (1, "l_snmp_metric_create failed to create metric for OID %s.%s in object %s", base_oid_str, index_oid_str, obj->name_str);
    return NULL;
  }
  met->refresh_func = l_snmp_metric_refresh;
  met->record_method = record_method;

  /* Create/Configure the refresh data */
  refdata = l_snmp_metric_refresh_data_create ();
  if (!refdata)
  {
    i_printf (1, "l_snmp_metric_create failed to create l_snmp_metric_refresh_data struct for metric %s in object %s", 
      met->name_str, obj->name_str);
    i_entity_free (ENTITY(met));
    return NULL;
  }
  met->refresh_data = refdata;

  /* Create the oid_str */
  if (index_oid_str)
  { asprintf (&refdata->oid_str, "%s.%s", base_oid_str, index_oid_str); }
  else
  { refdata->oid_str = strdup (base_oid_str); }
  
  /* Parse the refdata->oid_str */
  refdata->name_len = MAX_OID_LEN;
  num = l_snmp_parse_oidstr (refdata->oid_str, refdata->name, &refdata->name_len);
  if (num != 0)
  { 
    i_printf (1, "l_snmp_metric_create failed to parse OID %s for object %s metric %s", refdata->oid_str, obj->name_str, name_str);
    i_entity_free (ENTITY(met));
    return NULL;
  }

  /* Register metric */
  num = i_entity_register (self, ENTITY(obj), ENTITY(met));
  if (num != 0)
  { 
    i_printf (1, "l_snmp_metric_create failed to register metric %s to object %s", met->name_str, obj->name_str);
    i_entity_free (ENTITY(met));
    return NULL;
  }

  /* Load/apply refresh config.
   * If the SMET_PARENTREFMETHOD flag is present,
   * a loading of a refresh configuration is skipped
   * and the default is applied. Otherwise, the 
   * configuration is loaded from SQL, or a default
   * of REFMETHOD_PARENT is applied
   */

  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
//  if (flags & SMET_PARENTREFMETHOD)                         /* FIX HACK Castnet */
//  {
    /* Apply the default REFMETHOD_PARENT
     * based refresh config
     */
    num = i_entity_refresh_config_apply (self, ENTITY(met), &refconfig);
//  }
//  else
//  {
    /* Attempt to load the refresh config, using
     * the defaults if necessary
     */
//    num = i_entity_refresh_config_loadapply (self, ENTITY(met), &refconfig);
//    if (num != 0)
//    { 
//      i_printf (1, "l_snmp_metric_create failed to apply refresh configuration for metric %s", met->name_str); 
//      i_entity_deregister (self, ENTITY(met)); 
//      i_entity_free (ENTITY(met));
//      return NULL;
//    }
//  }
    

  return met;
}

