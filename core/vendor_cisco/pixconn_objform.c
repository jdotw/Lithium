#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/timeutil.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/str.h"
#include "induction/form.h"

#include "pixconn.h"

int v_pixconn_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_pixconn_item *pixconn = obj->itemptr;

  /* 
   * PIX Connections
   */

  asprintf (&str, "Service '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Service", obj->desc_str);

  str = i_metric_valstr (pixconn->curconn, NULL);
  i_form_string_add (reqdata->form_out, "curconn", "Current Connections", str);
  free (str);

  str = i_metric_valstr (pixconn->highconn, NULL);
  i_form_string_add (reqdata->form_out, "highconn", "Highest Connection Count", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_pixconn_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_pixconn_item *pixconn = obj->itemptr;

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metgraph_add (self, reqdata->form_out, pixconn->curconn, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
  /* Week */
  i_form_metgraph_add (self, reqdata->form_out, pixconn->curconn, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metgraph_add (self, reqdata->form_out, pixconn->curconn, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metgraph_add (self, reqdata->form_out, pixconn->curconn, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);


  return 1;
}



