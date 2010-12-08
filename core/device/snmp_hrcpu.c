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
#include "snmp_hrcpu.h"

/* snmp_hrcpu - SNMP Host Resources CPU Sub-System */

static int static_enabled = 0;                        /* 0 = disabled / 1 = enabled */
static i_container *static_cnt = NULL;                /* Cement Container */
static l_snmp_objfact *static_objfact = NULL;         /* Object Factory Data */

/* Variable Fetching */

i_container* i_snmp_hrcpu_cnt ()
{ return static_cnt; }

/* Sub-System Enable / Disable */

int l_snmp_hrcpu_enable (i_resource *self)
{
  int num;
  i_triggerset *tset;
  static i_entity_refresh_config defrefconfig;

  if (static_enabled == 1)
  { i_printf (1, "l_snmp_hrcpu_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("hrcpu", "CPU Cores");
  if (!static_cnt)
  { i_printf (1, "l_snmp_hrcpu_enable failed to create container"); l_snmp_hrcpu_disable (self); return -1; }
  static_cnt->mainform_func = l_snmp_hrcpu_cntform;
  static_cnt->sumform_func = l_snmp_hrcpu_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "l_snmp_hrcpu_enable failed to register container"); l_snmp_hrcpu_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "l_snmp_hrcpu_enable failed to load and apply container refresh config"); l_snmp_hrcpu_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "l_snmp_hrcpu_enable failed to create item_list"); l_snmp_hrcpu_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* 
   * Trigger Sets
   */

  /* Load */
  tset = i_triggerset_create ("load_pc", "Load Percent", "load_pc");
  tset->default_duration = 10 * 60;   /* Ten minutes duration hold-down */
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 70, NULL, 85, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 85, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* Create the object factory */
  static_objfact = l_snmp_objfact_create (self, static_cnt->name_str, static_cnt->desc_str);
  if (!static_objfact)
  {
    i_printf (1, "l_snmp_hrcpu_enable failed to call l_snmp_objfact_create to create the objfact"); 
    l_snmp_hrcpu_disable (self); 
    return -1;
  }
  static_objfact->dev = self->hierarchy->dev;
  static_objfact->cnt = static_cnt;
  static_objfact->name_oid_str = strdup (".1.3.6.1.2.1.25.3.3.1.2");
  static_objfact->fabfunc = l_snmp_hrcpu_objfact_fab;
  static_objfact->ctrlfunc = l_snmp_hrcpu_objfact_ctrl;
  static_objfact->cleanfunc = l_snmp_hrcpu_objfact_clean;
  static_objfact->namesource = OBJFACT_NAME_OID;

  /* Start the object factory */
  num = l_snmp_objfact_start (self, static_objfact);
  if (num != 0)
  { 
    i_printf (1, "l_snmp_hrcpu_enable failed to call l_snmp_objfact_start to start the object factory"); 
    l_snmp_hrcpu_disable (self); 
    return -1; 
  }

  return 0;  
}

int l_snmp_hrcpu_disable (i_resource *self)
{
  /* Disable the sub-system */

  if (static_enabled == 0)
  { i_printf (1, "l_snmp_hrcpu_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  /* Terminate the object factory */
  if (static_objfact)
  { l_snmp_objfact_stop (self, static_objfact); static_objfact = NULL; }

  /* Deregister container */
  if (static_cnt) 
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

