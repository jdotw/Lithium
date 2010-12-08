#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <dlfcn.h>

#include "induction.h"
#include "construct.h"
#include "module.h"

extern i_resource *global_self;

static i_resource_info static_resource_info [] =
{
  { RES_CORE, "Core", "liblithium_core.so", 0},
  { RES_EXEC, "Construct Executable", "construct", 0 },
  { RES_CLIENT_HANDLER_CORE, "Client Handler Core", "client_handler.so", 0 },
  { RES_CLIENT_HANDLER_CHILD, "Client Handler Child", "client_handler.so", 0 },
  { RES_ADMIN, "Administration", "admin.so", 0 },
  { RES_CUSTOMER, "Customer", "customer.so", 1 },
  { RES_SITE, "Site", "site.so", 1},
  { RES_DEVICE, "Device", "device.so", 1},
  { RES_SMS, "SMS Transceiver", "sms.so", 0},
};

/* Struct Manipulation */

i_resource_info* i_resource_info_create ()
{
  i_resource_info *info;

  info = (i_resource_info *) malloc (sizeof(i_resource_info));
  if (!info) { i_printf (1, "i_resource_info_create failed to malloc info struct"); return NULL; }
  memset (info, 0, sizeof(i_resource_info));

  return info;
}

void i_resource_info_free (i_resource_info *info)
{
  if (!info) return; 
  
  if (info->type_str) free (info->type_str);
  if (info->module_name) free (info->module_name);

  free (info);
}

i_resource_info* i_resource_info_duplicate (i_resource_info *info_in)
{
  i_resource_info *info_out;

  if (!info_in) return NULL;

  info_out = (i_resource_info *) malloc (sizeof(i_resource_info));
  if (!info_out) { i_printf (1, "i_resource_info_duplicate unable to malloc info_out"); return NULL; }
  memset (info_out, 0, sizeof(i_resource_info));

  info_out->type = info_in->type;
  if (info_in->type_str) info_out->type_str = strdup (info_in->type_str);
  if (info_in->module_name) info_out->module_name = strdup (info_in->module_name);
  info_out->user_spawn = info_in->user_spawn;

  return info_out;
}

/* Info Retrieval */

i_resource_info* i_resource_info_local (int type)
{
  i_resource_info *info;
  
  if (type < (int) (sizeof(static_resource_info)) / (int) (sizeof(i_resource_info)))
  { info = i_resource_info_duplicate (&static_resource_info[type]); }
  else
  { return NULL; }
  
  return info;
}

i_resource_info* i_resource_info_module (i_resource *self, char *module_name)
{
  /* Type info is not in the static_resource_info struct */
  /* Will need to load the res to check it*/

  const char *errstr;
  i_module *module;
  i_resource_info *info;
  i_resource_info* (*resource_info_func) ();

  if (!self || !self->construct) return NULL; 

  module = i_module_open (self->construct->module_path, module_name);        /* Open the module */
  if (!module) 
  { i_printf (1, "i_resource_get_info_from_module couldnt open module %s", module_name); return NULL; }
    
  resource_info_func = dlsym (module->handle, "resource_info");         /* Find the resource_info func */
  errstr = dlerror ();
  if (errstr) 
  { i_printf (1, "i_resource_get_info_from_module unable to find 'resource_info' func in %s", module_name); i_module_free (module); return NULL; }

  info = resource_info_func ();            /* Run the func to get the resource_info struct */
  i_module_free (module);  /* Close it and free it */    
  if (!info) 
  { i_printf (1, "i_resource_get_info_from_module 'resource_info' func returned null in %s", module_name); return NULL; }

  return info;

}

/* Utility Functions */

char* i_resource_info_type_string (i_resource *res)
{
  if (res->type < (int) (sizeof(static_resource_info)) / (int) (sizeof(i_resource_info)))
  { if (static_resource_info[res->type].type_str) return strdup (static_resource_info[res->type].type_str); }
  else
  { 
    i_resource_info *info; 

    info = i_resource_info_module (res, res->module_name);
    if (info->type_str)
    { 
      char *type_str;
      type_str = strdup (info->type_str);
      i_resource_info_free (info);
      return type_str;
    }
  }

  return NULL;
}
