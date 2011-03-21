#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/ip.h"
#include "induction/timeutil.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/form.h"
#include "induction/str.h"

#include "atcpu.h"

int v_atcpu_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_atcpu_item *cpu = obj->itemptr;
  i_form_item *item;

  /* Check Item */
  if (obj != v_atcpu_obj())
  { i_form_string_add (reqdata->form_out, "error", "Error", "Invalid item."); return 1; }
  if (!cpu)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No item present."); return 1; }

  /* Start Frame */
  i_form_frame_start (reqdata->form_out, obj->cnt->name_str, obj->cnt->desc_str);  

  /* 
   * Info Strings 
   */

  str = i_metric_valstr (cpu->onesec_avg, NULL);
  item = i_form_string_add (reqdata->form_out, "onesec_avg", "1 Second Average", str);
  if (str) free (str);
  i_form_item_add_link (item, 0, 0, 0, NULL, ENT_ADDR(cpu->onesec_avg), "main", 0, NULL, 0);  
  
  str = i_metric_valstr (cpu->tensec_avg, NULL);
  item = i_form_string_add (reqdata->form_out, "tensec_avg", "10 Second Average", str);
  if (str) free (str);
  i_form_item_add_link (item, 0, 0, 0, NULL, ENT_ADDR(cpu->tensec_avg), "main", 0, NULL, 0);  

  str = i_metric_valstr (cpu->onemin_avg, NULL);
  item = i_form_string_add (reqdata->form_out, "onemin_avg", "1 Minute Average", str);
  if (str) free (str);
  i_form_item_add_link (item, 0, 0, 0, NULL, ENT_ADDR(cpu->onemin_avg), "main", 0, NULL, 0);  

  str = i_metric_valstr (cpu->fivemin_avg, NULL);
  item = i_form_string_add (reqdata->form_out, "fivemin_avg", "5 Minute Average", str);
  if (str) free (str);
  i_form_item_add_link (item, 0, 0, 0, NULL, ENT_ADDR(cpu->fivemin_avg), "main", 0, NULL, 0);  

  str = i_metric_valstr (cpu->fivemin_avg, NULL);
  item = i_form_string_add (reqdata->form_out, "fivemin_max", "5 Minute Maximum", str);
  if (str) free (str);
  i_form_item_add_link (item, 0, 0, 0, NULL, ENT_ADDR(cpu->fivemin_max), "main", 0, NULL, 0);  

  /* End Frame */
  i_form_frame_end (reqdata->form_out, obj->cnt->name_str);

  return 1;
}

