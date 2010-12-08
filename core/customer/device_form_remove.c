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
#include <induction/device.h>
#include <induction/construct.h>
#include <induction/list.h>

#include "device.h"

int form_device_remove (i_resource *self, i_form_reqdata *reqdata)
{
  char *str;
  i_device *dev;
  i_site *site;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN) 
  { return i_form_deliver_denied (self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out)
  { i_printf (1, "form_device_remove failed to create form"); return -1; }

  /* Check for device entity */
  if (reqdata->entaddr)
  {
    /* Site specified, remove confirm */
    dev = (i_device *) i_entity_local_get (self, reqdata->entaddr);
    if (!dev)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified device not found"); return 1; }

    /* Frame start */
    i_form_frame_start (reqdata->form_out, "device_remove", "Confirm Device Remove");
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No device specified"); return 1; }

  /* Info */
  site = (i_site *) dev->site;
  asprintf (&str, "%s %s (%s)", site->desc_str, site->suburb_str, site->name_str);
  i_form_string_add (reqdata->form_out, "desc", "Site", str);
  free (str);
  asprintf (&str, "%s (%s)", dev->desc_str, dev->name_str);
  i_form_string_add (reqdata->form_out, "desc", "Device", str);
  free (str);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, "device_remove");

  return 1;
}

int form_device_remove_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  char *str;
  i_device *dev;
  i_site *site;

  /* Auth checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }
  
  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_device_remove_submit unable to create form"); return -1; }

  /* Check for specified device */
  if (reqdata->entaddr)
  {
    /* Site specified, remove */
    dev = (i_device *) i_entity_local_get (self, reqdata->entaddr);
    if (!dev)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified device not found"); return 1; }

    /* Start frame */
    i_form_frame_start (reqdata->form_out, "device_remove", "Remove Site");

    /* Info */
    site = (i_site *) dev->site;
    asprintf (&str, "%s %s (%s)", site->desc_str, site->suburb_str, site->name_str);
    i_form_string_add (reqdata->form_out, "desc", "Site", str);
    free (str);
    asprintf (&str, "%s (%s)", dev->desc_str, dev->name_str);
    i_form_string_add (reqdata->form_out, "desc", "Device", str);
    free (str);
    i_form_spacer_add (reqdata->form_out);

    /* Call Remove */
    num = l_device_remove (self, dev);
    if (num == 0)
    { i_form_string_add (reqdata->form_out, "msg", "Success", "Successfully removed device"); }
    else
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to remove device"); }

    /* End Frame */
    i_form_frame_end (reqdata->form_out, "device_remove");
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No device specified"); }
  
  return 1;
}

