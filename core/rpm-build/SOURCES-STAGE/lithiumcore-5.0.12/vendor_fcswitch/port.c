#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/navtree.h"
#include "induction/navform.h"
#include "induction/hierarchy.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "device/snmp.h"

#include "unit.h"
#include "port.h"

/* Fibre Channel Alliance - Port Sub-System */

i_container* v_port_enable (i_resource *self, v_unit_item *unit)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  char *name_str;
  char *desc_str;
  asprintf(&name_str, "%s_fcport", unit->obj->name_str);
  asprintf(&desc_str, "%s Ports", unit->obj->desc_str);
  i_container *cnt = i_container_create (name_str, desc_str);
  free (name_str);
  free (desc_str);

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(cnt));
  if (num != 0)
  { i_printf (1, "v_port_enable failed to register container"); return NULL; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_port_enable failed to load and apply container refresh config"); return NULL; }

  /*
   * Trigger Sets 
   */

  i_triggerset *tset = i_triggerset_create ("hwstate", "Hardware State", "hwstate");
  i_triggerset_addtrg (self, tset, "failed", "Failed Diag", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "txfault", "TX Fault", VALTYPE_INTEGER, TRGTYPE_EQUAL, 6, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "linkdown", "Link Down", VALTYPE_INTEGER, TRGTYPE_EQUAL, 8, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  tset->default_applyflag = 1;
  i_triggerset_assign (self, cnt, tset);

  tset = i_triggerset_create ("invalid_crc_rate", "CRC Errors", "invalid_crc_rate");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 1., NULL, 2., NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 2., NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  tset->default_applyflag = 1;
  i_triggerset_assign (self, cnt, tset);

  tset = i_triggerset_create ("encoding_error_rate", "Encoding Errors", "encoding_error_rate");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 1., NULL, 2., NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 2., NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  tset->default_applyflag = 1;
  i_triggerset_assign (self, cnt, tset);

  /*
   * Items and Object Factory
   */

  /* Create item list */
  cnt->item_list = i_list_create ();
  if (!cnt->item_list)
  { i_printf (1, "v_port_enable failed to create item_list"); return NULL; }
  i_list_set_destructor (cnt->item_list, v_port_item_free);
  cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  l_snmp_objfact *objfact = l_snmp_objfact_create (self, cnt->name_str, cnt->desc_str);
  if (!objfact)
  { 
    i_printf (1, "v_port_enable failed to call l_snmp_objfact_create to create objfact"); 
    return NULL; 
  }
  objfact->dev = self->hierarchy->dev;
  objfact->cnt = cnt;
  asprintf (&objfact->name_oid_str, ".1.3.6.1.3.94.1.10.1.18.%s", unit->oid_suffix);
  objfact->fabfunc = v_port_objfact_fab;
  objfact->ctrlfunc = v_port_objfact_ctrl;
  objfact->cleanfunc = v_port_objfact_clean;
  objfact->passdata = unit;

  /* Start the object factory */
  num = l_snmp_objfact_start (self, objfact);
  if (num != 0)
  { 
    i_printf (1, "v_port_enable failed to call l_snmp_objfact_start to start the object factory");
    return NULL;
  }         

  return cnt;
}

v_port_item* v_port_item_create ()
{
  v_port_item *item;

  item = (v_port_item *) malloc (sizeof(v_port_item));
  if (!item)
  { i_printf (1, "v_port_item_create failed to malloc v_port_item struct"); return NULL; }
  memset (item, 0, sizeof(v_port_item));

  return item;
}

void v_port_item_free (void *itemptr)
{
  v_port_item *item = itemptr;

  if (!item) return;

  free (item);
}

