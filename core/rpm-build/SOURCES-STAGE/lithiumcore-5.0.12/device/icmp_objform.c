#include <stdio.h>
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
#include "icmp.h"

int l_icmp_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  l_icmp_item *icmp = obj->itemptr;
  i_form_item *fitem;

  /* 
   * ICMP Object
   */

  asprintf (&str, "ICMP IP Address '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);
  
  str = i_metric_valstr (icmp->reachable, NULL);
  i_form_string_add (reqdata->form_out, "reachable", "Reachable", str);
  free (str);
  
  str = i_metric_valstr (icmp->response, NULL);
  i_form_string_add (reqdata->form_out, "response", "Response", str);
  free (str);
  
  if (icmp->avail_flag == 1)
  { str = "Yes"; }
  else
  { str = "No"; }
  fitem = i_form_string_add (reqdata->form_out, "availrecord", "Record Availability", str); 
  if (icmp->avail_flag == 1)
  { 
    i_object *availobj;
    availobj = l_icmp_availobj ();
    i_form_item_add_link (fitem, 0, 0, 0, RES_ADDR(self), ENT_ADDR(availobj), NULL, 0, NULL, 0); 
  }

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

