#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/navtree.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/value.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "device/snmp.h"

#include "rserv.h"
#include "rport.h"

/* rport - Real Port Sub-System */

static i_list *static_cnt_list = NULL;

/* Sub-System Enable / Disable */

int v_rport_enable (i_resource *self, v_rserv_item *rserv, char *index_oidstr)
{
  int num;
  char *name_str;
  char *desc_str;
  i_triggerset *tset;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  asprintf (&name_str, "%s_rport", rserv->obj->name_str);
  asprintf (&desc_str, "%s Real Ports", rserv->obj->desc_str);
  rserv->rport_cnt = i_container_create (name_str, desc_str);
  free (name_str);
  free (desc_str);
  if (!rserv->rport_cnt)
  { i_printf (1, "v_rport_enable failed to create container"); v_rport_disable (self, rserv); return -1; }
  rserv->rport_cnt->mainform_func = v_rport_cntform;
//  rserv->rport_cnt->sumform_func = v_rport_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(rserv->rport_cnt));
  if (num != 0)
  { i_printf (1, "v_rport_enable failed to register container"); v_rport_disable (self, rserv); return -1; }

  /* Normal Handling */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(rserv->rport_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_rport_enable failed to load and apply container refresh config"); v_rport_disable (self, rserv); return -1; }

  /* 
   * Trigger sets 
   */

  tset = i_triggerset_create ("opstate", "Operational State", "opstate");  
  i_triggerset_addtrg (self, tset, "notconnected", "Not Connected", VALTYPE_INTEGER, TRGTYPE_EQUAL, 1, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);  
  i_triggerset_addtrg (self, tset, "failed", "Failed", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "testing", "Testing", VALTYPE_INTEGER, TRGTYPE_EQUAL, 3, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "suspect", "Suspect", VALTYPE_INTEGER, TRGTYPE_EQUAL, 4, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "shutdown", "Shutdown", VALTYPE_INTEGER, TRGTYPE_EQUAL, 5, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, rserv->rport_cnt, tset);

  tset = i_triggerset_create ("reassignps", "Reassigns Per Second", "reassignps");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 10, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, rserv->rport_cnt, tset);

  /* 
   * Items and Object Factory
   */

  /* Create item list */
  rserv->rport_cnt->item_list = i_list_create ();
  if (!rserv->rport_cnt->item_list)
  { i_printf (1, "v_rport_enable failed to create item_list"); v_rport_disable (self, rserv); return -1; }
  rserv->rport_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* Create the object factory */
  rserv->rport_objfact = l_snmp_objfact_create (self, "rport", "Real Servers");
  if (!rserv->rport_objfact)
  { i_printf (1, "v_rport_enable failed to call l_snmp_objfact_create to create the objfact"); v_rport_disable (self, rserv); return -1; }
  rserv->rport_objfact->dev = self->hierarchy->dev;
  rserv->rport_objfact->cnt = rserv->rport_cnt;
  asprintf (&rserv->rport_objfact->name_oid_str, "enterprises.1991.1.1.4.20.1.1.3.%s", index_oidstr);
  rserv->rport_objfact->fabfunc = v_rport_objfact_fab;
  rserv->rport_objfact->ctrlfunc = v_rport_objfact_ctrl;
  rserv->rport_objfact->cleanfunc = v_rport_objfact_clean;
  rserv->rport_objfact->passdata = strdup (index_oidstr);

  /* Start the object factory */
  num = l_snmp_objfact_start (self, rserv->rport_objfact);
  if (num != 0)
  { i_printf (1, "v_rport_enable failed to call l_snmp_objfact_start to start the object factory"); v_rport_disable (self, rserv); return -1; }

  /*
   * Misc
   */
  
  /* Enqueue into cnt list */
  if (!static_cnt_list)
  { static_cnt_list = i_list_create (); }
  i_list_enqueue (static_cnt_list, rserv->rport_cnt);

  return 0;
}

int v_rport_disable (i_resource *self, v_rserv_item *rserv)
{
  /* Disable the sub-system */
  int num;

  /* Remove from cnt list */
  if (static_cnt_list)
  { 
    num = i_list_search (static_cnt_list, rserv->rport_cnt);
    if (num == 0)
    { i_list_delete (static_cnt_list); }
    if (static_cnt_list->size < 1)
    { i_list_free (static_cnt_list); static_cnt_list = NULL; }
  }

  /* Terminate the object factory */
  if (rserv->rport_objfact)
  { l_snmp_objfact_stop (self, rserv->rport_objfact); rserv->rport_objfact = NULL; }

  /* Deregister container */
  if (rserv->rport_cnt)
  { i_entity_deregister (self, ENTITY(rserv->rport_cnt)); i_entity_free (ENTITY(rserv->rport_cnt)); rserv->rport_cnt = NULL; }


  return 0;
}

/* Container validation */

int v_rport_cnt_validate (i_container *cnt)
{
  /* Verifies that the specified container is infact a rport container.
   * This is used to validate objects passed to forms
   */
  
  if (static_cnt_list) return i_list_search (static_cnt_list, cnt);
  
  return -1;
}
