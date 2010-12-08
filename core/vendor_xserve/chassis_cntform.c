#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
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
#include "device/snmp.h"

#include "chassis.h"

int v_chassis_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Chassis Container Main Form
   */
  i_container *cnt = (i_container *) ent;
  
  /* Start Frame */
  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  /* 
   * Info
   */

  char *str;
  v_chassis_item *item = v_chassis_static_item ();;
    
  str = i_metric_valstr (item->systempower, NULL);
  i_form_string_add (reqdata->form_out, "systempower", "System Power Status", str);
  if (str) free (str);

  str = i_metric_valstr (item->poweroncause, NULL);
  i_form_string_add (reqdata->form_out, "poweroncause", "Last Power-On Cause", str);
  if (str) free (str);

  str = i_metric_valstr (item->poweroffcause, NULL);
  i_form_string_add (reqdata->form_out, "poweroffcause", "Last Power-Off Cause", str);
  if (str) free (str);

  str = i_metric_valstr (item->serial, NULL);
  i_form_string_add (reqdata->form_out, "serial", "Serial Number", str);
  if (str) free (str);

  str = i_metric_valstr (item->model, NULL);
  i_form_string_add (reqdata->form_out, "model", "Model", str);
  if (str) free (str);

  if (i_metric_valflt (item->powercontrolfault, NULL) == 1)
  {
    str = i_metric_valstr (item->powercontrolfault, NULL);
    i_form_string_add (reqdata->form_out, "powercontrolfault", "Power Control Fault", "TRUE");
    if (str) free (str);
  }    
  
  if (i_metric_valflt (item->mainspowerfault, NULL) == 1)
  {
    str = i_metric_valstr (item->mainspowerfault, NULL);
    i_form_string_add (reqdata->form_out, "mainspowerfault", "Mains Power Fault", "TRUE");
    if (str) free (str);
  }    
  
  if (i_metric_valflt (item->poweroverload, NULL) == 1)
  {
    str = i_metric_valstr (item->poweroverload, NULL);
    i_form_string_add (reqdata->form_out, "poweroverload", "Power Overload", "TRUE");
    if (str) free (str);
  }
  
//  if (i_metric_valflt (item->coolingfault, NULL) == 1)
//  {
//    str = i_metric_valstr (item->coolingfault, NULL);
//    i_form_string_add (reqdata->form_out, "coolingfault", "Cooling Fault", "TRUE");
//    if (str) free (str);
//  }    
  
//  if (i_metric_valflt (item->drivefault, NULL) == 1)
//  {
//    str = i_metric_valstr (item->drivefault, NULL);
//    i_form_string_add (reqdata->form_out, "drivefault", "Drive Fault", "TRUE");
//    if (str) free (str);
//  }    
  
  i_form_frame_end (reqdata->form_out, cnt->name_str);
  
  return 1;
}
