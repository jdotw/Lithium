#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/hierarchy.h>
#include <induction/interface.h>
#include <induction/ip.h>
#include <induction/ipregistry.h>
#include <induction/postgresql.h>
#include <induction/timeutil.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/form.h>
#include <induction/str.h>

#include "snmp.h"
#include "snmp_route.h"

int l_snmp_route_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  char *str;
  l_snmp_route_item *route = obj->itemptr;

  /* Start frame */
  asprintf (&str, "Route '%s' - Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, "route_frame", str);
  free (str);

  /* Destination/Mask */

  i_form_string_add (reqdata->form_out, "dest", "Destination", obj->desc_str);

  str = i_metric_valstr (route->mask, NULL);
  i_form_string_add (reqdata->form_out, "mask", "Mask", str);
  free (str);

  i_form_spacer_add (reqdata->form_out);

  /* Next hop / egress */

  str = i_metric_valstr (route->nexthop, NULL);
  i_form_string_add (reqdata->form_out, "nexthop", "Next Hop", str);
  free (str);

  if (route->iface && route->iface->obj && route->iface->obj->desc_str)
  {
    i_form_string_add (reqdata->form_out, "iface", "Egress Interface", route->iface->obj->desc_str);
  }
  else
  {
    str = i_metric_valstr (route->ifindex, NULL);
    i_form_string_add (reqdata->form_out, "ifindex", "Egress Interface Index", str);
    free (str);
  }

  i_form_spacer_add (reqdata->form_out);

  /* Metrics */

  str = i_metric_valstr (route->metric1, NULL);
  i_form_string_add (reqdata->form_out, "metric1", "Primary Metric", str);
  free (str);

  str = i_metric_valstr (route->metric2, NULL);
  i_form_string_add (reqdata->form_out, "metric2", "Metric 2", str);
  free (str);

  str = i_metric_valstr (route->metric3, NULL);
  i_form_string_add (reqdata->form_out, "metric3", "Metric 3", str);
  free (str);

  str = i_metric_valstr (route->metric4, NULL);
  i_form_string_add (reqdata->form_out, "metric4", "Metric 4", str);
  free (str);

  str = i_metric_valstr (route->metric5, NULL);
  i_form_string_add (reqdata->form_out, "metric5", "Metric 5", str);
  free (str);

  str = i_metric_valstr (route->metric5, NULL);
  i_form_string_add (reqdata->form_out, "metric5", "Metric 5", str);
  free (str);

  i_form_spacer_add (reqdata->form_out);

  /* Misc info */

  str = i_metric_valstr (route->type, NULL);
  i_form_string_add (reqdata->form_out, "type", "Type", str);
  free (str);

  str = i_metric_valstr (route->protocol, NULL);
  i_form_string_add (reqdata->form_out, "Protocol", "Protocol", str);
  free (str);

  str = i_metric_valstr (route->age, NULL);
  i_form_string_add (reqdata->form_out, "age", "Age", str);
  free (str);
  
  /* End Current Frame */
  i_form_frame_end (reqdata->form_out, "route_frame");

  return 1;
}

