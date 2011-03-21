#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/ip.h>
#include <induction/timeutil.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/interface.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/form.h>
#include <induction/str.h>

#include "snmp.h"
#include "snmp_storage.h"

int l_snmp_storage_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  l_snmp_storage_item *store = obj->itemptr;

  /* 
   * Storage Resource
   */

  asprintf (&str, "Storage Resource '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);
  str = i_metric_valstr (store->size, NULL);
  i_form_string_add (reqdata->form_out, "size", "Size", str);
  free (str);
  str = i_metric_valstr (store->used, NULL);
  i_form_string_add (reqdata->form_out, "used", "Used", str);
  free (str);
  str = i_metric_valstr (store->free, NULL);
  i_form_string_add (reqdata->form_out, "free", "Free", str);
  free (str);
  str = i_metric_valstr (store->used_pc, NULL);
  i_form_string_add (reqdata->form_out, "used_pc", "Used (%)", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int l_snmp_storage_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  l_snmp_storage_item *store = obj->itemptr;

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metgraph_add (self, reqdata->form_out, store->used_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY); 
  /* Week */
  i_form_metgraph_add (self, reqdata->form_out, store->used_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metgraph_add (self, reqdata->form_out, store->used_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metgraph_add (self, reqdata->form_out, store->used_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);
  
  return 1;
}
