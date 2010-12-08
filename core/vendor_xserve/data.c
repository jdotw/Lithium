#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/parser.h>

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

#include "plist.h"
#include "data.h"

/* Xserve Data Retrieval Sub-System */

static i_container *static_cnt = NULL;
static v_data_item *static_item = NULL;
static v_data_ipmi_item *static_ipmi_item = NULL;

/* Variable Retrieval */

i_container* v_data_cnt ()
{ return static_cnt; }

v_data_item* v_data_static_item ()
{ return static_item; }

v_data_ipmi_item* v_data_static_ipmi_item ()
{ return static_ipmi_item; }

/* Enable / Disable */

int v_data_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_object *obj;

  /* Create/Config Container */
  static_cnt = i_container_create ("xdata", "Data Retrieval");
  if (!static_cnt)
  { i_printf (1, "v_data_enable failed to create container"); v_data_disable (self); return -1; }
  static_cnt->hidden = 1;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_data_enable failed to register container"); v_data_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_data_enable failed to load and apply container refresh config"); v_data_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_data_enable failed to create item_list"); v_data_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* 
   * Master object
   */

  /* Create table object */
  obj = i_object_create ("master", "Master");

  /* Register table object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Create item */
  static_item = v_data_item_create ();
  static_item->obj = obj;
  obj->itemptr = static_item;

  /*
   * IPMI Object (timer-refreshed)
   */

  /* Create table object */
  obj = i_object_create ("ipmi", "IPMI");

  /* Register table object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Load/Apply IPMI refresh config (timer on device interval) */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_TIMER;
  defrefconfig.refresh_int_sec = 300;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  i_entity_refresh_config_loadapply (self, ENTITY(obj), &defrefconfig);
  
  /* Create item */
  static_ipmi_item = v_data_ipmi_item_create ();
  static_ipmi_item->obj = obj;
  obj->itemptr = static_ipmi_item;

  return 0;
}

int v_data_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

