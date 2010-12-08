#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
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
#include <induction/str.h>
#include "device/snmp.h"
#include "device/record.h"

#include "osx_server.h"
#include "data.h"
#include "network.h"

/* OS X / Xserve Network Interface Info */

static i_container *static_cnt = NULL;

/* Variable Retrieval */

i_container* v_network_cnt ()
{ return static_cnt; }

v_network_item* v_network_get (char *name_str)
{
  if (!static_cnt) return NULL;

  v_network_item *item;
  for (i_list_move_head(static_cnt->item_list); (item=i_list_restore(static_cnt->item_list))!=NULL; i_list_move_next(static_cnt->item_list))
  {
    if (strcmp(item->obj->name_str, name_str) == 0)
    { return item; }
  }

  return NULL;
}

/* Enable / Disable */

int v_network_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  if (v_xserve_intel_extras())
  { static_cnt = i_container_create ("xsinetwork", "Network Interfaces"); }
  else if (v_xserve_extras())
  { static_cnt = i_container_create ("xsnetwork", "Network Interfaces"); }
  else
  { static_cnt = i_container_create ("xnetwork", "Network Interfaces"); }
  if (!static_cnt)
  { i_printf (1, "v_network_enable failed to create container"); v_network_disable (self); return -1; }
  static_cnt->mainform_func = v_network_cntform;
  static_cnt->sumform_func = v_network_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_network_enable failed to register container"); v_network_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_network_enable failed to load and apply container refresh config"); v_network_disable (self); return -1; }

  /*
   * Triggers
   */

  /* 
   * Item and objects 
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_network_enable failed to create item_list"); v_network_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /*
   * Create Data Metrics 
   */

  if (v_xserve_intel_extras() == 0)
  {
    v_data_item *dataitem = v_data_static_item();

    /* Standard refresh config */
    memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
    defrefconfig.refresh_method = REFMETHOD_PARENT;
    defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
    defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

    /* Network metric */
    dataitem->network_state = i_metric_create ("network_state", "Network State", METRIC_INTEGER);
    i_metric_enumstr_add (dataitem->network_state, 0, "Invalid");
    i_metric_enumstr_add (dataitem->network_state, 1, "Current");
    i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->network_state));
    i_entity_refresh_config_apply (self, ENTITY(dataitem->network_state), &defrefconfig);
    dataitem->network_state->refresh_func = v_data_network_state_refresh;
  }

  return 0;
}

v_network_item* v_network_create (i_resource *self, char *name)
{
  char *name_str;
  char *desc_str;
  v_network_item *item;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Set name/desc */
  name_str = strdup (name);
  desc_str = strdup (name);

  /* Create object */
  obj = i_object_create (name_str, desc_str);
  free (name_str);
  free (desc_str);
  obj->mainform_func = v_network_objform;
  obj->histform_func = v_network_objform_hist;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Create item */
  item = v_network_item_create ();
  item->obj = obj;
  obj->itemptr = item;
  i_list_enqueue (static_cnt->item_list, item);

  /*
   * Create Metrics 
   */

  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  item->fullname = i_metric_create ("fullname", "Description", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->fullname));
  i_entity_refresh_config_apply (self, ENTITY(item->fullname), &refconfig);

  item->ip = i_metric_create ("ip", "IP Address", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->ip));
  i_entity_refresh_config_apply (self, ENTITY(item->ip), &refconfig);

  item->subnetmask = i_metric_create ("subnetmask", "Subnet Mask", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->subnetmask));
  i_entity_refresh_config_apply (self, ENTITY(item->subnetmask), &refconfig);

  if (!v_xserve_intel_extras())
  {
    item->broadcast = i_metric_create ("broadcast", "Broadcast", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->broadcast));
    i_entity_refresh_config_apply (self, ENTITY(item->broadcast), &refconfig);

    item->router = i_metric_create ("router", "Router Address", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->router));
    i_entity_refresh_config_apply (self, ENTITY(item->router), &refconfig);
  }
  
  if (v_xserve_extras())
  {
    /*
     * Special Xserve only metrics 
     */
    
    item->bytes_in = i_metric_create ("bytes_in", "Input Byte Count", METRIC_FLOAT);
    i_entity_register (self, ENTITY(obj), ENTITY(item->bytes_in));
    i_entity_refresh_config_apply (self, ENTITY(item->bytes_in), &refconfig);

    item->bytes_out = i_metric_create ("bytes_out", "Output Byte Count", METRIC_FLOAT);
    i_entity_register (self, ENTITY(obj), ENTITY(item->bytes_out));
    i_entity_refresh_config_apply (self, ENTITY(item->bytes_out), &refconfig);

    item->bps_in = i_metric_acrate_create (self, obj, "bps_in", "Input Bits Per Second", "bit/s", RECMETHOD_RRD, item->bytes_in, ACRATE_MOD_BYTETOBIT);
    item->bps_in->record_defaultflag = 1;
    item->bps_out = i_metric_acrate_create (self, obj, "bps_out", "Output Bits Per Second", "bit/s", RECMETHOD_RRD, item->bytes_out, ACRATE_MOD_BYTETOBIT);
    item->bps_out->record_defaultflag = 1;

    item->packets_in = i_metric_create ("packets_in", "Input Packet Count", METRIC_FLOAT);
    i_entity_register (self, ENTITY(obj), ENTITY(item->packets_in));
    i_entity_refresh_config_apply (self, ENTITY(item->packets_in), &refconfig);

    item->packets_out = i_metric_create ("packets_out", "Output Packet Count", METRIC_FLOAT);
    i_entity_register (self, ENTITY(obj), ENTITY(item->packets_out));
    i_entity_refresh_config_apply (self, ENTITY(item->packets_out), &refconfig);

    item->pps_in = i_metric_acrate_create (self, obj, "pps_in", "Input Packets Per Second", "pkts/s", RECMETHOD_RRD, item->packets_in, 0);
    item->pps_in->record_defaultflag = 1;
    item->pps_out = i_metric_acrate_create (self, obj, "pps_out", "Output Packets Per Second", "pkts/s", RECMETHOD_RRD, item->packets_out, 0);
    item->pps_out->record_defaultflag = 1;
    
    item->mac = i_metric_create ("mac", "MAC Address", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->mac));
    i_entity_refresh_config_apply (self, ENTITY(item->mac), &refconfig);

    if (!v_xserve_intel_extras())
    {
      item->kind = i_metric_create ("kind", "Type", METRIC_STRING);
      i_entity_register (self, ENTITY(obj), ENTITY(item->kind));
      i_entity_refresh_config_apply (self, ENTITY(item->kind), &refconfig);
    }

    item->duplex = i_metric_create ("duplex", "Duplex", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->duplex));
    i_entity_refresh_config_apply (self, ENTITY(item->duplex), &refconfig);

    item->speed = i_metric_create ("speed", "Speed", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->speed));
    i_entity_refresh_config_apply (self, ENTITY(item->speed), &refconfig);

    item->linkstate = i_metric_create ("linkstate", "Link State", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->linkstate));
    i_entity_refresh_config_apply (self, ENTITY(item->linkstate), &refconfig);
  }
  else
  {
    /* 
     * Exclusively non-Xserve Metrics 
     */
    item->bps_in = i_metric_create ("bps_in", "Input Bits per Second", METRIC_GAUGE);
    item->bps_in->unit_str = strdup ("bps");
    item->bps_in->record_method = RECMETHOD_RRD;
    item->bps_in->record_defaultflag = 1;
    item->bps_in->alloc_unit = 8;
    item->bps_in->valstr_func = i_string_volume_metric;
    i_entity_register (self, ENTITY(obj), ENTITY(item->bps_in));
    i_entity_refresh_config_apply (self, ENTITY(item->bps_in), &refconfig);

    item->bps_out = i_metric_create ("bps_out", "Output Bits per Second", METRIC_GAUGE);
    item->bps_out->unit_str = strdup ("bps");
    item->bps_out->record_method = RECMETHOD_RRD;
    item->bps_out->record_defaultflag = 1;
    item->bps_out->alloc_unit = 8;
    item->bps_out->valstr_func = i_string_volume_metric;
    i_entity_register (self, ENTITY(obj), ENTITY(item->bps_out));
    i_entity_refresh_config_apply (self, ENTITY(item->bps_out), &refconfig);
  }

  /*
   * Combined/Custom Graphs
   */
  item->tput_cg = i_metric_cgraph_create (obj, "tput_cg", "Bits/sec");
  asprintf (&item->tput_cg->title_str, "Interface '%s' Throughput", obj->desc_str);
  asprintf (&item->tput_cg->render_str, "\"CDEF:kbps_in_min=met_%s_bps_in_min,1000,/\" \"CDEF:kbps_in_avg=met_%s_bps_in_avg,1000,/\" \"CDEF:kbps_in_max=met_%s_bps_in_max,1000,/\" \"CDEF:kbps_out_min=met_%s_bps_out_min,1000,/\" \"CDEF:kbps_out_avg=met_%s_bps_out_avg,1000,/\" \"CDEF:kbps_out_max=met_%s_bps_out_max,1000,/\" \"LINE1:met_%s_bps_in_min#000E73:Min.\" \"LINE1:met_%s_bps_in_avg#001EFF:Avg.\" \"LINE1:met_%s_bps_in_max#00B4FF: Max. Input\" \"GPRINT:kbps_in_min:MIN:   Min %%.2lf Kbits/s\" \"GPRINT:kbps_in_avg:AVERAGE: Avg %%.2lf Kbits/s\" \"GPRINT:kbps_in_max:MAX: Max %%.2lf Kbits/s\\n\" \"LINE1:met_%s_bps_out_min#006B00:Min.\" \"LINE1:met_%s_bps_out_avg#009B00:Avg.\" \"LINE1:met_%s_bps_out_max#00ED00: Max. Output\" \"GPRINT:kbps_out_min:MIN:  Min %%.2lf Kbits/s\" \"GPRINT:kbps_out_avg:AVERAGE: Avg %%.2lf Kbits/s\" \"GPRINT:kbps_out_max:MAX: Max %%.2lf Kbits/s\\n\"",
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str);
  i_list_enqueue (item->tput_cg->met_list, item->bps_in);
  i_list_enqueue (item->tput_cg->met_list, item->bps_out);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  return item;
}

int v_network_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

