#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/interface.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/adminstate.h>
#include <induction/triggerset.h>
#include <induction/str.h>
#include <induction/name.h>
#include <lithium/snmp.h>

#include "xsanvol.h"
#include "xsannode.h"
#include "xsanaffinity.h"
#include "xsansp.h"

/* 
 * SNMP Storage Resources - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_xsanvol_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_xsanvol_item *vol;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_xsanvol_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create storage item struct */
  vol = v_xsanvol_item_create ();
  if (!vol)
  { i_printf (1, "v_xsanvol_objfact_fab failed to create volume item for object %s", obj->name_str); return -1; }
  obj->itemptr = vol;
  vol->obj = obj;
  vol->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  vol->state = l_snmp_metric_create (self, obj, "state", "State", METRIC_INTEGER, ".1.3.6.1.4.1.20038.2.1.1.1.1.4", index_oidstr, RECMETHOD_NONE, 0);

  vol->devices = l_snmp_metric_create (self, obj, "devices", "Disk Devices", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.1.1.1.15", index_oidstr, RECMETHOD_NONE, 0);
  vol->stripe_groups = l_snmp_metric_create (self, obj, "stripe_groups", "Stripe Groups", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.1.1.1.16", index_oidstr, RECMETHOD_NONE, 0);

  vol->bytes_total = l_snmp_metric_create (self, obj, "bytes_total", "Size", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.1.1.1.24", index_oidstr, RECMETHOD_NONE, 0);
  vol->bytes_total->alloc_unit = 1024 * 1024;
  vol->bytes_total->valstr_func = i_string_volume_metric;
  vol->bytes_total->unit_str = strdup ("byte");
  vol->bytes_total->kbase = 1024;
  vol->bytes_total->summary_flag = 1;

  vol->bytes_free = l_snmp_metric_create (self, obj, "bytes_free", "Free", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.1.1.1.25", index_oidstr, RECMETHOD_NONE, 0);
  vol->bytes_free->alloc_unit = 1024 * 1024;
  vol->bytes_free->valstr_func = i_string_volume_metric;
  vol->bytes_free->unit_str = strdup ("byte");
  vol->bytes_free->kbase = 1024;
  vol->bytes_free->summary_flag = 1;

  vol->bytes_used = l_snmp_metric_create (self, obj, "bytes_used", "Used", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.1.1.1.26", index_oidstr, RECMETHOD_NONE, 0);
  vol->bytes_used->alloc_unit = 1024 * 1024;
  vol->bytes_used->valstr_func = i_string_volume_metric;
  vol->bytes_used->unit_str = strdup ("byte");
  vol->bytes_used->kbase = 1024;
  vol->bytes_used->summary_flag = 1;

  vol->used_pc = l_snmp_metric_create (self, obj, "used_pc", "Used Percent", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.1.1.1.21", index_oidstr, RECMETHOD_RRD, 0);
  vol->used_pc->record_defaultflag = 1;
  vol->used_pc->unit_str = strdup ("%");
  
  /* Enqueue the storage item */
  num = i_list_enqueue (cnt->item_list, vol);
  if (num != 0)
  { i_printf (1, "v_xsanvol_objfact_fab failed to enqueue storage item for object %s", obj->name_str); v_xsanvol_item_free (vol); return -1; }
  
  /*
   * Storage Pools Container and Object Factory
   */
   
  /* Create Container for Storage Pools */
  char *cnt_name;
  char *cnt_desc;
  asprintf (&cnt_name, "xsanvolsp_%s", obj->name_str);
  asprintf (&cnt_desc, "%s Storage Pools", obj->desc_str);
  i_name_parse (cnt_name);
  vol->sp_cnt = i_container_create (cnt_name, cnt_desc);
  free (cnt_name);
  cnt_name = NULL;
  free (cnt_desc);
  cnt_desc = NULL;
  vol->sp_cnt->item_list = i_list_create ();
  vol->sp_cnt->item_list_state = ITEMLIST_STATE_NORMAL;
   
  /* Create SNMP Object Factory for storage pools */
  vol->sp_objfact = l_snmp_objfact_create (self, vol->sp_cnt->name_str, vol->sp_cnt->desc_str);
  if (!vol->sp_objfact)
  {
    i_printf (1, "v_xsanvol_objfact_fab failed to call l_snmp_objfact_create to create the storage pool objfact"); 
    return -1;
  }
  vol->sp_objfact->dev = self->hierarchy->dev;
  vol->sp_objfact->cnt = vol->sp_cnt;
  asprintf (&vol->sp_objfact->name_oid_str, ".1.3.6.1.4.1.20038.2.1.1.2.1.3.%i", vol->index);
  vol->sp_objfact->fabfunc = v_xsansp_objfact_fab;
  vol->sp_objfact->ctrlfunc = v_xsansp_objfact_ctrl;
  vol->sp_objfact->cleanfunc = v_xsansp_objfact_clean;
  vol->sp_objfact->passdata = vol;
  num = l_snmp_objfact_start (self, vol->sp_objfact);
  if (num != 0)
  { 
    i_printf (1, "v_xsanvol_objfact_fab failed to call l_snmp_objfact_start to start the storage pool object factory"); 
  }
  
  /* Register entity */
  i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(vol->sp_cnt));

  /* Load/Apply refresh config */
  i_entity_refresh_config defrefconfig;
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  i_entity_refresh_config_loadapply (self, ENTITY(vol->sp_cnt), &defrefconfig);

  /* Storage Pool Triggers */
  i_triggerset *tset;
  tset = i_triggerset_create ("used_pc", "Percent Used", "used_pc");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 75, NULL, 85, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 85, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, vol->sp_cnt, tset);

  /*
   * Affinities Container and Storage Pool
   */
   
  /* Create Container for Storage Pools */
  asprintf (&cnt_name, "xsanvolaffinity_%s", obj->name_str);
  asprintf (&cnt_desc, "%s Affinities", obj->desc_str);
  i_name_parse (cnt_name);
  vol->affinity_cnt = i_container_create (cnt_name, cnt_desc);
  free (cnt_name);
  cnt_name = NULL;
  free (cnt_desc);
  cnt_desc = NULL;
  vol->affinity_cnt->item_list = i_list_create ();
  vol->affinity_cnt->item_list_state = ITEMLIST_STATE_NORMAL;
   
  /* Create SNMP Object Factory for storage pools */
  vol->affinity_objfact = l_snmp_objfact_create (self, vol->affinity_cnt->name_str, vol->affinity_cnt->desc_str);
  if (!vol->affinity_objfact)
  {
    i_printf (1, "v_xsanvol_objfact_fab failed to call l_snmp_objfact_create to create the storage pool objfact"); 
    return -1;
  }
  vol->affinity_objfact->dev = self->hierarchy->dev;
  vol->affinity_objfact->cnt = vol->affinity_cnt;
  asprintf (&vol->affinity_objfact->name_oid_str, ".1.3.6.1.4.1.20038.2.1.1.4.1.3.%i", vol->index);
  vol->affinity_objfact->fabfunc = v_xsanaffinity_objfact_fab;
  vol->affinity_objfact->ctrlfunc = v_xsanaffinity_objfact_ctrl;
  vol->affinity_objfact->cleanfunc = v_xsanaffinity_objfact_clean;
  vol->affinity_objfact->passdata = vol;
  num = l_snmp_objfact_start (self, vol->affinity_objfact);
  if (num != 0)
  { 
    i_printf (1, "v_xsanvol_objfact_fab failed to call l_snmp_objfact_start to start the storage pool object factory"); 
  }
  
  /* Register entity */
  i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(vol->affinity_cnt));

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  i_entity_refresh_config_loadapply (self, ENTITY(vol->affinity_cnt), &defrefconfig);

  /* Storage Pool Triggers */
  tset = i_triggerset_create ("used_pc", "Percent Used", "used_pc");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 75, NULL, 85, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 85, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, vol->affinity_cnt, tset);

  /*
   * LUNs Container and Storage Pool
   */
   
  /* Create Container for Storage Pools */
  asprintf (&cnt_name, "xsanvolluns_%s", obj->name_str);
  asprintf (&cnt_desc, "%s LUNs", obj->desc_str);
  i_name_parse (cnt_name);
  vol->lun_cnt = i_container_create (cnt_name, cnt_desc);
  free (cnt_name);
  cnt_name = NULL;
  free (cnt_desc);
  cnt_desc = NULL;
  vol->lun_cnt->item_list = i_list_create ();
  vol->lun_cnt->item_list_state = ITEMLIST_STATE_NORMAL;
   
  /* Create SNMP Object Factory for storage pools */
  vol->lun_objfact = l_snmp_objfact_create (self, vol->lun_cnt->name_str, vol->lun_cnt->desc_str);
  if (!vol->lun_objfact)
  {
    i_printf (1, "v_xsanvol_objfact_fab failed to call l_snmp_objfact_create to create the storage pool objfact"); 
    return -1;
  }
  vol->lun_objfact->dev = self->hierarchy->dev;
  vol->lun_objfact->cnt = vol->lun_cnt;
  asprintf (&vol->lun_objfact->name_oid_str, ".1.3.6.1.4.1.20038.2.1.1.3.1.4.%i", vol->index);
  vol->lun_objfact->fabfunc = v_xsannode_objfact_fab;
  vol->lun_objfact->ctrlfunc = v_xsannode_objfact_ctrl;
  vol->lun_objfact->cleanfunc = v_xsannode_objfact_clean;
  vol->lun_objfact->passdata = vol;
  num = l_snmp_objfact_start (self, vol->lun_objfact);
  if (num != 0)
  { 
    i_printf (1, "v_xsanvol_objfact_fab failed to call l_snmp_objfact_start to start the storage pool object factory"); 
  }
  
  /* Register entity */
  i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(vol->lun_cnt));

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  i_entity_refresh_config_loadapply (self, ENTITY(vol->lun_cnt), &defrefconfig);

  /* Storage Pool Triggers */
  tset = i_triggerset_create ("used_pc", "Percent Used", "used_pc");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 75, NULL, 85, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 85, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, vol->lun_cnt, tset);


  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_xsanvol_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
{
  /* Check the result */
  if (result == SNMP_ERROR_NOERROR)
  {
    /* No errors, set item list state to NORMAL */
    cnt->item_list_state = ITEMLIST_STATE_NORMAL;
  }

  return 0;
}

/* Object Factory Clean Func
 *
 * Called when an object is obsolete prior to it being deregistered and free
 */

int v_xsanvol_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  v_xsanvol_item *vol = obj->itemptr;
  
  /* Stop SP Object Fact */
  l_snmp_objfact_stop (self, vol->sp_objfact);

  /* Remove from item list */
  num = i_list_search (cnt->item_list, vol);
  if (num == 0) 
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;

  return 0;
}
