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
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/form.h>
#include <induction/str.h>

#include "power.h"

int v_power_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_power_item *item = obj->itemptr;

  /* Start Frame */
  i_form_frame_start (reqdata->form_out, "power", "Xserve Power");  

  /* 
   * Info Strings 
   */

  str = i_metric_valstr (item->sc_vcore, NULL);
  i_form_string_add (reqdata->form_out, "sc_vcore", "Sys.Ctrl Vcore", str);
  if (str) free (str);

  str = i_metric_valstr (item->ddr_io, NULL);
  i_form_string_add (reqdata->form_out, "ddr_io", "DDR IO", str);
  if (str) free (str);

  str = i_metric_valstr (item->ddr_io_sleep, NULL);
  i_form_string_add (reqdata->form_out, "ddr_io_sleep", "DDR IO Sleep", str);
  if (str) free (str);

  str = i_metric_valstr (item->io_vdd, NULL);
  i_form_string_add (reqdata->form_out, "io_vdd", "IO VDD", str);
  if (str) free (str);

  str = i_metric_valstr (item->v_1_2v, NULL);
  i_form_string_add (reqdata->form_out, "v_1_2v", "1.2v", str);
  if (str) free (str);

  str = i_metric_valstr (item->v_1_2v_sleep, NULL);
  i_form_string_add (reqdata->form_out, "v_1_2v_sleep", "1.2v Sleep", str);
  if (str) free (str);

  str = i_metric_valstr (item->v_1_5v, NULL);
  i_form_string_add (reqdata->form_out, "v_1_5v", "1.5v", str);
  if (str) free (str);

  str = i_metric_valstr (item->v_1_5v_sleep, NULL);
  i_form_string_add (reqdata->form_out, "v_1_5v_sleep", "1.5v Sleep", str);
  if (str) free (str);

  str = i_metric_valstr (item->v_1_8v, NULL);
  i_form_string_add (reqdata->form_out, "v_1_8v", "1.8v", str);
  if (str) free (str);

  str = i_metric_valstr (item->v_3_3v, NULL);
  i_form_string_add (reqdata->form_out, "v_3_3v", "3.3v", str);
  if (str) free (str);

  str = i_metric_valstr (item->v_3_3v_sleep, NULL);
  i_form_string_add (reqdata->form_out, "v_3_3v_sleep", "3.3v Sleep", str);
  if (str) free (str);

  str = i_metric_valstr (item->v_3_3v_trickle, NULL);
  i_form_string_add (reqdata->form_out, "v_3_3v_trickle", "3.3v Trickle", str);
  if (str) free (str);

  str = i_metric_valstr (item->v_5v, NULL);
  i_form_string_add (reqdata->form_out, "v_5v", "5v", str);
  if (str) free (str);

  str = i_metric_valstr (item->v_5v_sleep, NULL);
  i_form_string_add (reqdata->form_out, "v_5v_sleep", "5v Sleep", str);
  if (str) free (str);

  str = i_metric_valstr (item->v_12v, NULL);
  i_form_string_add (reqdata->form_out, "v_12v", "12v", str);
  if (str) free (str);

  str = i_metric_valstr (item->v_12v_trickle, NULL);
  i_form_string_add (reqdata->form_out, "v_12v_trickle", "12v Trickle", str);
  if (str) free (str);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, "power");

  return 1;
}

