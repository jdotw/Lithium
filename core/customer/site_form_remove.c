#include <stdio.h>
#include <stdlib.h>
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

int form_site_remove (i_resource *self, i_form_reqdata *reqdata)
{
  i_site *site;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN) 
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_site_remove failed to create form"); return -1; }

  /* Check for site entity */
  if (reqdata->entaddr)
  {
    /* Site specified, remove confirm */
    site = (i_site *) i_entity_local_get (self, reqdata->entaddr);
    if (!site)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified site not found"); return 1; }

    /* Frame start */
    i_form_frame_start (reqdata->form_out, "site_remove", "Confirm Site Remove");
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No site specified"); return 1; }

  /* Info */
  i_form_string_add (reqdata->form_out, "name", "Site Name", site->name_str);
  i_form_string_add (reqdata->form_out, "desc", "Site Description", site->desc_str);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, "site_remove");

  return 1;
}

int form_site_remove_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  i_site *site;

  /* Auth checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }
  
  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_site_remove_submit unable to create form"); return -1; }

  /* Check for specified site */
  if (reqdata->entaddr)
  {
    /* Site specified, remove */
    site = (i_site *) i_entity_local_get (self, reqdata->entaddr);
    if (!site)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified site not found"); return 1; }

    /* Start frame */
    i_form_frame_start (reqdata->form_out, "site_remove", "Remove Site");

    /* Info */
    i_form_string_add (reqdata->form_out, "name", "Site Name", site->name_str);
    i_form_string_add (reqdata->form_out, "desc", "Site Description", site->desc_str);
    i_form_spacer_add (reqdata->form_out);

    /* Call Remove */
    num = l_site_remove (self, site);
    if (num == 0)
    { i_form_string_add (reqdata->form_out, "msg", "Success", "Successfully removed site"); }
    else
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to remove site"); }

    /* End Frame */
    i_form_frame_end (reqdata->form_out, "site_remove");
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No site specified"); }
  
  return 1;
}

