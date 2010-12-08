#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/callback.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/name.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/interface.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>

#include "avail.h"
#include "snmp.h"
#include "triggerset.h"
#include "record.h"
#include "icmp.h"

/* icmp - ICMP Sub-System */

static int static_enabled = 0;                        /* 0 = disabled / 1 = enabled */
static l_snmp_objfact *static_objfact;
static i_container *static_cnt = NULL;                /* Cement Container */
static i_object *static_availobj = NULL;
static i_object *static_masterobj = NULL;

/* Variable Fetching */

i_container* l_icmp_cnt ()
{ return static_cnt; }

i_object* l_icmp_availobj ()
{ return static_availobj; }

i_object* l_icmp_masterobj ()
{ return static_masterobj; }

/* Sub-System Enable / Disable */

int l_icmp_enable (i_resource *self, unsigned short flags)
{
  int num;
  char *str;
  l_icmp_item *icmp;
  static i_entity_refresh_config defrefconfig;

  /* Check profile and icmp flag */
  if (self->hierarchy->dev->profile_str && strstr(self->hierarchy->dev->profile_str, "noicmp")) return 0; 
  if (!self->hierarchy->dev->icmp && strcmp(self->hierarchy->dev->vendor_str, "icmp") != 0) return 0;
  
  if (static_enabled == 1)
  { return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("icmp", "ICMP");
  if (!static_cnt)
  { i_printf (1, "l_icmp_enable failed to create container"); l_icmp_disable (self); return -1; }
  static_cnt->mainform_func = l_icmp_cntform;
  
  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "l_icmp_enable failed to register container"); l_icmp_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_TIMER;
  defrefconfig.refresh_int_sec = 5;
//  defrefconfig.refresh_int_sec = (self->hierarchy->dev->refresh_interval / 5);
//  i_printf (1, "v_icmp_enable container defrefconfig is srt to %i seconds", defrefconfig.refresh_int_sec);
//  if (defrefconfig.refresh_int_sec < 5)
//  { defrefconfig.refresh_int_sec = 5; }
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "l_icmp_enable failed to load and apply container refresh config"); l_icmp_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "l_icmp_enable failed to create item_list"); l_icmp_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* 
   * Create the master object
   */

  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  
  /* Create object */
  str = strdup (self->hierarchy->dev->ip_str);
  i_name_parse (str);
  static_masterobj = i_object_create (str, self->hierarchy->dev->ip_str);
  free (str);
  if (!static_masterobj)
  { i_printf (1, "l_icmp_enable failed to create static_masterobj"); l_icmp_disable (self); return -1; }

  /* Register masterobj */
  num = i_entity_register (self, ENTITY(static_cnt), ENTITY(static_masterobj));
  if (num != 0)
  { i_printf (1, "l_icmp_enable failed to register object"); l_icmp_disable (self); return -1; }

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_masterobj), &defrefconfig);
  if (num != 0)
  { i_printf (1, "l_icmp_enable failed to load and apply object %s refresh config", static_masterobj->name_str); return -1; }

  /* Create icmp struct */
  icmp = l_icmp_item_create ();
  if (!icmp)
  { i_printf (1, "l_icmp_enable failed to create ICMP item for object %s", static_masterobj->name_str); return -1; }
  icmp->obj = static_masterobj;
  static_masterobj->itemptr = icmp;

  /* Set avail_flag to record */
  icmp->avail_flag = 1;

  /* Create metrics */
  l_icmp_objfact_createmets (self, static_masterobj, icmp);

  /* Enqueue the icmp item */
  num = i_list_enqueue (static_cnt->item_list, icmp);
  if (num != 0)
  { i_printf (1, "l_icmp_objfact_fab failed to enqueue ICMP item for object %s", static_masterobj->name_str); l_icmp_item_free (icmp); return -1; }

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_masterobj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_masterobj); 
  
  /* Enable SNMP Object factory if specified */
  if (flags & ICMPFLAG_SNMPOBJFACT)
  { 
    /* 
     * Create the object factory 
     */

    static_objfact = l_snmp_objfact_create (self, static_cnt->name_str, static_cnt->desc_str);
    if (!static_objfact)
    {
      i_printf (1, "l_icmp_enable failed to call l_snmp_objfact_create to create the objfact"); 
      l_icmp_disable (self); 
      return -1;
    }
    static_objfact->dev = self->hierarchy->dev;
    static_objfact->cnt = static_cnt;
    static_objfact->name_oid_str = strdup (".1.3.6.1.2.1.4.20.1.1");
    static_objfact->fabfunc = l_icmp_objfact_fab;
    static_objfact->ctrlfunc = l_icmp_objfact_ctrl;
    static_objfact->cleanfunc = l_icmp_objfact_clean;

    /* Start the object factory */
    num = l_snmp_objfact_start (self, static_objfact);
    if (num != 0)
    { 
      i_printf (1, "l_icmp_enable failed to call l_snmp_objfact_start to start the object factory"); 
      l_icmp_disable (self); 
      return -1; 
    }
  }

  /* Add Availability Object */
  static_availobj = l_avail_object_add (self, "icmp", "ICMP");
  if (!static_availobj)
  { i_printf (1, "l_icmp_enable warning, failed to add availability object"); }

  return 0;  
}

int l_icmp_disable (i_resource *self)
{
  /* Disable the sub-system */

  if (static_enabled == 0)
  { i_printf (1, "l_icmp_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  /* Remove masterobj */
  if (static_masterobj)
  { 
    if (static_masterobj->itemptr)
    {
      l_icmp_item *icmp = static_masterobj->itemptr;
      l_icmp_item_free (icmp);
    }
    i_entity_deregister (self, ENTITY(static_masterobj));
    i_entity_free (ENTITY(static_masterobj));
    static_masterobj = NULL;
  }

  /* Terminate the object factory */
  if (static_objfact)
  { l_snmp_objfact_stop (self, static_objfact); static_objfact = NULL; }

  /* Deregister container */
  if (static_cnt) 
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  /* Remove Availability Object */
  if (static_availobj)
  { l_avail_object_remove (self, static_availobj); static_availobj = NULL; }

  return 0;
}

