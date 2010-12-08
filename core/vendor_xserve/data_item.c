#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/path.h>
#include <lithium/snmp.h>

#include "plist.h"
#include "data.h"

/* Item Struct Manipulation */

v_data_item* v_data_item_create ()
{
  v_data_item *item;

  item = (v_data_item *) malloc (sizeof(v_data_item));
  if (!item)
  { i_printf (1, "v_data_item_create failed to malloc v_data_item struct"); return NULL; }
  memset (item, 0, sizeof(v_data_item));

  return item;
}

void v_data_item_free (void *itemptr)
{
  v_data_item *item = itemptr;

  if (!item) return;

  free (item);
}

v_data_ipmi_item* v_data_ipmi_item_create ()
{
  v_data_ipmi_item *item;

  item = (v_data_ipmi_item *) malloc (sizeof(v_data_ipmi_item));
  if (!item)
  { i_printf (1, "v_data_item_create failed to malloc v_data_ipmi_item struct"); return NULL; }
  memset (item, 0, sizeof(v_data_ipmi_item));

  return item;
}

void v_data_ipmi_item_free (void *itemptr)
{
  v_data_ipmi_item *item = itemptr;

  if (!item) return;

  free (item);
}
