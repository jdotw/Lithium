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
#include "induction/str.h"
#include "device/snmp.h"

#include "arrays.h"

/* Xraid System Information Sub-System */

static i_list* static_top_arrays = NULL;
static i_list* static_bottom_arrays = NULL;
static int static_array_count = 0;

/* Variable Retrieval */

v_arrays_item* v_arrays_get_item (int controller, int id)
{
  i_list *array_list;
  if (controller == 0)
  {
    /* Top array */
    array_list = static_top_arrays;
  }
  else
  {
    /* Bottom array */
    array_list = static_bottom_arrays;
  }

  /* Get the array */
  int i;
  i_list_move_head (array_list);
  for (i=1; i < id; i++)
  {
    i_list_move_next (array_list); 
  }

  return i_list_restore (array_list);
}

/* Enable / Disable */

int v_arrays_enable (i_resource *self)
{
  /* Create array lists */
  static_top_arrays = i_list_create ();
  i_list_set_destructor (static_top_arrays, v_arrays_item_free);
  static_bottom_arrays = i_list_create ();
  i_list_set_destructor (static_bottom_arrays, v_arrays_item_free);

  return 0;
}

v_arrays_item* v_arrays_create (i_resource *self, int controller)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_object *obj;
  i_entity_refresh_config refconfig;
  char *name_str;
  char *desc_str;
  i_container *cnt;
  
  /* Increment counter */
  static_array_count++;

  /* Create/Config Container */
  asprintf (&name_str, "xrarray_%i", static_array_count);
  asprintf (&desc_str, "Array %i", static_array_count);
  cnt = i_container_create (name_str, desc_str);
  free (name_str);
  free (desc_str);
  if (!cnt)
  { i_printf (1, "v_arrays_create failed to create container"); return NULL; }
  cnt->mainform_func = v_arrays_cntform;
  cnt->sumform_func = v_arrays_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(cnt));
  if (num != 0)
  { i_printf (1, "v_arrays_create failed to register container"); return NULL; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_arrays_create failed to load and apply container refresh config"); return NULL; }

  /* Create item list */
  cnt->item_list = i_list_create ();
  if (!cnt->item_list)
  { i_printf (1, "v_arrays_create failed to create item_list"); return NULL; }
  cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create info object */
  v_arrays_item *item;

  /* Create table object */
  obj = i_object_create ("master", "Master");
  obj->mainform_func = v_arrays_objform;

  /* Register table object */
  i_entity_register (self, ENTITY(cnt), ENTITY(obj));

  /* Create item */
  item = v_arrays_item_create ();
  item->obj = obj;
  item->cnt = cnt;
  obj->itemptr = item;
  item->index = static_array_count;

  /*
   * Triggers
   */
  i_triggerset *tset;
  
  /* Array Status */
  tset = i_triggerset_create ("raidstatus", "Array Status", "raidstatus");
  i_triggerset_addtrg (self, tset, "critical", "Critical", VALTYPE_STRING, TRGTYPE_NOTEQUAL, 0, "online", 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, cnt, tset);

  /* Initialising */
  tset = i_triggerset_create ("initialize", "Initialization", "initialize");
  i_triggerset_addtrg (self, tset, "inprogress", "In Progress", VALTYPE_INTEGER, TRGTYPE_LT, 255, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, cnt, tset);

  /* Add Member */
  tset = i_triggerset_create ("addmember", "Add Member", "addmember");
  i_triggerset_addtrg (self, tset, "inprogress", "In Progress", VALTYPE_INTEGER, TRGTYPE_LT, 255, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, cnt, tset);

  /* Verify */
  tset = i_triggerset_create ("verify", "Verify", "verify");
  i_triggerset_addtrg (self, tset, "inprogress", "In Progress", VALTYPE_INTEGER, TRGTYPE_LT, 255, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, cnt, tset);

  /* Expand */
  tset = i_triggerset_create ("expand", "Expand", "expand");
  i_triggerset_addtrg (self, tset, "inprogress", "In Progress", VALTYPE_INTEGER, TRGTYPE_LT, 255, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, cnt, tset);

  /*
   * Create Metrics 
   */ 
  
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  item->stripesize = i_metric_create ("stripesize", "Stripe Size", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->stripesize));
  i_entity_refresh_config_apply (self, ENTITY(item->stripesize), &refconfig);
    
  item->raidlevel = i_metric_create ("raidlevel", "RAID Level", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->raidlevel));
  i_entity_refresh_config_apply (self, ENTITY(item->raidlevel), &refconfig);
    
  item->membercount = i_metric_create ("membercount", "Member Count", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->membercount));
  i_entity_refresh_config_apply (self, ENTITY(item->membercount), &refconfig);
    
  item->raidtype = i_metric_create ("raidtype", "RAID Type", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->raidtype));
  i_entity_refresh_config_apply (self, ENTITY(item->raidtype), &refconfig);
    
  item->raidstatus = i_metric_create ("raidstatus", "RAID Status", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->raidstatus));
  i_entity_refresh_config_apply (self, ENTITY(item->raidstatus), &refconfig);
    
  item->sectorcapacity = i_metric_create ("sectorcapacity", "Sector Capacity", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(item->sectorcapacity));
  i_entity_refresh_config_apply (self, ENTITY(item->sectorcapacity), &refconfig);
    
  item->capacity = i_metric_create ("capacity", "Capacity", METRIC_GAUGE);
  item->capacity->alloc_unit = 512;
  item->capacity->unit_str = strdup ("b");
  item->capacity->kbase = 1024;
  item->capacity->valstr_func = i_string_volume_metric;
  i_entity_register (self, ENTITY(obj), ENTITY(item->capacity));
  i_entity_refresh_config_apply (self, ENTITY(item->capacity), &refconfig);

  item->initialize = i_metric_create ("initialize", "Initialization Progress", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->initialize));
  i_entity_refresh_config_apply (self, ENTITY(item->initialize), &refconfig);

  item->addmember = i_metric_create ("addmember", "Add Member Progress", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->addmember));
  i_entity_refresh_config_apply (self, ENTITY(item->addmember), &refconfig);

  item->verify = i_metric_create ("verify", "Verification Progress", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->verify));
  i_entity_refresh_config_apply (self, ENTITY(item->verify), &refconfig);

  item->expand = i_metric_create ("expand", "Expand Progress", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->expand));
  i_entity_refresh_config_apply (self, ENTITY(item->expand), &refconfig);

  item->slice1_size = i_metric_create ("slice1_size", "Slice 1 Size", METRIC_GAUGE);
  item->slice1_size->alloc_unit = 1048576;
  item->slice1_size->unit_str = strdup ("b");
  item->slice1_size->kbase = 1024;
  item->slice1_size->valstr_func = i_string_volume_metric;
  i_entity_register (self, ENTITY(obj), ENTITY(item->slice1_size));
  i_entity_refresh_config_apply (self, ENTITY(item->slice1_size), &refconfig);

  item->slice2_size = i_metric_create ("slice2_size", "Slice 2 Size", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(item->slice2_size));
  i_entity_refresh_config_apply (self, ENTITY(item->slice2_size), &refconfig);

  item->slice3_size = i_metric_create ("slice3_size", "Slice 3 Size", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(item->slice3_size));
  i_entity_refresh_config_apply (self, ENTITY(item->slice3_size), &refconfig);

  item->slice4_size = i_metric_create ("slice4_size", "Slice 4 Size", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(item->slice4_size));
  i_entity_refresh_config_apply (self, ENTITY(item->slice4_size), &refconfig);

  item->slice5_size = i_metric_create ("slice5_size", "Slice 5 Size", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(item->slice5_size));
  i_entity_refresh_config_apply (self, ENTITY(item->slice5_size), &refconfig);

  item->slice6_size = i_metric_create ("slice6_size", "Slice 6 Size", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(item->slice6_size));
  i_entity_refresh_config_apply (self, ENTITY(item->slice6_size), &refconfig);

  item->slice7_size = i_metric_create ("slice7_size", "Slice 7 Size", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(item->slice7_size));
  i_entity_refresh_config_apply (self, ENTITY(item->slice7_size), &refconfig);

  item->slice8_size = i_metric_create ("slice8_size", "Slice 8 Size", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(item->slice8_size));
  i_entity_refresh_config_apply (self, ENTITY(item->slice8_size), &refconfig);

  item->member1_slot = i_metric_create ("member1_slot", "Member 1 Slot", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->member1_slot));
  i_entity_refresh_config_apply (self, ENTITY(item->member1_slot), &refconfig);

  item->member2_slot = i_metric_create ("member2_slot", "Member 2 Slot", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->member2_slot));
  i_entity_refresh_config_apply (self, ENTITY(item->member2_slot), &refconfig);

  item->member3_slot = i_metric_create ("member3_slot", "Member 3 Slot", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->member3_slot));
  i_entity_refresh_config_apply (self, ENTITY(item->member3_slot), &refconfig);

  item->member4_slot = i_metric_create ("member4_slot", "Member 4 Slot", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->member4_slot));
  i_entity_refresh_config_apply (self, ENTITY(item->member4_slot), &refconfig);

  item->member5_slot = i_metric_create ("member5_slot", "Member 5 Slot", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->member5_slot));
  i_entity_refresh_config_apply (self, ENTITY(item->member5_slot), &refconfig);

  item->member6_slot = i_metric_create ("member6_slot", "Member 6 Slot", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->member6_slot));
  i_entity_refresh_config_apply (self, ENTITY(item->member6_slot), &refconfig);

  item->member7_slot = i_metric_create ("member7_slot", "Member 7 Slot", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->member7_slot));
  i_entity_refresh_config_apply (self, ENTITY(item->member7_slot), &refconfig);

  /* Enqueue item */
  if (controller == 0)
  { i_list_enqueue (static_top_arrays, item); }
  else
  { i_list_enqueue (static_bottom_arrays, item); }
    
  return item;
}

int v_arrays_disable (i_resource *self)
{
  /* Deregister container */

  return 0;
}

