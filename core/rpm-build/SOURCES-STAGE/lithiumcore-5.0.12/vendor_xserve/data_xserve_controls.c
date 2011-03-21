#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/parser.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/socket.h"
#include "induction/entity.h"
#include "induction/navtree.h"
#include "induction/navform.h"
#include "induction/hierarchy.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "device/snmp.h"

#include "fans.h"
#include "plist.h"
#include "data.h"

/*
 * Xserve Controls Processing
 */

int v_data_xserve_process_controls (i_resource *self, v_plist_req *req, xmlNodePtr controlArrayNode)
{
  /* Process each control */
  xmlNodePtr node;
  for (node = controlArrayNode->children; node; node = node->next)
  {
    /* Look through each control dict */
    if (strcmp((char *)node->name, "dict") == 0)
    {
      /* Get control type */
      char *type_str = v_plist_data_from_dict (req->plist, node, "type");
      if (!type_str) continue;
      if (strstr(type_str, "fan"))
      {
        /* Process a fan control */
        v_data_xserve_process_fan_control (self, req, node);
      }
      free (type_str);
      type_str = NULL;
    }
  }

  return 0;
}

int v_data_xserve_process_fan_control (i_resource *self, v_plist_req *req, xmlNodePtr dictNode)
{
  /* Get iface item */
  i_metric_value *val;
  v_fans_item *fan;
  char *fan_desc = v_plist_data_from_dict (req->plist, dictNode, "location");
  if (!fan_desc) return -1;
  fan = v_fans_get (fan_desc);
  if (!fan)
  { 
    fan = v_fans_create (self, fan_desc);
    if (!fan)
    { i_printf (0, "v_data_xserve_process_fan_control failed to create fan item for %s", fan_desc); free (fan_desc); return -1; }
  }
  if (fan_desc) free (fan_desc);
  fan_desc = NULL;

  /* Speed */
  val = i_metric_value_create ();
  val->gauge = v_plist_int_from_dict (req->plist, dictNode, "current-value");
  i_metric_value_enqueue (self, fan->rpm, val);
  fan->rpm->refresh_result = REFRESULT_OK;

  return 0;
}
