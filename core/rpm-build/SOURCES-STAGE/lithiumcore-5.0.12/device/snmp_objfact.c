#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/navtree.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/object.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>

#include "snmp.h"

static i_container *static_cnt = NULL;

/*
 * SNMP Object Factory 
 *
 * The SNMP Object Factory creates objects by walking
 * a given OID that will provide the unique name for 
 * the objects. As each PDU is received, an object is created 
 * and the fabrication function is called to fill out the object 
 * with the system-specific metrics etc.
 *
 * Once the walk has been successfully completed, the object
 * (and hence all metrics, triggers, etc) will be registered.
 *
 * Periodically, the object factory will walk the OID again
 * and add any new objects as well as removing obsolete ones.
 */

i_container* l_snmp_objfact_cnt ()
{ return static_cnt; }

/* Sub-System Initialisation */

int l_snmp_objfact_init (i_resource *self, i_device *dev)
{
  int num; 
  i_entity_refresh_config refconfig;

  /* Check state */
  if (static_cnt) return 0;

  /* Create/Config Container */
  static_cnt = i_container_create ("snmp_objfact", "SNMP Object Factory");
  if (!static_cnt)
  { i_printf (1, "l_snmp_objfact_init failed to create container"); return -1; }
  static_cnt->prio = 0;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;
  static_cnt->hidden = 1;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  {
    i_printf (1, "l_snmp_objfact_init failed to register container");
    i_entity_free (ENTITY(static_cnt));
    static_cnt = NULL;
    return -1;
  }

  /* Load/Apply refresh config */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_NONE;
  refconfig.refresh_int_sec = 0;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_apply (self, ENTITY(static_cnt), &refconfig);
  if (num != 0)
  {
    i_printf (1, "l_snmp_objfact_init failed to load and apply container refresh config");
    i_entity_deregister (self, ENTITY(static_cnt));
    i_entity_free (ENTITY(static_cnt));
    static_cnt = NULL;
    return -1;
  }

  return 0;
}

/* Factory Struct Manipulation */

l_snmp_objfact* l_snmp_objfact_create (i_resource *self, char *name_str, char *desc_str)
{
  l_snmp_objfact *fact;

  fact = (l_snmp_objfact *) malloc (sizeof(l_snmp_objfact));
  if (!fact)
  { i_printf (1, "l_snmp_objfact_create failed to malloc l_snmp_objfact struct"); return NULL; }
  memset (fact, 0, sizeof(l_snmp_objfact));
  fact->obj_list = i_list_create ();
  if (!fact->obj_list) 
  { i_printf (1, "l_snmp_objfact_create failed to create obj_list list"); l_snmp_objfact_free (fact); return NULL; }
  fact->unreg_list = i_list_create ();
  if (!fact->unreg_list) 
  { i_printf (1, "l_snmp_objfact_create failed to create unreg_list list"); l_snmp_objfact_free (fact); return NULL; }
  fact->refresh_int_sec = OBJFACT_DEF_REFRESH_INT;
  fact->retry_int_sec = OBJFACT_DEF_RETRY_INT; 
  fact->name_oid_len = MAX_OID_LEN;

  /* Create a cement object for the factory */
  fact->obj = i_object_create (name_str, desc_str);
  if (!fact->obj)
  { i_printf (1, "l_snmp_objfact_create failed to create fact->obj"); l_snmp_objfact_free (fact); return NULL; }
  fact->obj->refresh_func = l_snmp_objfact_refresh;
  fact->obj->refresh_data = fact;

  return fact;
}

void l_snmp_objfact_free (void *factptr)
{
  l_snmp_objfact *fact = factptr;

  if (!fact) return;

  if (fact->obj) i_entity_free (ENTITY(fact->obj));
  if (fact->name_oid_str) free (fact->name_oid_str);
  if (fact->session) l_snmp_session_close (fact->session); 
  if (fact->obj_list) i_list_free (fact->obj_list); 
  if (fact->unreg_list) i_list_free (fact->unreg_list); 

  free (fact);
}

/* Factory Manipulation */

int l_snmp_objfact_start (i_resource *self, l_snmp_objfact *fact)
{
  /* Start an object factory
   *
   * The factories object is registerd and a TIMER based refresh
   * configuration applied. 
   */

  int num;

  /* Check marking */
  if (static_cnt->dev->mark == ENTSTATE_OUTOFSERVICE) return 0;

  /* Register factories object */
  num = i_entity_register (self, ENTITY(static_cnt), ENTITY(fact->obj));
  if (num != 0)
  { i_printf (1, "l_snmp_objfact_start failed to register the factory's object"); l_snmp_objfact_stop (self, fact); return -1; }

  /* Apply 'normal' refresh config */
  num = l_snmp_objfact_normalrefcfg (self, fact);
  if (num != 0)
  { i_printf (1, "l_snmp_objfact_start failed to apply normal refresh config"); l_snmp_objfact_stop (self, fact); return -1; }

  /* Parse the name OID */
  num = l_snmp_parse_oidstr (fact->name_oid_str, fact->name_oid, &fact->name_oid_len);
  if (num != 0)
  { i_printf (1, "l_snmp_objfact_start failed to parse name_oid_str (%s)", fact->name_oid_str); l_snmp_objfact_stop (self, fact); return -1; }

  /* Set refresh flag */
  fact->first_refresh = 1;

  /* Call a refresh on the factory object */
  num = i_entity_refresh (self, ENTITY(fact->obj), REFFLAG_AUTO, NULL, NULL);
  if (num == -1)
  { 
    i_printf (1, "l_snmp_objfact_start failed to call the initial refresh of the object factory");
    l_snmp_objfact_stop (self, fact); 
    return -1; 
  }

  /* Set state */
  fact->started = 1;

  return 0;
}

int l_snmp_objfact_stop (i_resource *self, l_snmp_objfact *fact)
{
  /* Terminate any active refresh operation and 
   * then remove the objects refresh system and
   * finally de-register the object.
   */
  
  i_entity_refresh_config refconfig;

  /* Cancel any refresh op */
  if (fact->obj && (fact->obj->refresh_state & REFSTATE_ALL) == REFSTATE_BUSY) 
  { i_entity_refresh_terminate (ENTITY(fact->obj)); }

  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_NONE;
  refconfig.refresh_int_sec = fact->refresh_int_sec;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  i_entity_refresh_config_apply (self, ENTITY(fact->obj), &refconfig);
  
  /* De-register */
  i_entity_deregister (self, ENTITY(fact->obj));

  return 0;
}

/* Refresh configuration manipulation 
 *
 * During normal operation 'normalrefcfg' is called to
 * apply a TIMER based refresh configuration for the factory's
 * object using the fact->refresh_int_sec interval
 *
 * If an error occurs during a refresh operation, 'retryrefcfg'
 * is called to apply a TIMER based refresh configuration for
 * the factory's object using the fact->retry_int_sec interval
 */

int l_snmp_objfact_normalrefcfg (i_resource *self, l_snmp_objfact *fact)
{
  i_entity_refresh_config refconfig;

  i_printf (2, "l_snmp_objfact_normalrefcfg setting objfact %s refresh interval to %li",
    fact->obj->name_str, fact->refresh_int_sec);

  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_TIMER|REFFLAG_MANUAL;
  refconfig.refresh_int_sec = fact->refresh_int_sec;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  i_entity_refresh_config_apply (self, ENTITY(fact->obj), &refconfig);

  return 0;
}

int l_snmp_objfact_retryrefcfg (i_resource *self, l_snmp_objfact *fact)
{
  i_entity_refresh_config refconfig;

  i_printf (2, "l_snmp_objfact_retryrefcfg setting objfact %s refresh interval to %li",
    fact->obj->name_str, fact->retry_int_sec);

  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_TIMER|REFFLAG_MANUAL;
  refconfig.refresh_int_sec = fact->retry_int_sec;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  i_entity_refresh_config_apply (self, ENTITY(fact->obj), &refconfig);

  return 0;
}
