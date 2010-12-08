#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
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
#include "device/record.h"
#include "device/snmp.h"

#include "clientcount.h"

/* Client Count Sub-System */

static i_container *static_cnt = NULL;
static v_clientcount_item *static_item = NULL;

/* Variable Retrieval */

i_container* v_clientcount_cnt ()
{ return static_cnt; }

v_clientcount_item* v_clientcount_static_item ()
{ return static_item; }

/* Enable / Disable */

int v_clientcount_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_object *obj;
//  i_entity_refresh_config refconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("apclientcount", "Client Count");
  if (!static_cnt)
  { i_printf (1, "v_clientcount_enable failed to create container"); v_clientcount_disable (self); return -1; }
  static_cnt->mainform_func = v_clientcount_cntform;
  static_cnt->sumform_func = v_clientcount_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_clientcount_enable failed to register container"); v_clientcount_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_clientcount_enable failed to load and apply container refresh config"); v_clientcount_disable (self); return -1; }

  /*
   * Triggers
   */

  /* Client Count */
  i_triggerset *tset;
  tset = i_triggerset_create ("count", "Count", "count");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_GAUGE, TRGTYPE_GT, 50, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* 
   * Items and objects
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_clientcount_enable failed to create item_list"); v_clientcount_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  obj = i_object_create ("master", "Master");
  obj->mainform_func = v_clientcount_objform; 
  obj->histform_func = v_clientcount_objform_hist; 

  /* Register table object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Create item */
  v_clientcount_item *item;
  item = v_clientcount_item_create ();
  item->obj = obj;
  obj->itemptr = item;
  static_item = item;

  /*
   * Create Metrics 
   */ 

//  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
//  refconfig.refresh_method = REFMETHOD_EXTERNAL;
//  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
//  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

//  item->count = i_metric_create ("count", "Client Count", METRIC_GAUGE);
//  item->count->record_method = RECMETHOD_RRD;
//  item->count->record_defaultflag = 1;
//  i_entity_register (self, ENTITY(obj), ENTITY(item->count));
//  i_entity_refresh_config_apply (self, ENTITY(item->count), &refconfig);
//
  
  /* enterprises.63.501.3.2.1.0 is client count */
  item->count = l_snmp_metric_create (self, obj, "count", "Client Count", METRIC_GAUGE, "enterprises.63.501.3.2.1", "0", RECMETHOD_RRD, 0);
  item->count->record_defaultflag = 1;

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  return 0;
}

int v_clientcount_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

