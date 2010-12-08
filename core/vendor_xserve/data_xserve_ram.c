#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/socket.h>
#include <induction/entity.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>
#include <lithium/snmp.h>

#include "ram.h"
#include "plist.h"
#include "data.h"

/*
 * Xserve RAM Processing
 */

int v_data_xserve_process_memory (i_resource *self, v_plist_req *req, xmlNodePtr memoryArrayNode)
{
  /* Process each control */
  xmlNodePtr node;
  for (node = memoryArrayNode->children; node; node = node->next)
  {
    /* Loop through each memory dict */
    if (strcmp((char *)node->name, "dict") == 0)
    {
      /* Get ram item */
      i_metric_value *val;
      v_ram_item *ram;
      char *ram_desc = v_plist_data_from_dict (req->plist, node, "Slot");
      if (!ram_desc) return -1;
      ram = v_ram_get (ram_desc);
      if (!ram)
      { 
        ram = v_ram_create (self, ram_desc);
        if (!ram)
        { i_printf (0, "v_data_xserve_process_ram failed to create ram item for %s", ram_desc); free (ram_desc); return -1; }
      }
      free (ram_desc);
      ram_desc = NULL;

      /* Size */
      val = i_metric_value_create ();
      val->gauge = v_plist_int_from_dict (req->plist, node, "Size");
      i_metric_value_enqueue (self, ram->size, val);
      ram->size->refresh_result = REFRESULT_OK;

      /* Speed */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, node, "Speed");
      i_metric_value_enqueue (self, ram->speed, val);
      ram->speed->refresh_result = REFRESULT_OK;
      
      /* Type */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, node, "Type");
      i_metric_value_enqueue (self, ram->type, val);
      ram->type->refresh_result = REFRESULT_OK;
    }
  }

  return 0;
}
