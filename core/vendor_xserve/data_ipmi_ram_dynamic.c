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
#include "cpu.h"
#include "pci.h"
#include "psu.h"
#include "ram.h"
#include "mainboard.h"
#include "ipmitool.h"
#include "csv.h"
#include "plist.h"
#include "data.h"

/*
 * IPMI Apple dynamic ram data
 */

static int static_ram_index = 0;
static v_ipmitool_apple_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_ipmi_ram_dynamic_refresh (i_resource *self, i_metric *met, int opcode)
{
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh process */
      static_ram_index = 0;
      static_req = v_ipmitool_apple_get (self, "0xc9", "0x00", v_data_ipmi_ram_dynamic_ipmicb, met);
      break;

    case REFOP_COLLISION:   /* Handle collision */
      break;

    case REFOP_TERMINATE:   /* Terminate the refresh */
      if (static_req)
      {
        /* FIX must cancel request */
        static_req = NULL;
      }
      break;

    case REFOP_CLEANDATA:   /* Cleanup persistent refresh data */
      break;
  }

  return 0;
}

int v_data_ipmi_ram_dynamic_ipmicb (i_resource *self, v_ipmitool_apple_req *req, int result, void *passdata)
{
  i_metric *datamet = passdata;

  /* Check for data */
  if (!req->data || !req->datasize || result != IPMIRESULT_OK)
  {
    /* No data, error */
    /* Set result and terminate */
    datamet->refresh_result = REFRESULT_TOTAL_FAIL;
    i_entity_refresh_terminate (ENTITY(datamet));
    return -1;
  }

  /* Check for empty data */
  if (req->datasize < 60)
  {
    /* Single-response received, it's empty */
    datamet->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(datamet));
  }

  /* Get iface */
  int i = 0;
  i_container *cnt = v_ram_cnt ();
  i_object *obj;
  for (i_list_move_head(cnt->obj_list); (obj=i_list_restore(cnt->obj_list))!=NULL; i_list_move_next(cnt->obj_list))
  {
    if (i == static_ram_index) break;
    i++;
  }

  /* Process iface */
  if (obj)
  {
    v_ram_item *ram = obj->itemptr;
    
    /* Get IP */
    i_metric_value *val = i_metric_value_create ();
    val->count = (unsigned long) v_ipmitool_uint32 (req->data, req->datasize, 3);
    i_metric_value_enqueue (self, ram->ecc_errors, val);
    ram->ecc_errors->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(ram->ecc_errors));

    /* Kick off refresh for next item */
    static_ram_index++;
    if (static_ram_index > 18)
    {
      /* Maximum index reached */
      datamet->refresh_result = REFRESULT_TOTAL_FAIL;
      i_entity_refresh_terminate (ENTITY(datamet));
      i_printf (1, "v_data_ipmi_ram_dynamic_ipmicb failed, maximum index reached");
      return -1;
    }  
    char *index_str;
    asprintf (&index_str, "0x%.2x", static_ram_index);
    static_req = v_ipmitool_apple_get (self, "0xc9", index_str, v_data_ipmi_ram_dynamic_ipmicb, datamet);
    free (index_str);
    if (!static_req)
    {
      /* Failed to refrsh next */
      datamet->refresh_result = REFRESULT_TOTAL_FAIL;
      i_entity_refresh_terminate (ENTITY(datamet));
      i_printf (0, "v_data_ipmi_ram_dynamic_ipmicb failed to start refresh of item index %i", static_ram_index);
      return -1;
    }
  }
  else
  {
    /* Drive not found, end of refresh */
    datamet->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(datamet));
  }

  return 0;
}

