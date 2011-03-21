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
#include "device/snmp.h"
#include "device/record.h"

#include "upsinfo.h"

/* UPS Info Sub-System */

static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Variable Retrieval */

i_container* v_upsinfo_cnt ()
{ return static_cnt; }

/* Enable / Disable */

int v_upsinfo_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("apcupsinfo", "UPS Information");
  if (!static_cnt)
  { i_printf (1, "v_upsinfo_enable failed to create container"); v_upsinfo_disable (self); return -1; }
  //static_cnt->mainform_func = v_upsinfo_cntform;
  //static_cnt->sumform_func = v_upsinfo_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_upsinfo_enable failed to register container"); v_upsinfo_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_upsinfo_enable failed to load and apply container refresh config"); v_upsinfo_disable (self); return -1; }

  /*
   * Trigger Sets 
   */

  /* Load */

//  tset = i_triggerset_create ("onemin_pc", "One Minute Load Average", "onemin_pc");
//  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 85, NULL, 0, NULL, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
//  i_triggerset_assign (self, static_cnt, tset);

//  tset = i_triggerset_create ("fivemin_pc", "Five Minute Load Average", "fivemin_pc");
//  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 70, NULL, 85, NULL, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
//  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 85, NULL, 0, NULL, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
//  i_triggerset_assign (self, static_cnt, tset);
  
  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_upsinfo_enable failed to create item_list"); v_upsinfo_disable (self); return -1; }
  i_list_set_destructor (static_cnt->item_list, v_upsinfo_item_free);
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "v_upsinfo_enable failed to create master object"); v_upsinfo_disable (self); return -1; }
  //static_obj->mainform_func = v_upsinfo_objform;

  /* Create upsinfo item */
  v_upsinfo_item *upsinfo = v_upsinfo_item_create ();
  if (!upsinfo)
  { i_printf (1, "v_upsinfo_enable failed to create upsinfo item struct"); v_upsinfo_disable (self); return -1; }
  upsinfo->obj = static_obj;
  static_obj->itemptr = upsinfo;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, upsinfo);
  if (num != 0)
  { i_printf (1, "v_upsinfo_enable failed to enqueue item into static_cnt->item_list"); v_upsinfo_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  /* Basic */
  upsinfo->model = l_snmp_metric_create (self, static_obj, "model", "Model", METRIC_STRING, ".1.3.6.1.4.1.318.1.1.1.1.1.1", "0", RECMETHOD_NONE, 0);
  upsinfo->name = l_snmp_metric_create (self, static_obj, "name", "Name", METRIC_STRING, ".1.3.6.1.4.1.318.1.1.1.1.1.2", "0", RECMETHOD_NONE, 0);

  /* Advanced */
  upsinfo->firmware = l_snmp_metric_create (self, static_obj, "firmware", "Firmware", METRIC_STRING, ".1.3.6.1.4.1.318.1.1.1.1.2.1", "0", RECMETHOD_NONE, 0);
  upsinfo->manufacture_date = l_snmp_metric_create (self, static_obj, "manufacture_date", "Manufacture Date", METRIC_STRING, ".1.3.6.1.4.1.318.1.1.1.1.2.2", "0", RECMETHOD_NONE, 0);
  upsinfo->serial = l_snmp_metric_create (self, static_obj, "serial", "Serial Number", METRIC_STRING, ".1.3.6.1.4.1.318.1.1.1.1.2.3", "0", RECMETHOD_NONE, 0);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);

  return 0;
}

int v_upsinfo_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

