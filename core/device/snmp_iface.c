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
#include <induction/value.h>
#include <induction/interface.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>

#include "snmp.h"
#include "snmp_iface.h"
#include "navtree.h"

/* snmpiface - SNMP Interface Sub-System */

static int static_enabled = 0;                        /* 0 = disabled / 1 = enabled */
static i_container *static_cnt = NULL;                /* Cement Container */
static l_snmp_objfact *static_objfact = NULL;         /* Object Factory Data */

/* Variable Fetching */

i_container* l_snmp_iface_cnt ()
{ return static_cnt; }

l_snmp_objfact* l_snmp_iface_objfact ()
{ return static_objfact; }

/* Sub-System Enable / Disable */

int l_snmp_iface_enable (i_resource *self)
{
  int num;
  i_triggerset *tset;
  static i_entity_refresh_config defrefconfig;

  if (static_enabled == 1)
  { i_printf (1, "l_snmp_iface_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("iface", "Network Interfaces");
  if (!static_cnt)
  { i_printf (1, "l_snmp_iface_enable failed to create container"); l_snmp_iface_disable (self); return -1; }
  static_cnt->mainform_func = l_snmp_iface_cntform;
  static_cnt->sumform_func = l_snmp_iface_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "l_snmp_iface_enable failed to register container"); l_snmp_iface_disable (self); return -1; }

  /* Normal Handling */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "l_snmp_iface_enable failed to load and apply container refresh config"); l_snmp_iface_disable (self); return -1; }

  /* Add containers refresh callback */
  i_entity_refreshcb_add (ENTITY(static_cnt), l_snmp_iface_cnt_refcb, NULL);
  
  /* 
   * Trigger Set Creation
   */

  /* Operational State */
  tset = i_triggerset_create ("opstate", "Operational State", "opstate");
  i_triggerset_addtrg (self, tset, "down", "Down", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_FATAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "testing", "Testing", VALTYPE_INTEGER, TRGTYPE_EQUAL, 3, NULL, 0, NULL, 0, ENTSTATE_FATAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "unknown", "Unknown", VALTYPE_INTEGER, TRGTYPE_EQUAL, 4, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "dormant", "Dormant", VALTYPE_INTEGER, TRGTYPE_EQUAL, 5, NULL, 0, NULL, 0, ENTSTATE_NORMAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "notpresent", "Not Present", VALTYPE_INTEGER, TRGTYPE_EQUAL, 6, NULL, 0, NULL, 0, ENTSTATE_FATAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "lowerlayerdown", "Lower Layer Down", VALTYPE_INTEGER, TRGTYPE_EQUAL, 7, NULL, 0, NULL, 0, ENTSTATE_FATAL, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* Errors per second */
  tset = i_triggerset_create ("eps_in", "Input Errors Per Second", "eps_in");
  i_triggerset_addtrg (self, tset, "errors", "Errors", VALTYPE_FLOAT, TRGTYPE_GT, 10, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);  
  i_triggerset_assign (self, static_cnt, tset);
  tset = i_triggerset_create ("eps_out", "Output Errors Per Second", "eps_out");
  i_triggerset_addtrg (self, tset, "errors", "Errors", VALTYPE_FLOAT, TRGTYPE_GT, 10, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  /* Discards per second */
  tset = i_triggerset_create ("dps_in", "Input Discards Per Second", "dps_in");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 10, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);  
  i_triggerset_assign (self, static_cnt, tset);
  tset = i_triggerset_create ("dps_out", "Output Discards Per Second", "dps_out");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 10, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* Utilisation */
  tset = i_triggerset_create ("utilpc_in", "Input Utilisation", "utilpc_in");
  tset->default_duration = 15 * 60;   /* Fifteen minute hold-down */
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 75, NULL, 90, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 90, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  tset = i_triggerset_create ("utilpc_out", "Output Utilisation", "utilpc_out");
  tset->default_duration = 15 * 60;   /* Fifteen minute hold-down */
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 75, NULL, 90, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 90, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* 
   * Items and Object Factory
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "l_snmp_iface_enable failed to create item_list"); l_snmp_iface_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* Create the object factory */
  static_objfact = l_snmp_objfact_create (self, "iface", "Network Interface");
  if (!static_objfact)
  {
    i_printf (1, "l_snmp_iface_enable failed to call l_snmp_objfact_create to create the objfact"); 
    l_snmp_iface_disable (self); 
    return -1;
  }
  static_objfact->dev = self->hierarchy->dev;
  static_objfact->cnt = static_cnt;
  static_objfact->name_oid_str = strdup (".1.3.6.1.2.1.2.2.1.2");
  static_objfact->fabfunc = l_snmp_iface_objfact_fab;
  static_objfact->ctrlfunc = l_snmp_iface_objfact_ctrl;
  static_objfact->cleanfunc = l_snmp_iface_objfact_clean;

  /* Start the object factory */
  num = l_snmp_objfact_start (self, static_objfact);
  if (num != 0)
  { 
    i_printf (1, "l_snmp_iface_enable failed to call l_snmp_objfact_start to start the object factory"); 
    l_snmp_iface_disable (self); 
    return -1; 
  }

  return 0;  
}

int l_snmp_iface_disable (i_resource *self)
{
  /* Disable the sub-system */

  if (static_enabled == 0)
  { i_printf (1, "l_snmp_iface_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  /* Terminate the object factory */
  if (static_objfact)
  { l_snmp_objfact_stop (self, static_objfact); static_objfact = NULL; }

  /* Deregister container */
  if (static_cnt) 
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}


