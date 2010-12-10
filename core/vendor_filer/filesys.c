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

#include "filesys.h"

/* filesys - File System Sub-System */

static int static_enabled = 0;                        /* 0 = disabled / 1 = enabled */
static i_container *static_cnt = NULL;                /* Cement Container */
static l_snmp_objfact *static_objfact = NULL;         /* Object Factory Data */

/* Variable Fetching */

i_container* v_filesys_cnt ()
{ return static_cnt; }

/* Sub-System Enable / Disable */

int v_filesys_enable (i_resource *self)
{
  int num;
  i_triggerset *tset;
  static i_entity_refresh_config defrefconfig;

  if (static_enabled == 1)
  { i_printf (1, "v_filesys_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("filesys", "File System");
  if (!static_cnt)
  { i_printf (1, "v_filesys_enable failed to create container"); v_filesys_disable (self); return -1; }
  static_cnt->mainform_func = v_filesys_cntform;
  static_cnt->sumform_func = v_filesys_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_filesys_enable failed to register container"); v_filesys_disable (self); return -1; }

  /* Normal Handling */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_filesys_enable failed to load and apply container refresh config"); v_filesys_disable (self); return -1; }

  /* 
   * Trigger sets 
   */

  tset = i_triggerset_create ("bytes_used_pc", "Bytes Used Percent", "bytes_used_pc");
  i_triggerset_addtrg (self, tset, "failed", "Failed", VALTYPE_INTEGER, TRGTYPE_GT, 97, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_INTEGER, TRGTYPE_RANGE, 90, NULL, 97, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_INTEGER, TRGTYPE_RANGE, 70, NULL, 90, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("inodes_used_pc", "Inodes Used Percent", "inodes_used_pc");
  i_triggerset_addtrg (self, tset, "failed", "Failed", VALTYPE_INTEGER, TRGTYPE_GT, 97, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_INTEGER, TRGTYPE_RANGE, 90, NULL, 97, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_INTEGER, TRGTYPE_RANGE, 70, NULL, 90, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("files_used_pc", "Files Used Percent", "files_used_pc");
  i_triggerset_addtrg (self, tset, "failed", "Failed", VALTYPE_INTEGER, TRGTYPE_GT, 98, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_INTEGER, TRGTYPE_RANGE, 90, NULL, 97, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_INTEGER, TRGTYPE_RANGE, 70, NULL, 90, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* 
   * Items and Object Factory
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_filesys_enable failed to create item_list"); v_filesys_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* Create the object factory */
  static_objfact = l_snmp_objfact_create (self, "filesys", "File System");
  if (!static_objfact)
  { i_printf (1, "v_filesys_enable failed to call l_snmp_objfact_create to create the objfact"); v_filesys_disable (self); return -1; }
  static_objfact->dev = self->hierarchy->dev;
  static_objfact->cnt = static_cnt;
  static_objfact->name_oid_str = strdup ("enterprises.789.1.5.4.1.2");
  static_objfact->fabfunc = v_filesys_objfact_fab;
  static_objfact->ctrlfunc = v_filesys_objfact_ctrl;
  static_objfact->cleanfunc = v_filesys_objfact_clean;

  /* Start the object factory */
  num = l_snmp_objfact_start (self, static_objfact);
  if (num != 0)
  { i_printf (1, "v_filesys_enable failed to call l_snmp_objfact_start to start the object factory"); v_filesys_disable (self); return -1; }

  return 0;
}

int v_filesys_disable (i_resource *self)
{
  /* Disable the sub-system */

  if (static_enabled == 0)
  { i_printf (1, "v_filesys_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  /* Terminate the object factory */
  if (static_objfact)
  { l_snmp_objfact_stop (self, static_objfact); static_objfact = NULL; }

  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}
