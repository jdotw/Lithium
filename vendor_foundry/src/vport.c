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
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <lithium/snmp.h>

#include "vserv.h"
#include "vport.h"

/* vport - Virtual Port Sub-System */

static i_list *static_cnt_list = NULL;

/* Sub-System Enable / Disable */

int v_vport_enable (i_resource *self, v_vserv_item *vserv, char *index_oidstr)
{
  int num;
  char *name_str;
  char *desc_str;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  asprintf (&name_str, "%s_vport", vserv->obj->name_str);
  asprintf (&desc_str, "%s Virtual Ports", vserv->obj->desc_str);
  vserv->vport_cnt = i_container_create (name_str, desc_str);
  free (name_str);
  free (desc_str);
  if (!vserv->vport_cnt)
  { i_printf (1, "v_vport_enable failed to create container"); v_vport_disable (self, vserv); return -1; }
  vserv->vport_cnt->mainform_func = v_vport_cntform;
  vserv->vport_cnt->prio--;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(vserv->vport_cnt));
  if (num != 0)
  { i_printf (1, "v_vport_enable failed to register container"); v_vport_disable (self, vserv); return -1; }

  /* Normal Handling */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(vserv->vport_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_vport_enable failed to load and apply container refresh config"); v_vport_disable (self, vserv); return -1; }

  /* 
   * Items and Object Factory
   */

  /* Create item list */
  vserv->vport_cnt->item_list = i_list_create ();
  if (!vserv->vport_cnt->item_list)
  { i_printf (1, "v_vport_enable failed to create item_list"); v_vport_disable (self, vserv); return -1; }
  vserv->vport_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* Create the object factory */
  vserv->vport_objfact = l_snmp_objfact_create (self, "vport", "Real Servers");
  if (!vserv->vport_objfact)
  { i_printf (1, "v_vport_enable failed to call l_snmp_objfact_create to create the objfact"); v_vport_disable (self, vserv); return -1; }
  vserv->vport_objfact->dev = self->hierarchy->dev;
  vserv->vport_objfact->cnt = vserv->vport_cnt;
  asprintf (&vserv->vport_objfact->name_oid_str, "enterprises.1991.1.1.4.22.1.1.2.%s", index_oidstr);
  vserv->vport_objfact->fabfunc = v_vport_objfact_fab;
  vserv->vport_objfact->ctrlfunc = v_vport_objfact_ctrl;
  vserv->vport_objfact->cleanfunc = v_vport_objfact_clean;
  vserv->vport_objfact->passdata = strdup (index_oidstr);

  /* Start the object factory */
  num = l_snmp_objfact_start (self, vserv->vport_objfact);
  if (num != 0)
  { i_printf (1, "v_vport_enable failed to call l_snmp_objfact_start to start the object factory"); v_vport_disable (self, vserv); return -1; }

  /*
   * Misc
   */
  
  /* Enqueue into cnt list */
  if (!static_cnt_list)
  { static_cnt_list = i_list_create (); }
  i_list_enqueue (static_cnt_list, vserv->vport_cnt);

  return 0;
}

int v_vport_disable (i_resource *self, v_vserv_item *vserv)
{
  /* Disable the sub-system */
  int num;

  /* Remove from cnt list */
  if (static_cnt_list)
  { 
    num = i_list_search (static_cnt_list, vserv->vport_cnt);
    if (num == 0)
    { i_list_delete (static_cnt_list); }
    if (static_cnt_list->size < 1)
    { i_list_free (static_cnt_list); static_cnt_list = NULL; }
  }

  /* Terminate the object factory */
  if (vserv->vport_objfact)
  { l_snmp_objfact_stop (self, vserv->vport_objfact); vserv->vport_objfact = NULL; }

  /* Deregister container */
  if (vserv->vport_cnt)
  { i_entity_deregister (self, ENTITY(vserv->vport_cnt)); i_entity_free (ENTITY(vserv->vport_cnt)); vserv->vport_cnt = NULL; }


  return 0;
}

/* Container validation */

int v_vport_cnt_validate (i_container *cnt)
{
  /* Verifies that the specified container is infact a vport container.
   * This is used to validate objects passed to forms
   */
  
  if (static_cnt_list) return i_list_search (static_cnt_list, cnt);
  
  return -1;
}
