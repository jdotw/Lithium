#include <stdlib.h>
#include <math.h>
#include <string.h>

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

int form_dhcp_subnet_list (i_resource *self, i_form_reqdata *reqdata)
{
  i_form_item *table;
  l_dhcp_subnet *subnet;
  i_list *subnet_list;
  i_site *site;
  char *str;
  char *labels[2] = { "Subnet", "Site" };

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_dhcp_subnet_list failed to create form"); return -1; }

  /* Get subnet list */
  if (reqdata->entaddr)
  {
    /* Site specified, find site */
    site = (i_site *) i_entity_local_get (self, reqdata->entaddr);
    if (!site)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified site not found"); return 1; }
    subnet_list = site->subnet_list;
  }
  else
  {
    /* No site specified, use global list */ 
    site = NULL;
    subnet_list = l_dhcp_subnet_list (); 
  }

  /* Start frame */
  if (site)
  { 
    asprintf (&str, "%s %s DHCP Subnet List", site->desc_str, site->suburb_str); 
    i_form_frame_start (reqdata->form_out, "dhcp_subnetlist", str);
    free (str);
  }
  else
  { i_form_frame_start (reqdata->form_out, "dhcp_subnetlist", "DHCP Subnet List (All Sites)"); }

  /* Subnet table */
  table = i_form_table_create (reqdata->form_out, "subnet_list", NULL, 2);
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 0, 0, "142", 4);

  /* Add header row */
  i_form_table_add_row (table, labels);

  /* Loop through subnets */
  for (i_list_move_head(subnet_list); (subnet=i_list_restore(subnet_list))!=NULL; i_list_move_next(subnet_list))
  {
    int row;
    char *pass_str;

    /* Subnet */
    asprintf (&labels[0], "%s/%.0f", subnet->network_str, 32 - (log(ntohl(~subnet->mask_addr.s_addr)) / log(2)));

    /* Site */
    if (subnet->site->suburb_str)
    { asprintf (&labels[1], "%s %s", subnet->site->desc_str, subnet->site->suburb_str); }
    else
    { asprintf (&labels[1], "%s", subnet->site->desc_str); }

    /* Add row */
    row = i_form_table_add_row (table, labels);

    /* Links */
    asprintf (&pass_str, "%li", subnet->id);
    i_form_table_add_link (table, 0, row, NULL, NULL, "dhcp_subnet_main", 0, pass_str, strlen(pass_str)+1);
    free (pass_str);
    if (subnet->site)
    { i_form_table_add_link (table, 1, row, NULL, ENT_ADDR(subnet->site), "device_list", 0, NULL, 0); }
  }

  /* End subnet list frame */
  i_form_frame_end (reqdata->form_out, "dhcp_subnetlist");

  /* Subnet Admin */
  if (site && reqdata->auth->level >= AUTH_LEVEL_STAFF)
  {
    i_form_item *item;
    
    i_form_frame_start (reqdata->form_out, "dhcp_subnetlist_admin", "Subnet Administration");

    item = i_form_string_add (reqdata->form_out, "subnet_add", "Add New Subnet", NULL);
    i_form_item_add_link (item, 0, 0, 0, RES_ADDR(self), ENT_ADDR(site), "dhcp_subnet_edit", 0, NULL, 0);

    i_form_frame_end (reqdata->form_out, "dhcp_subnetlist_admin");
  }
  
  return 1;
}
