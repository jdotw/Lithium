#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/site.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>

#include "dhcp.h"

/* 
 * ISC DHCP Management Sub-System 
 */

int form_dhcp_subnet_main (i_resource *self, i_form_reqdata *reqdata)
{
  int row;
  unsigned long id;
  char *str;
  char *labels[4] = { "Start", "End", "Edit", "Remove" };
  i_form_item *table;
  l_dhcp_range *range;
  l_dhcp_subnet *subnet;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_dhcp_subnet_list failed to create form"); return -1; }

  /* Check passdata */
  if (!reqdata->form_passdata)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No subnet specified"); return 1; }

  /* Get subnet */
  id = strtoul (reqdata->form_passdata, NULL, 10);
  subnet = l_dhcp_subnet_get (self, id);
  if (!subnet)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified subnet not found"); return 1; }
  
  /* Start frame */
  asprintf (&str, "DHCP Subnet %s/%.0f", subnet->network_str, 32 - (log(ntohl(~subnet->mask_addr.s_addr)) / log(2)));
  i_form_frame_start (reqdata->form_out, "dhcp_subnet", str);
  free (str);

  /* Site */
  if (subnet->site->suburb_str)
  { asprintf (&str, "%s %s", subnet->site->desc_str, subnet->site->suburb_str); }
  else
  { asprintf (&str, "%s", subnet->site->desc_str); }
  i_form_string_add (reqdata->form_out, "site", "Site", str);
  free (str);

  /* Subnet Address */
  i_form_string_add (reqdata->form_out, "network", "Network Address", subnet->network_str);
  i_form_string_add (reqdata->form_out, "mask", "Network Mask", subnet->mask_str);
  asprintf (&str, "%.0fbit", 32 - (log(ntohl(~subnet->mask_addr.s_addr)) / log(2)));
  i_form_string_add (reqdata->form_out, "prefix_length", "Prefix Length", str);
  free (str);
  i_form_spacer_add (reqdata->form_out);

  /* Subnet Config */
  i_form_string_add (reqdata->form_out, "router", "Router", subnet->router_str);
  i_form_string_add (reqdata->form_out, "domain", "Domain", subnet->domain_str);
  i_form_string_add (reqdata->form_out, "options", "Other Options", subnet->options_str);
  i_form_spacer_add (reqdata->form_out);

  /* 
   * Ranges
   */

  /* Create table */
  table = i_form_table_create (reqdata->form_out, "ranges", "DHCP Address Ranges", 4);
  i_form_table_add_row (table, labels);
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);

  /* Loop through ranges */
  for (i_list_move_head(subnet->range_list); (range=i_list_restore(subnet->range_list))!=NULL; i_list_move_next(subnet->range_list))
  {
    char *pass_str;

    labels[0] = range->start_str;
    labels[1] = range->end_str;
    row = i_form_table_add_row (table, labels);

    asprintf (&pass_str, "%li:%li", subnet->id, range->id);
    i_form_table_add_link (table, 2, row, RES_ADDR(self), ENT_ADDR(subnet->site), "dhcp_range_edit", 0, pass_str, strlen(pass_str)+1);
    i_form_table_add_link (table, 3, row, RES_ADDR(self), ENT_ADDR(subnet->site), "dhcp_range_remove", 0, pass_str, strlen(pass_str)+1);
    free (pass_str);
  }

  /* End frame */
  i_form_frame_end (reqdata->form_out, "dhcp_subnet");

  /* 
   * Admin 
   */

  if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
  {
    char *pass_str;
    char *admin_labels[3];

    /* Start frame */
    i_form_frame_start (reqdata->form_out, "dhcp_subnet_admin", "Subnet Administration");

    table = i_form_table_create (reqdata->form_out, "admin_table", NULL, 3);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);
    i_form_item_add_option (table, ITEM_OPTION_NOHEADERS, 0, 0, 0, NULL, 0);
    admin_labels[0] = "Add Range";
    admin_labels[1] = "Edit Subnet";
    admin_labels[2] = "Remove Subnet";
    row = i_form_table_add_row (table, admin_labels);
    asprintf (&pass_str, "%li", subnet->id);
    i_form_table_add_link (table, 0, row, RES_ADDR(self), ENT_ADDR(subnet->site), "dhcp_range_edit", 0, pass_str, strlen(pass_str)+1);
    i_form_table_add_link (table, 1, row, RES_ADDR(self), ENT_ADDR(subnet->site), "dhcp_subnet_edit", 0, pass_str, strlen(pass_str)+1);
    i_form_table_add_link (table, 2, row, RES_ADDR(self), ENT_ADDR(subnet->site), "dhcp_subnet_remove", 0, pass_str, strlen(pass_str)+1);
    free (pass_str);

    /* End frame */
    i_form_frame_end (reqdata->form_out, "dhcp_subnet_admin");
  }

  return 1;
}
