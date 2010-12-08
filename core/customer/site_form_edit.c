#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/module.h>
#include <induction/auth.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/site.h>
#include <induction/construct.h>
#include <induction/list.h>

#include "site.h"

int form_site_edit (i_resource *self, i_form_reqdata *reqdata)
{
  char *str;
  i_site *site;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN) 
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_site_edit failed to create form"); return -1; }

  /* Check for site entity */
  if (reqdata->entaddr)
  {
    /* Site specified, edit existing */
    site = (i_site *) i_entity_local_get (self, reqdata->entaddr);
    if (!site)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified site not found"); return 1; }

    /* Frame start */
    i_form_frame_start (reqdata->form_out, "site_edit", "Edit Existing Site");
  }
  else
  {
    /* No site specified, add new */
    site = NULL;
    
    /* Frame start */
    i_form_frame_start (reqdata->form_out, "site_edit", "Add New Site");
  }

  /* Name */
  if (site) 
  {
    i_form_string_add (reqdata->form_out, "name_label", "Site Name", site->name_str);
    i_form_hidden_add (reqdata->form_out, "name", site->name_str);
  }
  else i_form_entry_add (reqdata->form_out, "name", "Site Name", NULL);

  /* Desc */
  if (site) i_form_entry_add (reqdata->form_out, "desc", "Site Description", site->desc_str);
  else i_form_entry_add (reqdata->form_out, "desc", "Site Description", NULL);

  /* Addr1 */
  if (site) i_form_entry_add (reqdata->form_out, "addr1", "Address Line 1", site->addr1_str);
  else i_form_entry_add (reqdata->form_out, "addr1", "Address Line 1", NULL);

  /* Addr2 */
  if (site) i_form_entry_add (reqdata->form_out, "addr2", "Address Line 2", site->addr2_str);
  else i_form_entry_add (reqdata->form_out, "addr2", "Address Line 2", NULL);

  /* Addr3 */
  if (site) i_form_entry_add (reqdata->form_out, "addr3", "Address Line 3", site->addr3_str);
  else i_form_entry_add (reqdata->form_out, "addr3", "Address Line 3", NULL);

  /* Suburb */
  if (site) i_form_entry_add (reqdata->form_out, "suburb", "Suburb", site->suburb_str);
  else i_form_entry_add (reqdata->form_out, "suburb", "Suburb", NULL);

  /* State */
  if (site) i_form_entry_add (reqdata->form_out, "state", "State", site->state_str);
  else i_form_entry_add (reqdata->form_out, "state", "State", NULL);

  /* Postcode */
  if (site) i_form_entry_add (reqdata->form_out, "postcode", "Postcode", site->postcode_str);
  else i_form_entry_add (reqdata->form_out, "postcode", "Postcode", NULL);

  /* Country */
  if (site) i_form_entry_add (reqdata->form_out, "country", "Country", site->country_str);
  else i_form_entry_add (reqdata->form_out, "country", "Country", NULL);

  /* Longitude */
  if (site) 
  {
    asprintf (&str, "%f", site->longitude);
    i_form_entry_add (reqdata->form_out, "longitude", "Longitude", str);
    free (str);
  }
  else i_form_entry_add (reqdata->form_out, "longitude", "Longitude", NULL);

  /* Latitude */
  if (site) 
  {
    asprintf (&str, "%f", site->latitude);
    i_form_entry_add (reqdata->form_out, "latitude", "Latitude", str);
    free (str);
  }
  else i_form_entry_add (reqdata->form_out, "latitude", "Latitude", NULL);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, "site_edit");

  return 1;
}

int form_site_edit_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  i_site *site;
  i_form_item_option *name_opt;
  i_form_item_option *desc_opt;
  i_form_item_option *addr1_opt;
  i_form_item_option *addr2_opt;
  i_form_item_option *addr3_opt;
  i_form_item_option *suburb_opt;
  i_form_item_option *state_opt;
  i_form_item_option *postcode_opt;
  i_form_item_option *country_opt;
  i_form_item_option *longitude_opt;
  i_form_item_option *latitude_opt;

  /* Auth checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }
  
  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_site_add_submit unable to create form"); return -1; }

  /* Name */
  name_opt = i_form_get_value_for_item (reqdata->form_in, "name");
  if (!name_opt || !name_opt->data)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Name not found in form"); return 1; }

  /* Other fields */
  desc_opt = i_form_get_value_for_item (reqdata->form_in, "desc");
  if (!desc_opt || !desc_opt->data)  
  { i_form_string_add (reqdata->form_out, "error", "Error", "Description not found in form"); return 1; }
  addr1_opt = i_form_get_value_for_item (reqdata->form_in, "addr1");
  addr2_opt = i_form_get_value_for_item (reqdata->form_in, "addr2");
  addr3_opt = i_form_get_value_for_item (reqdata->form_in, "addr3");
  suburb_opt = i_form_get_value_for_item (reqdata->form_in, "suburb");
  state_opt = i_form_get_value_for_item (reqdata->form_in, "state");
  postcode_opt = i_form_get_value_for_item (reqdata->form_in, "postcode");
  country_opt = i_form_get_value_for_item (reqdata->form_in, "country");
  if (!addr1_opt || !addr2_opt || !addr3_opt || !suburb_opt || !state_opt || !postcode_opt || !country_opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Address info not found in form"); return 1; }
  longitude_opt = i_form_get_value_for_item (reqdata->form_in, "longitude");
  latitude_opt = i_form_get_value_for_item (reqdata->form_in, "latitude");
  if (!longitude_opt || !latitude_opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Longitude/Latitude info not found in form"); return 1; }

  /* Check for specified site */
  if (reqdata->entaddr)
  {
    /* Site specified, edit existing site */
    site = (i_site *) i_entity_local_get (self, reqdata->entaddr);
    if (!site)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified site not found"); return 1; }

    /* Start frame */
    i_form_frame_start (reqdata->form_out, "site_edit", "Edit Existing Site");

    /* Description */
    if (site->desc_str) free (site->desc_str);
    if (desc_opt && desc_opt->data) site->desc_str = strdup ((char *)desc_opt->data);
    else site->desc_str = NULL;

    /* Address 1 */
    if (site->addr1_str) free (site->addr1_str);
    if (addr1_opt && addr1_opt->data) site->addr1_str = strdup ((char *)addr1_opt->data);
    else site->addr1_str = NULL;

    /* Address 2 */
    if (site->addr2_str) free (site->addr2_str);
    if (addr2_opt && addr2_opt->data) site->addr2_str = strdup ((char *)addr2_opt->data);
    else site->addr2_str = NULL;

    /* Address 3 */
    if (site->addr3_str) free (site->addr3_str);
    if (addr3_opt && addr3_opt->data) site->addr3_str = strdup ((char *)addr3_opt->data);
    else site->addr3_str = NULL;

    /* Suburb */
    if (site->suburb_str) free (site->suburb_str);
    if (suburb_opt && suburb_opt->data) site->suburb_str = strdup ((char *)suburb_opt->data);
    else site->suburb_str = NULL;

    /* State */
    if (site->state_str) free (site->state_str);
    if (state_opt && state_opt->data) site->state_str = strdup ((char *)state_opt->data);
    else site->state_str = NULL;

    /* Postcode */
    if (site->postcode_str) free (site->postcode_str);
    if (postcode_opt && postcode_opt->data) site->postcode_str = strdup ((char *)postcode_opt->data);
    else site->postcode_str = NULL;

    /* Country */
    if (site->country_str) free (site->country_str);
    if (country_opt && country_opt->data) site->country_str = strdup ((char *)country_opt->data);
    else site->country_str = NULL;

    /* Longitude */
    if (longitude_opt && longitude_opt->data) site->longitude = atof ((char *)longitude_opt->data);
    else site->longitude = 0;

    /* Latitude */
    if (latitude_opt && latitude_opt->data) site->latitude = atof ((char *)latitude_opt->data);
    else site->latitude = 0;

    /* Call update */
    num = l_site_update (self, site);
    if (num == 0)
    { i_form_string_add (reqdata->form_out, "msg", "Success", "Successfully updated site"); }
    else
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to update site"); }
  }
  else
  {
    /* Start frame */
    i_form_frame_start (reqdata->form_out, "site_edit", "Add New Site");

    /* Create site */
    site = l_site_add (self, (char *) name_opt->data, (char *) desc_opt->data, (char *) addr1_opt->data, (char *) addr2_opt->data, (char *) addr3_opt->data, (char *) suburb_opt->data, (char *) state_opt->data, (char *) postcode_opt->data, (char *) country_opt->data, atof ((char *) longitude_opt->data), atof ((char *) latitude_opt->data));
    if (site)
    { i_form_string_add (reqdata->form_out, "msg", "Success", "Successfully added site"); }
    else
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add site"); }
  }

  i_form_frame_end (reqdata->form_out, "site_edit");
  
  return 1;
}

