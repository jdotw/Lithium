#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/interface.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>

#include "snmp.h"
#include "snmp_storage.h"

/* snmp_storage - SNMP Storage Resources Sub-System */

static int static_enabled = 0;                        /* 0 = disabled / 1 = enabled */
static i_container *static_cnt = NULL;                /* Cement Container */
static l_snmp_objfact *static_objfact = NULL;         /* Object Factory Data */
static int static_monitor_memory = 0;                 /* 0=Ignore memory objects */

/* Variable Fetching */

i_container* l_snmp_storage_cnt ()
{ return static_cnt; }

int l_snmp_storage_monitor_memory ()
{ return static_monitor_memory; }

void l_snmp_storage_monitor_memory_set (int value)
{ static_monitor_memory = value; }

/* Sub-System Enable / Disable */

int l_snmp_storage_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  if (static_enabled == 1)
  { i_printf (1, "l_snmp_storage_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("storage", "Storage Resources");
  if (!static_cnt)
  { i_printf (1, "l_snmp_storage_enable failed to create container"); l_snmp_storage_disable (self); return -1; }
  static_cnt->mainform_func = l_snmp_storage_cntform;
  static_cnt->sumform_func = l_snmp_storage_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "l_snmp_storage_enable failed to register container"); l_snmp_storage_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "l_snmp_storage_enable failed to load and apply container refresh config"); l_snmp_storage_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "l_snmp_storage_enable failed to create item_list"); l_snmp_storage_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* Create the object factory */
  static_objfact = l_snmp_objfact_create (self, static_cnt->name_str, static_cnt->desc_str);
  if (!static_objfact)
  {
    i_printf (1, "l_snmp_storage_enable failed to call l_snmp_objfact_create to create the objfact"); 
    l_snmp_storage_disable (self); 
    return -1;
  }
  static_objfact->dev = self->hierarchy->dev;
  static_objfact->cnt = static_cnt;
  if (l_snmp_xsnmp_enabled())
  { static_objfact->name_oid_str = strdup (".1.3.6.1.4.1.20038.2.1.4.1.1.3"); }
  else
  { static_objfact->name_oid_str = strdup (".1.3.6.1.2.1.25.2.3.1.3"); }
  static_objfact->fabfunc = l_snmp_storage_objfact_fab;
  static_objfact->ctrlfunc = l_snmp_storage_objfact_ctrl;
  static_objfact->cleanfunc = l_snmp_storage_objfact_clean;

  /* Start the object factory */
  num = l_snmp_objfact_start (self, static_objfact);
  if (num != 0)
  { 
    i_printf (1, "l_snmp_storage_enable failed to call l_snmp_objfact_start to start the object factory"); 
    l_snmp_storage_disable (self); 
    return -1; 
  }

  return 0;  
}

int l_snmp_storage_disable (i_resource *self)
{
  /* Disable the sub-system */

  if (static_enabled == 0)
  { i_printf (1, "l_snmp_storage_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  /* Terminate the object factory */
  if (static_objfact)
  { l_snmp_objfact_stop (self, static_objfact); static_objfact = NULL; }

  /* Deregister container */
  if (static_cnt) 
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

