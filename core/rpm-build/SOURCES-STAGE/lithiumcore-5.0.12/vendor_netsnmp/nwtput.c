#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/navtree.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/value.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "device/snmp.h"
#include "device/record.h"
#include "device/nwtput.h"

/* Net-SNMP Network Throughput Additions */

int l_snmp_nsnwtput_enable (i_resource *self)
{
  /* Enables l_nwtput and adds our netsnmp additions */
  l_nwtput_enable (self);
  i_container *cnt = l_nwtput_cnt ();
  
  /*
   * TCP Item 
   */

  i_object *obj;
  i_metric *raw;
  i_metric *met;

  obj = i_object_create ("tcp", "TCP Segments");
  obj->cnt = cnt;
//  obj->mainform_func = l_nwtput_objform;
//  obj->histform_func = l_nwtput_objform_hist;
  obj->navtree_expand = NAVTREE_EXP_RESTRICT;
  
  l_nwtput_item *item = l_nwtput_item_create ();
  obj->itemptr = item;
  item->obj = obj;

  i_entity_register (self, ENTITY(cnt), ENTITY(obj));

  i_entity_refresh_config refconfig;
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  int num = i_entity_refresh_config_loadapply (self, ENTITY(obj), &refconfig);
  
  raw = l_snmp_metric_create (self, obj, "input_raw", "Input Segment Count", METRIC_COUNT, ".1.3.6.1.2.1.6.10.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  item->input = i_metric_acrate_create (self, obj, "input", "Input", "seg/sec", RECMETHOD_RRD, raw, 0);
  item->input->record_defaultflag = 1;

  raw = l_snmp_metric_create (self, obj, "output_raw", "Output Segment Count", METRIC_COUNT, ".1.3.6.1.2.1.6.11.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  item->output = i_metric_acrate_create (self, obj, "output", "Output", "seg/sec", RECMETHOD_RRD, raw, 0);
  item->output->record_defaultflag = 1;

  raw = l_snmp_metric_create (self, obj, "retrans_count", "Retransmission Count", METRIC_COUNT, ".1.3.6.1.2.1.6.12.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  met = i_metric_acrate_create (self, obj, "retrans", "Retransmissions", "rt/sec", RECMETHOD_RRD, raw, 0);
  item->output->record_defaultflag = 1;

  met = l_snmp_metric_create (self, obj, "curconn", "Current Connections", METRIC_GAUGE, ".1.3.6.1.2.1.6.9.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);

  i_list_enqueue (cnt->item_list, item);  
  i_triggerset_evalapprules_allsets (self, obj);
  l_record_eval_recrules_obj (self, obj);
  
  /*
   * UDP Item 
   */

  obj = i_object_create ("udp", "UDP Datagrams");
  obj->cnt = cnt;
//  obj->mainform_func = l_nwtput_objform;
//  obj->histform_func = l_nwtput_objform_hist;
  obj->navtree_expand = NAVTREE_EXP_RESTRICT;

  item = l_nwtput_item_create ();
  obj->itemptr = item;
  item->obj = obj;

  i_entity_register (self, ENTITY(cnt), ENTITY(obj));

  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), &refconfig);

  raw = l_snmp_metric_create (self, obj, "input_raw", "Input Datagram Count", METRIC_COUNT, ".1.3.6.1.2.1.7.1.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  item->input = i_metric_acrate_create (self, obj, "input", "Input", "dg/sec", RECMETHOD_RRD, raw, 0);
  item->input->record_defaultflag = 1;

  raw = l_snmp_metric_create (self, obj, "output_raw", "Output Datagram Count", METRIC_COUNT, ".1.3.6.1.2.1.7.4.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  item->output = i_metric_acrate_create (self, obj, "output", "Output", "dg/sec", RECMETHOD_RRD, raw, 0);
  item->output->record_defaultflag = 1;

  i_list_enqueue (cnt->item_list, item);
  i_triggerset_evalapprules_allsets (self, obj);
  l_record_eval_recrules_obj (self, obj);
  
  return 0;
}
