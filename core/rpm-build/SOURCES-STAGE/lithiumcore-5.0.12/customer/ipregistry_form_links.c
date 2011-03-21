#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/ip.h>
#include <induction/message.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/socket.h>
#include <induction/msgproc.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/entity.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/ipregistry.h>

#include "ipregistry.h"

int l_ipregistry_form_links (i_resource *self, i_form_reqdata *reqdata)
{
  int row;
  char *labels[5] = { "Network List", "Address List", "Site List", "Device List", "Duplicate List" };
  i_form_item *item;
  
  item = i_form_table_create (reqdata->form_out, "ipregistry_links", NULL, 4);
  if (!item) { i_printf (1, "l_ipregistry_form_links failed to create table"); return -1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  row = i_form_table_add_row (item, labels);  

  i_form_table_add_link (item, 0, row, NULL, NULL, "ipregistry_networklist", 0, NULL, 0);
  i_form_table_add_link (item, 1, row, NULL, NULL, "ipregistry_iplist", 0, NULL, 0);
  i_form_table_add_link (item, 2, row, NULL, NULL, "ipregistry_sitelist", 0, NULL, 0);
  i_form_table_add_link (item, 3, row, NULL, NULL, "ipregistry_devicelist", 0, NULL, 0);
  i_form_table_add_link (item, 4, row, NULL, NULL, "ipregistry_duplist", 0, NULL, 0);

  return 0;
}
