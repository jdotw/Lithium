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
#include <induction/triggerset.h>

#include "snmp.h"
#include "xsnmp_raidset.h"

/* xsnmp_raidset - xSNMP RAID Set Monitoring */

static int static_enabled = 0;                        /* 0 = disabled / 1 = enabled */
static i_container *static_cnt = NULL;                /* Cement Container */
static l_snmp_objfact *static_objfact = NULL;         /* Object Factory Data */

/* Variable Fetching */

i_container* i_xsnmp_raidset_cnt ()
{ return static_cnt; }

/* Sub-System Enable / Disable */

int l_xsnmp_raidset_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  if (static_enabled == 1)
  { i_printf (1, "l_xsnmp_raidset_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("xsnmp_raidsets", "RAID Sets");
  if (!static_cnt)
  { i_printf (1, "l_xsnmp_raidset_enable failed to create container"); return -1; }

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "l_xsnmp_raidset_enable failed to register container"); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "l_xsnmp_raidset_enable failed to load and apply container refresh config"); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "l_xsnmp_raidset_enable failed to create item_list"); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* 
   * Trigger Sets
   */

  /* Create the object factory */
  static_objfact = l_snmp_objfact_create (self, static_cnt->name_str, static_cnt->desc_str);
  if (!static_objfact)
  {
    i_printf (1, "l_xsnmp_raidset_enable failed to call l_snmp_objfact_create to create the objfact"); 
    return -1;
  }
  static_objfact->dev = self->hierarchy->dev;
  static_objfact->cnt = static_cnt;
  static_objfact->name_oid_str = strdup (".1.3.6.1.4.1.20038.2.1.6.2.1.2");
  static_objfact->fabfunc = l_xsnmp_raidset_objfact_fab;
  static_objfact->ctrlfunc = l_xsnmp_raidset_objfact_ctrl;
  static_objfact->cleanfunc = l_xsnmp_raidset_objfact_clean;

  /* Start the object factory */
  num = l_snmp_objfact_start (self, static_objfact);
  if (num != 0)
  { 
    i_printf (1, "l_xsnmp_raidset_enable failed to call l_snmp_objfact_start to start the object factory"); 
    return -1; 
  }

  return 0;  
}

l_xsnmp_raidset_item* l_xsnmp_raidset_item_create ()
{
  l_xsnmp_raidset_item *item;

  item = (l_xsnmp_raidset_item *) malloc (sizeof(l_xsnmp_raidset_item));
  if (!item)
  { i_printf (1, "l_xsnmp_raidset_item_create failed to malloc l_xsnmp_raidset_item struct"); return NULL; }
  memset (item, 0, sizeof(l_xsnmp_raidset_item));

  return item;
}

void l_xsnmp_raidset_item_free (void *itemptr)
{
  l_xsnmp_raidset_item *item = itemptr;

  if (item) return;

  free (item);
}

