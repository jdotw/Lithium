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
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/interface.h>
#include <induction/form.h>
#include <induction/str.h>

#include "snmp.h"
#include "snmp_ipaddr.h"

int l_snmp_ipaddr_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_form_item *fitem;
  i_object *obj = (i_object *) ent;
  l_snmp_ipaddr *addr = obj->itemptr;

  /* Check Item */
  if (obj->cnt != l_snmp_ipaddr_cnt())
  { i_form_string_add (reqdata->form_out, "error", "Error", "Invalid item."); return 1; }
  if (!addr)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No item present."); return 1; }

  /* Start Frame */
  asprintf (&str, "IP Address %s", ent->desc_str);
  i_form_frame_start (reqdata->form_out, ent->name_str, str);
  free (str);

  /* 
   * Info Strings 
   */

  str = i_metric_valstr (addr->addr, NULL);
  i_form_string_add (reqdata->form_out, "addr", "Address", str);
  if (str) free (str);

  str = i_metric_valstr (addr->netmask, NULL);
  i_form_string_add (reqdata->form_out, "netmask", "Netmask", str);
  if (str) free (str);

  str = i_metric_valstr (addr->ifindex, NULL);
  i_form_string_add (reqdata->form_out, "ifindex", "Interface Index", str);
  if (str) free (str);

  if (addr->iface)
  {
    fitem = i_form_string_add (reqdata->form_out, "iface", "Interface Description", addr->iface->obj->desc_str);
    i_form_item_add_link (fitem, 0, 0, 0, RES_ADDR(self), ENT_ADDR(addr->iface->obj), NULL, 0, NULL, 0);
  }

  /* End Frame */
  i_form_frame_end (reqdata->form_out, ent->name_str);

  return 1;
}

