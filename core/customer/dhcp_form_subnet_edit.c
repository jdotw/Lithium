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

int form_dhcp_subnet_edit (i_resource *self, i_form_reqdata *reqdata)
{
  char *str;
  i_site *site;
  l_dhcp_subnet *subnet = NULL;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied(self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_dhcp_subnet_edit failed to create form"); return -1; }

  /* Find site */
  if (reqdata->entaddr)
  {
    site = (i_site *) i_entity_local_get (self, reqdata->entaddr);
    if (!site)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified site not found"); return 1; }
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No site specified"); return 1; }

  /* Check for specified subnet */
  if (reqdata->form_passdata && reqdata->form_passdata_size > 0)
  {
    char *id_str;
    id_str = (char *) reqdata->form_passdata;
    subnet = l_dhcp_subnet_get (self, atol(id_str));
    if (!subnet)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified subnet not found"); return 1; }
    i_form_frame_start (reqdata->form_out, "subnet_edit", "Edit DHCP Subnet"); 
  }
  else
  { i_form_frame_start (reqdata->form_out, "subnet_edit", "Add DHCP Subnet"); }

  /* Add ID (if present) */
  if (subnet)
  { 
    asprintf (&str, "%li", subnet->id); 
    i_form_hidden_add (reqdata->form_out, "id", str); 
    free (str); 
  }

  /* Site */
  i_form_hidden_add (reqdata->form_out, "site_name", site->name_str);
  asprintf (&str, "%s %s", site->desc_str, site->suburb_str);
  i_form_string_add (reqdata->form_out, "site_desc", "Site", str);
  free (str);

  /* Network */
  if (subnet) i_form_entry_add (reqdata->form_out, "network", "Network", subnet->network_str);
  else i_form_entry_add (reqdata->form_out, "network", "Network", NULL); 

  /* Mask */
  if (subnet) i_form_entry_add (reqdata->form_out, "mask", "Mask", subnet->mask_str);
  else i_form_entry_add (reqdata->form_out, "mask", "Mask", NULL); 

  /* Router */
  if (subnet) i_form_entry_add (reqdata->form_out, "router", "Router", subnet->router_str);
  else i_form_entry_add (reqdata->form_out, "router", "Router", NULL); 

  /* Domain */
  if (subnet) i_form_entry_add (reqdata->form_out, "domain", "Domain Suffix", subnet->domain_str);
  else i_form_entry_add (reqdata->form_out, "domain", "Domain Suffix", NULL); 

  /* Options */
  if (subnet) i_form_textarea_add (reqdata->form_out, "options", "Options", subnet->options_str);
  else i_form_textarea_add (reqdata->form_out, "options", "Options", NULL); 

  /* End frame */
  i_form_frame_end (reqdata->form_out, "subnet_edit");
  
  return 1;
}

int form_dhcp_subnet_edit_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  l_dhcp_subnet *subnet = NULL;
  i_form_item_option *id_opt;
  i_form_item_option *site_opt;
  i_form_item_option *network_opt;
  i_form_item_option *mask_opt;
  i_form_item_option *router_opt;
  i_form_item_option *domain_opt;
  i_form_item_option *options_opt;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied(self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_dhcp_subnet_edit_submit failed to create form"); return -1; }

  /* Retrieve Values */
  site_opt = i_form_get_value_for_item (reqdata->form_in, "site_name");
  if (!site_opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Site name not found in form"); return 1; }
  network_opt = i_form_get_value_for_item (reqdata->form_in, "network");
  if (!network_opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Network not found in form"); return 1; }
  mask_opt = i_form_get_value_for_item (reqdata->form_in, "mask");
  if (!mask_opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Mask not found in form"); return 1; }
  router_opt = i_form_get_value_for_item (reqdata->form_in, "router");
  if (!router_opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Router not found in form"); return 1; }
  domain_opt = i_form_get_value_for_item (reqdata->form_in, "domain");
  if (!domain_opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Domain not found in form"); return 1; }
  options_opt = i_form_get_value_for_item (reqdata->form_in, "options");
  if (!options_opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Options not found in form"); return 1; }

  /* Find subnet */
  id_opt = i_form_get_value_for_item (reqdata->form_in, "id");
  if (id_opt)
  { subnet = l_dhcp_subnet_get (self, atol((char *) id_opt->data)); }

  /* Edit/Add Handling */
  if (subnet)
  {
    /* Existing Subnet Edit */
    i_form_frame_start (reqdata->form_out, "subnet_edit", "Edit DHCP Subnet");

    /* Network */
    if (subnet->network_str) { free (subnet->network_str); subnet->network_str = NULL; }
    subnet->network_str = strdup ((char *)network_opt->data);
    inet_aton (subnet->network_str, &subnet->network_addr);
    if (subnet->network_str) { inet_aton (subnet->network_str, &subnet->network_addr); }
    else { memset (&subnet->network_addr, 0, sizeof(struct in_addr)); }
    
    /* Mask */
    if (subnet->mask_str) { free (subnet->mask_str); subnet->mask_str = NULL; }
    subnet->mask_str = strdup ((char *)mask_opt->data);
    inet_aton (subnet->network_str, &subnet->network_addr);
    if (subnet->mask_str) { inet_aton (subnet->mask_str, &subnet->mask_addr); }
    else { memset (&subnet->mask_addr, 0, sizeof(struct in_addr)); }
    
    /* Router */
    if (subnet->router_str) { free (subnet->router_str); subnet->router_str = NULL; }
    subnet->router_str = strdup ((char *)router_opt->data);
    if (subnet->router_str) { inet_aton (subnet->router_str, &subnet->router_addr); }
    else { memset (&subnet->router_addr, 0, sizeof(struct in_addr)); }

    /* Domain */
    if (subnet->domain_str) { free (subnet->domain_str); subnet->domain_str = NULL; }
    subnet->domain_str = strdup ((char *)domain_opt->data);
    
    /* Options */
    if (subnet->options_str) { free (subnet->options_str); subnet->options_str = NULL; }
    subnet->options_str = strdup ((char *)options_opt->data);

    /* Update subnet */
    num = l_dhcp_subnet_update (self, subnet);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to update DHCP Subnet"); }
    else
    { i_form_string_add (reqdata->form_out, "msg", "Success", "DHCP subnet successfully updated"); }
  }
  else
  {
    /* Add New Subnet */
    i_form_frame_start (reqdata->form_out, "subnet_edit", "Add DHCP Subnet");

    /* Add Subnet */
    subnet = l_dhcp_subnet_add (self, (char *) site_opt->data, (char *) network_opt->data, (char *) mask_opt->data, (char *) router_opt->data, (char *) domain_opt->data, (char *) options_opt->data);
    if (!subnet)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add DHCP Subnet"); }
    else
    { i_form_string_add (reqdata->form_out, "msg", "Success", "DHCP subnet successfully added"); }
  }

  /* End Frame */
  i_form_frame_end (reqdata->form_out, "subnet_edit");

  return 1;
}

