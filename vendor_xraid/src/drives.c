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
#include <lithium/snmp.h>
#include <lithium/record.h>

#include "drives.h"

/* Xraid System Information Sub-System */

static i_container *static_cnt = NULL;
static v_drives_item *static_drive_1 = NULL;
static v_drives_item *static_drive_2 = NULL;
static v_drives_item *static_drive_3 = NULL;
static v_drives_item *static_drive_4 = NULL;
static v_drives_item *static_drive_5 = NULL;
static v_drives_item *static_drive_6 = NULL;
static v_drives_item *static_drive_7 = NULL;
static v_drives_item *static_drive_8 = NULL;
static v_drives_item *static_drive_9 = NULL;
static v_drives_item *static_drive_10 = NULL;
static v_drives_item *static_drive_11 = NULL;
static v_drives_item *static_drive_12 = NULL;
static v_drives_item *static_drive_13 = NULL;
static v_drives_item *static_drive_14 = NULL;

/* Variable Retrieval */

i_container* v_drives_cnt ()
{ return static_cnt; }

v_drives_item* v_drives_1 () { return static_drive_1; }
v_drives_item* v_drives_2 () { return static_drive_2; }
v_drives_item* v_drives_3 () { return static_drive_3; }
v_drives_item* v_drives_4 () { return static_drive_4; }
v_drives_item* v_drives_5 () { return static_drive_5; }
v_drives_item* v_drives_6 () { return static_drive_6; }
v_drives_item* v_drives_7 () { return static_drive_7; }
v_drives_item* v_drives_8 () { return static_drive_8; }
v_drives_item* v_drives_9 () { return static_drive_9; }
v_drives_item* v_drives_10 () { return static_drive_10; }
v_drives_item* v_drives_11 () { return static_drive_11; }
v_drives_item* v_drives_12 () { return static_drive_12; }
v_drives_item* v_drives_13 () { return static_drive_13; }
v_drives_item* v_drives_14 () { return static_drive_14; }

v_drives_item* v_drives_x (int x)
{
  switch (x)
  {
    case 1: return static_drive_1; 
    case 2: return static_drive_2; 
    case 3: return static_drive_3; 
    case 4: return static_drive_4; 
    case 5: return static_drive_5; 
    case 6: return static_drive_6; 
    case 7: return static_drive_7; 
    case 8: return static_drive_8; 
    case 9: return static_drive_9; 
    case 10: return static_drive_10; 
    case 11: return static_drive_11; 
    case 12: return static_drive_12; 
    case 13: return static_drive_13; 
    case 14: return static_drive_14;
    default: return NULL;
  }
}

/* Enable / Disable */

int v_drives_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("xrdrives", "Drives");
  if (!static_cnt)
  { i_printf (1, "v_drives_enable failed to create container"); v_drives_disable (self); return -1; }
  static_cnt->mainform_func = v_drives_cntform;
  static_cnt->sumform_func = v_drives_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_drives_enable failed to register container"); v_drives_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_drives_enable failed to load and apply container refresh config"); v_drives_disable (self); return -1; }

  /*
   * Triggers
   */
  i_triggerset *tset;

  /* SMART */
  tset = i_triggerset_create ("smart_state", "SMART State", "smart_state");
  i_triggerset_addtrg (self, tset, "warning", "Pre-Failure Warning", VALTYPE_STRING, TRGTYPE_EQUAL, 0, "threshold-exceeded", 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* Rebuilding */
  tset = i_triggerset_create ("rebuilding", "Drive Status", "rebuilding");
  i_triggerset_addtrg (self, tset, "rebuilding", "Rebuilding", VALTYPE_INTEGER, TRGTYPE_EQUAL, 1, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* Broken RAID Member */
  tset = i_triggerset_create ("brokenraidmember", "RAID Status", "brokenraidmember");
  i_triggerset_addtrg (self, tset, "broken", "Broken Member", VALTYPE_INTEGER, TRGTYPE_EQUAL, 1, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* 
   * Item and objects 
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_drives_enable failed to create item_list"); v_drives_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /*
   * Create drive objects 
   */
  int i;
  for (i=1; i<=14; i++)
  {
    char *name_str;
    char *desc_str;
    v_drives_item *driveitem;

    /* Set name/desc */
    asprintf (&name_str, "drive_%i", i);
    asprintf (&desc_str, "Drive Slot %i", i);
    
    /* Create table object */
    obj = i_object_create (name_str, desc_str);
    free (name_str);
    free (desc_str);
    obj->mainform_func = v_drives_objform;

    /* Register table object */
    i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

    /* Create item */
    driveitem = v_drives_item_create ();
    driveitem->obj = obj;
    obj->itemptr = driveitem;
    i_list_enqueue (static_cnt->item_list, driveitem);

    switch (i)
    {
      case 1: static_drive_1 = driveitem;
      case 2: static_drive_2 = driveitem;
      case 3: static_drive_3 = driveitem;
      case 4: static_drive_4 = driveitem;
      case 5: static_drive_5 = driveitem;
      case 6: static_drive_6 = driveitem;
      case 7: static_drive_7 = driveitem;
      case 8: static_drive_8 = driveitem;
      case 9: static_drive_9 = driveitem;
      case 10: static_drive_10 = driveitem;
      case 11: static_drive_11 = driveitem;
      case 12: static_drive_12 = driveitem;
      case 13: static_drive_13 = driveitem;
      case 14: static_drive_14 = driveitem;
    }

    /*
     * Create Metrics 
     */ 
  
    memset (&refconfig, 0, sizeof(i_entity_refresh_config));
    refconfig.refresh_method = REFMETHOD_EXTERNAL;
    refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
    refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

    driveitem->smart_state = i_metric_create ("smart_state", "SMART State", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(driveitem->smart_state));
    i_entity_refresh_config_apply (self, ENTITY(driveitem->smart_state), &refconfig);
    
    driveitem->smart_status = i_metric_create ("smart_status", "SMART Status", METRIC_INTEGER);
    i_entity_register (self, ENTITY(obj), ENTITY(driveitem->smart_status));
    i_entity_refresh_config_apply (self, ENTITY(driveitem->smart_status), &refconfig);

    driveitem->vendor = i_metric_create ("vendor", "Vendor ID", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(driveitem->vendor));
    i_entity_refresh_config_apply (self, ENTITY(driveitem->vendor), &refconfig);
    
    driveitem->firmware = i_metric_create ("firmware", "Firmware Version", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(driveitem->firmware));
    i_entity_refresh_config_apply (self, ENTITY(driveitem->firmware), &refconfig);
    
    driveitem->sectorcapacity = i_metric_create ("sectorcapacity", "Sector Capacity", METRIC_GAUGE);
    i_entity_register (self, ENTITY(obj), ENTITY(driveitem->sectorcapacity));
    i_entity_refresh_config_apply (self, ENTITY(driveitem->sectorcapacity), &refconfig);

    driveitem->capacity = i_metric_create ("capacity", "Capacity", METRIC_GAUGE);
    driveitem->capacity->alloc_unit = 512;
    driveitem->capacity->unit_str = strdup ("b");
    driveitem->capacity->kbase = 1024;
    driveitem->capacity->valstr_func = i_string_volume_metric;
    i_entity_register (self, ENTITY(obj), ENTITY(driveitem->capacity));
    i_entity_refresh_config_apply (self, ENTITY(driveitem->capacity), &refconfig);
    
    driveitem->badblockcount = i_metric_create ("badblockcount", "Bad Block Count", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(driveitem->badblockcount));
    i_entity_refresh_config_apply (self, ENTITY(driveitem->badblockcount), &refconfig);
    
    driveitem->remapcount = i_metric_create ("remapcount", "Drive Re-Map Count", METRIC_STRING);
    i_entity_register (self, ENTITY(obj), ENTITY(driveitem->remapcount));
    i_entity_refresh_config_apply (self, ENTITY(driveitem->remapcount), &refconfig);

    driveitem->online = i_metric_create ("online", "Online", METRIC_INTEGER);
    i_entity_register (self, ENTITY(obj), ENTITY(driveitem->online));
    i_entity_refresh_config_apply (self, ENTITY(driveitem->online), &refconfig);
    i_metric_enumstr_add (driveitem->online, 0, "No");
    i_metric_enumstr_add (driveitem->online, 1, "Yes");

    driveitem->arraymember = i_metric_create ("arraymember", "Array Member", METRIC_INTEGER);
    i_entity_register (self, ENTITY(obj), ENTITY(driveitem->arraymember));
    i_entity_refresh_config_apply (self, ENTITY(driveitem->arraymember), &refconfig);
    i_metric_enumstr_add (driveitem->arraymember, 0, "No");
    i_metric_enumstr_add (driveitem->arraymember, 1, "Yes");

    driveitem->rebuilding = i_metric_create ("rebuilding", "Rebuilding", METRIC_INTEGER);
    i_entity_register (self, ENTITY(obj), ENTITY(driveitem->rebuilding));
    i_entity_refresh_config_apply (self, ENTITY(driveitem->rebuilding), &refconfig);
    i_metric_enumstr_add (driveitem->rebuilding, 0, "No");
    i_metric_enumstr_add (driveitem->rebuilding, 1, "Yes");

    driveitem->brokenraidmember = i_metric_create ("brokenraidmember", "Broken RAID Member", METRIC_INTEGER);
    i_entity_register (self, ENTITY(obj), ENTITY(driveitem->brokenraidmember));
    i_entity_refresh_config_apply (self, ENTITY(driveitem->brokenraidmember), &refconfig);
    i_metric_enumstr_add (driveitem->brokenraidmember, 0, "No");
    i_metric_enumstr_add (driveitem->brokenraidmember, 1, "Yes");

    driveitem->diskcache_enabled = i_metric_create ("diskcache_enabled", "Drive Cache", METRIC_INTEGER);
    i_entity_register (self, ENTITY(obj), ENTITY(driveitem->diskcache_enabled));
    i_entity_refresh_config_apply (self, ENTITY(driveitem->diskcache_enabled), &refconfig);
    i_metric_enumstr_add (driveitem->diskcache_enabled, 0, "Disabled");
    i_metric_enumstr_add (driveitem->diskcache_enabled, 1, "Enabled");
    
    driveitem->arraynumber = i_metric_create ("arraynumber", "Array Number", METRIC_INTEGER);
    i_entity_register (self, ENTITY(obj), ENTITY(driveitem->arraynumber));
    i_entity_refresh_config_apply (self, ENTITY(driveitem->arraynumber), &refconfig);

    /* Evaluate apprules for all triggersets */
    i_triggerset_evalapprules_allsets (self, obj);

    /* Evaluate recrules for all metrics */
    l_record_eval_recrules_obj (self, obj);
  }

  return 0;
}

int v_drives_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

