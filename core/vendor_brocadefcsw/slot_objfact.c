#include <stdlib.h>

#include "induction.h"
#include "induction/timer.h"
#include "induction/timeutil.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/str.h"
#include "induction/hierarchy.h"
#include "induction/list.h"

#include "device/snmp.h"

#include "port.h"
#include "slot.h"

/* 
 * Brocade Fibre Channel Ports (Not Slot-specific) - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_slot_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  char *prefix_str = (char *) passdata;

  /* Object Configuration */
  char *desc_str = l_snmp_get_string_from_pdu(pdu);

  /* Check to see if the specified (desc) has a slot/slot format */
  char *full_prefix;
  asprintf(&full_prefix, "%s/", prefix_str);
  if (strstr(desc_str, full_prefix) == desc_str)    // Matches prefix AND at start of string
  {
    /* Fabricate the port */
    v_port_objfact_fab_port(self, cnt, obj, pdu, index_oidstr, NULL);
    free (full_prefix);
    return 0;
  }
  else
  {
    /* Disregard port not in this slot */
    free (full_prefix);
    return -1;
  }
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_slot_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_slot_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  v_port_item *slot = obj->itemptr;

  if (!slot) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, slot);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
