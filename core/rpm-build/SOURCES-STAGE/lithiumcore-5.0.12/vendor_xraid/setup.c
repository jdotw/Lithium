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

#include "setup.h"

/* Xraid Setup Information Sub-System */

static i_container *static_cnt = NULL;
static v_setup_item *static_top_item = NULL;
static v_setup_item *static_bottom_item = NULL;

/* Variable Retrieval */

i_container* v_setup_cnt ()
{ return static_cnt; }

v_setup_item* v_setup_static_top_item ()
{ return static_top_item; }

v_setup_item* v_setup_static_bottom_item ()
{ return static_bottom_item; }

/* Enable / Disable */

int v_setup_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("xrsetup", "Controller Config");
  if (!static_cnt)
  { i_printf (1, "v_setup_enable failed to create container"); v_setup_disable (self); return -1; }
//  static_cnt->mainform_func = v_setup_cntform;
//  static_cnt->sumform_func = v_setup_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_setup_enable failed to register container"); v_setup_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_setup_enable failed to load and apply container refresh config"); v_setup_disable (self); return -1; }

  /*
   * Triggers
   */

  /* 
   * Items and objects
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_setup_enable failed to create item_list"); v_setup_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create top and bottom items */
  int i;
  for (i=0; i < 2; i++)
  {  
    /* Create table object */
    if (i == 0)
    { obj = i_object_create ("upper", "Upper Controller"); }
    else
    { obj = i_object_create ("lower", "Lower Controller"); }
    
//  obj->mainform_func = v_ipacct_data_objform; 

    /* Register table object */
    i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

    /* Create item */
    v_setup_item *item;
    item = v_setup_item_create ();
    item->obj = obj;
    obj->itemptr = item;
    if (i == 0) static_top_item = item;
    else static_bottom_item = item;

    /*
     * Create Metrics 
     */ 
  
    memset (&refconfig, 0, sizeof(i_entity_refresh_config));
    refconfig.refresh_method = REFMETHOD_EXTERNAL;
    refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
    refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

    item->stripesize = i_metric_create ("stripesize", "Stripe Size", METRIC_INTEGER);
    i_entity_register (self, ENTITY(obj), ENTITY(item->stripesize));
    i_entity_refresh_config_apply (self, ENTITY(item->stripesize), &refconfig);

    item->writebuffer_enabled = i_metric_create ("writebuffer_enabled", "Write Buffer Enabled", METRIC_INTEGER);
    i_entity_register (self, ENTITY(obj), ENTITY(item->writebuffer_enabled));
    i_entity_refresh_config_apply (self, ENTITY(item->writebuffer_enabled), &refconfig);
    i_metric_enumstr_add (item->writebuffer_enabled, 0, "Disabled");
    i_metric_enumstr_add (item->writebuffer_enabled, 1, "Enabled");

    item->memorysize = i_metric_create ("memorysize", "Memory Size", METRIC_INTEGER);
    item->memorysize->unit_str = strdup ("Mb");
    i_entity_register (self, ENTITY(obj), ENTITY(item->memorysize));
    i_entity_refresh_config_apply (self, ENTITY(item->memorysize), &refconfig);

    item->accessmode = i_metric_create ("accessmode", "Access Mode", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(item->accessmode));
    i_entity_refresh_config_apply (self, ENTITY(item->accessmode), &refconfig);

    item->prefetchsize = i_metric_create ("prefetchsize", "Pre-fetch Size", METRIC_INTEGER);
    i_entity_register (self, ENTITY(obj), ENTITY(item->prefetchsize));
    i_entity_refresh_config_apply (self, ENTITY(item->prefetchsize), &refconfig);

    item->autorebuild_enabled = i_metric_create ("autorebuild_enabled", "Auto Rebuild Enabled", METRIC_INTEGER);
    i_metric_enumstr_add (item->autorebuild_enabled, 0, "Disabled");
    i_metric_enumstr_add (item->autorebuild_enabled, 1, "Enabled");
    i_entity_register (self, ENTITY(obj), ENTITY(item->autorebuild_enabled));
    i_entity_refresh_config_apply (self, ENTITY(item->autorebuild_enabled), &refconfig);

    item->rebuildrate = i_metric_create ("rebuildrate", "Rebuild Rate", METRIC_INTEGER);
    i_entity_register (self, ENTITY(obj), ENTITY(item->rebuildrate));
    i_entity_refresh_config_apply (self, ENTITY(item->rebuildrate), &refconfig);

    item->checkdata_enabled = i_metric_create ("checkdata_enabled", "Check Data Generation Enabled", METRIC_INTEGER);
    i_metric_enumstr_add (item->checkdata_enabled, 0, "Disabled");
    i_metric_enumstr_add (item->checkdata_enabled, 1, "Enabled");
    i_entity_register (self, ENTITY(obj), ENTITY(item->checkdata_enabled));
    i_entity_refresh_config_apply (self, ENTITY(item->checkdata_enabled), &refconfig);

    item->lunmask_enabled = i_metric_create ("lunmask_enabled", "LUN Mask Enabled", METRIC_INTEGER);
    i_entity_register (self, ENTITY(obj), ENTITY(item->lunmask_enabled));
    i_entity_refresh_config_apply (self, ENTITY(item->lunmask_enabled), &refconfig);
    i_metric_enumstr_add (item->lunmask_enabled, 0, "Disabled");
    i_metric_enumstr_add (item->lunmask_enabled, 1, "Enabled");

    item->jbodmask_enabled = i_metric_create ("jbodmask_enabled", "JBID Mask Enabled", METRIC_INTEGER);
    i_entity_register (self, ENTITY(obj), ENTITY(item->jbodmask_enabled));
    i_entity_refresh_config_apply (self, ENTITY(item->jbodmask_enabled), &refconfig);
    i_metric_enumstr_add (item->jbodmask_enabled, 0, "Disabled");
    i_metric_enumstr_add (item->jbodmask_enabled, 1, "Enabled");

    /* Evaluate apprules for all triggersets */
    i_triggerset_evalapprules_allsets (self, obj);

    /* Evaluate recrules for all metrics */
    l_record_eval_recrules_obj (self, obj);
  }

  return 0;
}

int v_setup_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

