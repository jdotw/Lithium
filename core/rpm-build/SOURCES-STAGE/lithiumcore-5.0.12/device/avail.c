#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>

#include "navtree.h"
#include "record.h"
#include "mformprio.h"
#include "avail.h"

/* Availability Sub-System 
 *
 * The Availability sub-system provides a uniform 
 * method of representing the availability of a 
 * system component. Typically, each device will 
 * have an SNMP object created under the availability
 * container. Each object in the availability 
 * sub-system has both availability percentage
 * metrics and response time metrics.
 */

static int static_enabled = 0;                        /* 0 = disabled / 1 = enabled */
static i_container *static_cnt = NULL;                /* Cement Container */
static i_object *static_masterobj = NULL;             /* The 'master' avail obj */

/* Variable Fetching */

i_container* l_avail_cnt ()
{ return static_cnt; }

i_object* l_avail_masterobj ()
{ return static_masterobj; }

/* Sub-System Enable / Disable */

int l_avail_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config refconfig;

  if (static_enabled == 1)
  { i_printf (1, "l_avail_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("avail", "Availability");
  if (!static_cnt)
  { i_printf (1, "l_avail_enable failed to create container"); l_avail_disable (self); return -1; }
  static_cnt->mainform_func = l_avail_cntform;
  static_cnt->sumform_func = l_avail_cntform_summary;
  static_cnt->navtree_func = l_navtree_func;
  static_cnt->prio = 1000;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "l_avail_enable failed to register container"); l_avail_disable (self); return -1; }

  /* Load/Apply refresh config 
   * The availability container will refresh twice for
   * every device refresh interval
   */

  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
//  refconfig.refresh_method = REFMETHOD_EXTERNAL;
//  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
//  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  refconfig.refresh_method = REFMETHOD_TIMER;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  refconfig.refresh_int_sec = self->hierarchy->dev->refresh_interval / 2;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &refconfig);
  if (num != 0)
  { i_printf (1, "l_avail_enable failed to load and apply container refresh config"); l_avail_disable (self); return -1; }

  /*
   * Item List etc 
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "l_avail_enable failed to create item_list"); l_avail_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /*
   * Create master object
   */

  /* Create object */
  static_masterobj = i_object_create ("master", "Master");
  static_masterobj->mainform_func = l_avail_objform;
  static_masterobj->histform_func = l_avail_objform_hist;
  static_masterobj->navtree_expand = NAVTREE_EXP_RESTRICT;
  static_masterobj->refresh_func = l_avail_refresh_master_obj;

  /* Create item */
  l_avail_item *item = l_avail_item_create ();
  item->obj = static_masterobj;
  static_masterobj->itemptr = item;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_masterobj));

  /* Setup refresh configuration default
   * to default to a REFMETHOD_PARENT 
   * refresh method. This default is applied
   * to all metrics and to the object
   * itself
   */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  
  /* Operation rate metrics */
  item->ok_pc = i_metric_create ("ok_pc", "Successful Rate", METRIC_FLOAT);
  i_entity_register (self, ENTITY(static_masterobj), ENTITY(item->ok_pc));
  i_entity_refresh_config_loadapply (self, ENTITY(item->ok_pc), &refconfig);
  item->ok_pc->unit_str = strdup ("%");
  item->ok_pc->record_method = RECMETHOD_RRD;
  item->ok_pc->record_defaultflag = 1;
  item->ok_pc->min_val = i_metric_value_create ();
  item->ok_pc->min_val->flt = 0.0;
  item->ok_pc->max_val = i_metric_value_create ();
  item->ok_pc->max_val->flt = 100.0;

  item->fail_pc = i_metric_create ("fail_pc", "Failure Rate", METRIC_FLOAT);
  i_entity_register (self, ENTITY(static_masterobj), ENTITY(item->fail_pc));
  i_entity_refresh_config_loadapply (self, ENTITY(item->fail_pc), &refconfig);
  item->fail_pc->unit_str = strdup ("%");
  item->fail_pc->record_method = RECMETHOD_RRD;
  item->fail_pc->record_defaultflag = 1;
  item->fail_pc->min_val = i_metric_value_create ();
  item->fail_pc->min_val->flt = 0.0;
  item->fail_pc->max_val = i_metric_value_create ();
  item->fail_pc->max_val->flt = 100.0;

  /* Response time metric */
  item->resptime = i_metric_create ("resptime", "Response Time", METRIC_FLOAT);
  item->resptime->unit_str = strdup ("ms");
  item->resptime->record_method = RECMETHOD_RRD;
  item->resptime->record_defaultflag = 1;
  i_entity_register (self, ENTITY(static_masterobj), ENTITY(item->resptime));
  i_entity_refresh_config_loadapply (self, ENTITY(item->resptime), &refconfig);

  /* Availability CGraph */
  item->avail_cg = i_metric_cgraph_create (static_masterobj, "avail_cg", "Availability");
  item->avail_cg->width_small = METRIC_CGRAPH_WIDTH_SMALL;
  item->avail_cg->height_small = METRIC_CGRAPH_HEIGHT_SMALL;
  item->avail_cg->upper_limit = 100;
  item->avail_cg->upper_limit_set = 1;
  item->avail_cg->lower_limit = 0;
  item->avail_cg->lower_limit_set = 1;
  item->avail_cg->rigid = 1;
  asprintf (&item->avail_cg->title_str, "%s Availability", static_masterobj->desc_str);
  asprintf (&item->avail_cg->render_str, "\"AREA:met_%s_ok_pc_avg#00AA00:Successful Operations\" \"AREA:met_%s_fail_pc_avg#AA0000:Failed Operations\" \"GPRINT:met_%s_ok_pc_avg:AVERAGE:          Average Availability\\: %%.2lf %%%%\"",
    static_masterobj->name_str, static_masterobj->name_str, static_masterobj->name_str);
  i_list_enqueue (item->avail_cg->met_list, item->ok_pc);
  i_list_enqueue (item->avail_cg->met_list, item->fail_pc);

  /* Load/Apply refresh configuration for the object */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  i_entity_refresh_config_loadapply (self, ENTITY(static_masterobj), &refconfig);
  
  /* Trigger Sets */
  static_masterobj->tset_list = i_list_create ();
  static_masterobj->tset_ht = i_hashtable_create (30);
  i_triggerset *tset = i_triggerset_create ("avail_pc", "Availability", "ok_pc");
  i_triggerset_addtrg (self, tset, "failed", "Failed", VALTYPE_FLOAT, TRGTYPE_EQUAL, 0, NULL, 0, NULL, self->hierarchy->dev->refresh_interval * 2, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_RANGE, 1, NULL, 61, NULL, 600, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 61, NULL, 90, NULL, 600, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  tset->default_applyflag = 1;
  i_triggerset_assign_obj (self, static_masterobj, tset);

  /* Evaluate all triggersets */
  i_triggerset_evalapprules_allsets (self, static_masterobj);

  /* Evaluate recording rules */
  l_record_eval_recrules_obj (self, static_masterobj);

  /* Enqueue the avail item */
  i_list_enqueue (static_cnt->item_list, item);

  return 0;  
}

int l_avail_disable (i_resource *self)
{
  /* Disable the sub-system */

  if (static_enabled == 0)
  { i_printf (1, "l_avail_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  /* Remove master object */
  if (static_masterobj)
  { l_avail_object_remove (self, static_masterobj); static_masterobj = NULL; }

  /* Deregister/Free container */
  if (static_cnt) 
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}


