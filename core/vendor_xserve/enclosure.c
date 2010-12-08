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
#include <lithium/snmp.h>
#include <lithium/record.h>

#include "osx_server.h"
#include "data.h"
#include "enclosure.h"

/* Xraid Enclosure Sub-System */

static i_container *static_cnt = NULL;
static v_enclosure_item *static_item = NULL;

/* Variable Retrieval */

i_container* v_enclosure_cnt ()
{ return static_cnt; }

v_enclosure_item* v_enclosure_static_item ()
{ return static_item; }

/* Enable / Disable */

int v_enclosure_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Create/Config Container */
  if (v_xserve_intel_extras())
  { static_cnt = i_container_create ("xsienclosure", "Enclosure"); }
  else
  { static_cnt = i_container_create ("xsenclosure", "Enclosure"); }
  if (!static_cnt)
  { i_printf (1, "v_enclosure_enable failed to create container"); v_enclosure_disable (self); return -1; }
  static_cnt->mainform_func = v_enclosure_cntform;
  static_cnt->sumform_func = v_enclosure_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;
  
  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_enclosure_enable failed to register container"); v_enclosure_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_enclosure_enable failed to load and apply container refresh config"); v_enclosure_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_enclosure_enable failed to create item_list"); v_enclosure_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create table object */
  obj = i_object_create ("master", "Master");
  obj->mainform_func = v_enclosure_objform;

  /* Register table object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Create item */
  static_item = v_enclosure_item_create ();
  static_item->obj = obj;
  obj->itemptr = static_item;

  /*
   * Create Metrics 
   */ 
  
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  static_item->int_amb_temp = i_metric_create ("int_amb_temp", "Internal Ambient Temperature", METRIC_FLOAT);
  static_item->int_amb_temp->unit_str = strdup ("deg.C");
  static_item->int_amb_temp->record_method = RECMETHOD_RRD;
  static_item->int_amb_temp->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->int_amb_temp));
  i_entity_refresh_config_apply (self, ENTITY(static_item->int_amb_temp), &refconfig);

  static_item->sc_amb_temp = i_metric_create ("sc_amb_temp", "System Controller Ambient Temperature", METRIC_FLOAT);
  static_item->sc_amb_temp->unit_str = strdup ("deg.C");
  static_item->sc_amb_temp->record_method = RECMETHOD_RRD;
  static_item->sc_amb_temp->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->sc_amb_temp));
  i_entity_refresh_config_apply (self, ENTITY(static_item->sc_amb_temp), &refconfig);

  static_item->sc_int_temp = i_metric_create ("sc_int_temp", "System Controller Internal Temperature", METRIC_FLOAT);
  static_item->sc_int_temp->unit_str = strdup ("deg.C");
  static_item->sc_int_temp->record_method = RECMETHOD_RRD;
  static_item->sc_int_temp->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->sc_int_temp));
  i_entity_refresh_config_apply (self, ENTITY(static_item->sc_int_temp), &refconfig);

  static_item->memory_temp = i_metric_create ("memory_temp", "RAM Temperature", METRIC_FLOAT);
  static_item->memory_temp->unit_str = strdup ("deg.C");
  static_item->memory_temp->record_method = RECMETHOD_RRD;
  static_item->memory_temp->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->memory_temp));
  i_entity_refresh_config_apply (self, ENTITY(static_item->memory_temp), &refconfig);

  static_item->pci_temp = i_metric_create ("pci_temp", "PCI Slot Temperature", METRIC_FLOAT);
  static_item->pci_temp->unit_str = strdup ("deg.C");
  static_item->pci_temp->record_method = RECMETHOD_RRD;
  static_item->pci_temp->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->pci_temp));
  i_entity_refresh_config_apply (self, ENTITY(static_item->pci_temp), &refconfig);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  return 0;
}

int v_enclosure_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

