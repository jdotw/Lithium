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
#include <induction/postgresql.h>

#include "navtree.h"
#include "snmp.h"
#include "snmp_swrun.h"
#include "procpro.h"

/* snmp_swrun - SNMP Running Software Sub-System */

static int static_enabled = 0;                        /* 0 = disabled / 1 = enabled */
static i_container *static_cnt = NULL;                /* Cement Container */
static l_snmp_objfact *static_objfact = NULL;         /* Object Factory Data */

/* Variable Fetching */

i_container* l_snmp_swrun_cnt ()
{ return static_cnt; }

/* Sub-System Enable / Disable */

int l_snmp_swrun_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  if (!self->hierarchy->dev->swrun) return 0;

  if (static_enabled == 1)
  { i_printf (1, "l_snmp_swrun_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("swrun", "Running Software");
  if (!static_cnt)
  { i_printf (1, "l_snmp_swrun_enable failed to create container"); l_snmp_swrun_disable (self); return -1; }
  static_cnt->mainform_func = l_snmp_swrun_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;
  static_cnt->hidden = 1;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "l_snmp_swrun_enable failed to register container"); l_snmp_swrun_disable (self); return -1; }
  i_entity_refreshcb_add (ENTITY(static_cnt), l_snmp_swrun_cnt_refcb, NULL);

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_EXTERNAL;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "l_snmp_swrun_enable failed to load and apply container refresh config"); l_snmp_swrun_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "l_snmp_swrun_enable failed to create item_list"); l_snmp_swrun_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;
  static_cnt->navtree_func = l_navtree_func;

  /* Create the object factory */
  static_objfact = l_snmp_objfact_create (self, "swrun", "Running Software");
  if (!static_objfact)
  {
    i_printf (1, "l_snmp_swrun_enable failed to call l_snmp_objfact_create to create the objfact"); 
    l_snmp_swrun_disable (self); 
    return -1;
  }
  static_objfact->dev = self->hierarchy->dev;
  static_objfact->cnt = static_cnt;
  static_objfact->name_oid_str = strdup (".1.3.6.1.2.1.25.4.2.1.1");
  static_objfact->fabfunc = l_snmp_swrun_objfact_fab;
  static_objfact->ctrlfunc = l_snmp_swrun_objfact_ctrl;
  static_objfact->cleanfunc = l_snmp_swrun_objfact_clean;
  static_objfact->refresh_int_sec = self->hierarchy->dev->refresh_interval;
  static_objfact->retry_int_sec = self->hierarchy->dev->refresh_interval;

  /* Start the object factory */
  num = l_snmp_objfact_start (self, static_objfact);
  if (num != 0)
  { 
    i_printf (1, "l_snmp_swrun_enable failed to call l_snmp_objfact_start to start the object factory"); 
    l_snmp_swrun_disable (self); 
    return -1; 
  }

  return 0;  
}

int l_snmp_swrun_disable (i_resource *self)
{
  /* Disable the sub-system */

  if (static_enabled == 0)
  { i_printf (1, "l_snmp_swrun_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  /* Terminate the object factory */
  if (static_objfact)
  { l_snmp_objfact_stop (self, static_objfact); static_objfact = NULL; }

  /* Deregister container */
  if (static_cnt) 
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

/* swrun Item Struct Manipulation */

l_snmp_swrun* l_snmp_swrun_create ()
{
  l_snmp_swrun *swrun;

  swrun = (l_snmp_swrun *) malloc (sizeof(l_snmp_swrun));
  if (!swrun)
  { i_printf (1, "l_snmp_swrun_create failed to malloc l_snmp_swrun struct"); return NULL; }
  memset (swrun, 0, sizeof(l_snmp_swrun));

  return swrun;
}

void l_snmp_swrun_free (void *swrunptr)
{
  l_snmp_swrun *swrun = swrunptr;

  if (!swrun) return;

  free (swrun);
}

/* Container Refresh Callback */

int l_snmp_swrun_cnt_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when the swrun container has been refreshed.
   *
   * At this point all process data should be populated and a procpro refreh
   * can happen
   *
   * Always return 0
   */

  if (ent->refresh_result == REFRESULT_OK)
  {
    i_container *procpro_cnt = l_procpro_cnt ();
    if (procpro_cnt)
    {
      i_entity_refresh (self, ENTITY(procpro_cnt), REFFLAG_AUTO, NULL, NULL);
    }
  }
  else
  {
    i_printf(0, "l_snmp_swrun_cnt_refcb warning, process profiles not being updated because the process list was not refreshed successfully");
  }

  if (ent->refresh_forcedterm == 1 && static_objfact->refresh_int_sec < REFDEFAULT_MAXBACKOFF)
  {
    /* Back off the swrun objfact refresh time due to forced termination  */
    static_objfact->refresh_int_sec += 30;
    static_objfact->retry_int_sec = static_objfact->refresh_int_sec;
    l_snmp_objfact_normalrefcfg(self, static_objfact);
    i_printf(0, "l_snmp_swrun_cnt_refcb increased process list refresh interval to %i seconds due to slow response time", static_objfact->refresh_int_sec);
  }

  return 0;
}

