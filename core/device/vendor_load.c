#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/device.h>
#include <induction/inventory.h>
#include <induction/construct.h>
#include <induction/hierarchy.h>
#include <induction/vendor.h>
#include <induction/files.h>
#include <induction/configfile.h>
#include <induction/module.h>

#include "vendor.h"

/* Vendor module loading code */

int l_vendor_load (i_resource *self, char *vendor_id)
{
  int num;
  i_vendor *vendor;

  vendor = i_vendor_create ();
  if (!vendor)
  { i_printf (1, "l_vendor_load failed to create vendor struct"); return -1; }

  vendor->name_str = strdup (vendor_id);
  asprintf (&vendor->module_str, "vendor_modules/%s.so", vendor_id);

  vendor->module = i_module_open (self->construct->module_path, vendor->module_str);
  if (!vendor->module)
  { i_printf (1, "l_vendor_load failed to load module for vendor is %s", vendor_id); i_vendor_free (vendor); return -1; }

  if (vendor->module->init)
  { 
    num = vendor->module->init (self);
    if (num != 0)
    { i_printf (1, "l_vendor_load failed to initialise module for vendor %s", vendor_id); i_vendor_free (vendor); return -1; }
  }

  if (vendor->module->entry)
  { 
    num = vendor->module->entry (self);
    if (num != 0)
    { i_printf (1, "l_vendor_load failed to enter module for vendor %s", vendor_id); i_vendor_free (vendor); return -1; }
  }

  i_vendor_set (vendor);
    
  return 0;
}

int l_vendor_unload (i_resource *self, i_vendor *vendor)
{
  int num;
  
  if (vendor->module->shutdown)
  { 
    num = vendor->module->shutdown (self);
    if (num != 0)
    { i_printf (1, "l_vendor_load failed to shutdown vendor module"); }
  }

  i_module_free (vendor->module);
  vendor->module = NULL;

  return 0;
}
