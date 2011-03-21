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

#include "osx_server.h"
#include "data.h"
#include "sysinfo.h"

/* Xraid System Information Sub-System */

static i_container *static_cnt = NULL;
static v_sysinfo_item *static_item = NULL;

/* Variable Retrieval */

i_container* v_sysinfo_cnt ()
{ return static_cnt; }

v_sysinfo_item* v_sysinfo_static_item ()
{ return static_item; }

/* Enable / Disable */

int v_sysinfo_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Create/Config Container */
  if (v_xserve_extras())
  {
    if (v_xserve_intel_extras())
    { static_cnt = i_container_create ("xsisysinfo", "System Information"); }
    else
    { static_cnt = i_container_create ("xssysinfo", "System Information"); }
  }
  else
  {
    static_cnt = i_container_create ("xsysinfo", "System Information");
  }
  if (!static_cnt)
  { i_printf (1, "v_sysinfo_enable failed to create container"); v_sysinfo_disable (self); return -1; }
  static_cnt->mainform_func = v_sysinfo_cntform;
  static_cnt->sumform_func = v_sysinfo_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_sysinfo_enable failed to register container"); v_sysinfo_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_sysinfo_enable failed to load and apply container refresh config"); v_sysinfo_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_sysinfo_enable failed to create item_list"); v_sysinfo_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create table object */
  obj = i_object_create ("master", "Master");
  obj->mainform_func = v_sysinfo_objform;

  /* Register table object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Create item */
  static_item = v_sysinfo_item_create ();
  static_item->obj = obj;
  obj->itemptr = static_item;

  /*
   * Create Metrics 
   */ 
  
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  static_item->computername = i_metric_create ("computername", "Computer Name", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->computername));
  i_entity_refresh_config_apply (self, ENTITY(static_item->computername), &refconfig);

  static_item->rendname = i_metric_create ("rendname", "Rendezvous Name", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->rendname));
  i_entity_refresh_config_apply (self, ENTITY(static_item->rendname), &refconfig);

  static_item->hostname = i_metric_create ("hostname", "Hostname", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->hostname));
  i_entity_refresh_config_apply (self, ENTITY(static_item->hostname), &refconfig);

  static_item->boottime = i_metric_create ("boottime", "Boot Time", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->boottime));
  i_entity_refresh_config_apply (self, ENTITY(static_item->boottime), &refconfig);

  static_item->version = i_metric_create ("version", "Version", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->version));
  i_entity_refresh_config_apply (self, ENTITY(static_item->version), &refconfig);

  static_item->serialvalid = i_metric_create ("serialvalid", "Serial Number Valid", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->serialvalid));
  i_entity_refresh_config_apply (self, ENTITY(static_item->serialvalid), &refconfig);

  static_item->serialuserlimit = i_metric_create ("serialuserlimit", "User Count Limit", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->serialuserlimit));
  i_entity_refresh_config_apply (self, ENTITY(static_item->serialuserlimit), &refconfig);

  if (v_xserve_extras())
  {
    /* Xserve Extras */

    /* CPU Count */
    static_item->cpu_count = i_metric_create ("cpu_count", "CPUs", METRIC_GAUGE);
    i_entity_register (self, ENTITY(obj), ENTITY(static_item->cpu_count));
    i_entity_refresh_config_apply (self, ENTITY(static_item->cpu_count), &refconfig);

    /* CPU Speed */
    static_item->cpu_speed = i_metric_create ("cpu_speed", "CPU Speed", METRIC_GAUGE);
    static_item->cpu_speed->unit_str = strdup ("Mhz");
    i_entity_register (self, ENTITY(obj), ENTITY(static_item->cpu_speed));
    i_entity_refresh_config_apply (self, ENTITY(static_item->cpu_speed), &refconfig);

    if (v_xserve_intel_extras())
    {
      /* CPU Type */
      static_item->cpu_type = i_metric_create ("cpu_type", "CPU Type", METRIC_STRING);
      i_entity_register (self, ENTITY(obj), ENTITY(static_item->cpu_type));
      i_entity_refresh_config_apply (self, ENTITY(static_item->cpu_type), &refconfig);
    }
    else
    {
      /* Boot ROM */
      static_item->bootrom = i_metric_create ("bootrom", "Boot ROM", METRIC_STRING);
      i_entity_register (self, ENTITY(obj), ENTITY(static_item->bootrom));
      i_entity_refresh_config_apply (self, ENTITY(static_item->bootrom), &refconfig);

      /* CPU L2Cache */
      static_item->cpu_l2cache = i_metric_create ("cpu_l2cache", "CPU Layer2 Cache", METRIC_GAUGE);
      static_item->cpu_l2cache->unit_str = strdup ("Kb");
      i_entity_register (self, ENTITY(obj), ENTITY(static_item->cpu_l2cache));
      i_entity_refresh_config_apply (self, ENTITY(static_item->cpu_l2cache), &refconfig);

      /* RAM  */
      static_item->ram_size = i_metric_create ("ram_size", "RAM", METRIC_GAUGE);
      static_item->ram_size->unit_str = strdup ("Mb");
      i_entity_register (self, ENTITY(obj), ENTITY(static_item->ram_size));
      i_entity_refresh_config_apply (self, ENTITY(static_item->ram_size), &refconfig);
    }
  }

  /*
   * Create Data Metrics 
   */
  v_data_item *dataitem = v_data_static_item();

  /* Standard refresh config */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* Sysinfo metric */
  dataitem->sysinfo = i_metric_create ("sysinfo", "System Info", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->sysinfo, 0, "Invalid");
  i_metric_enumstr_add (dataitem->sysinfo, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->sysinfo));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->sysinfo), &refconfig);
  dataitem->sysinfo->refresh_func = v_data_info_state_refresh;

  return 0;
}

int v_sysinfo_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

