#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>

#include "induction.h"
#include "module.h"
#include "list.h"
#include "construct.h"
#include "path.h"

static i_list *static_module_list = NULL;

i_list* i_module_list ()
{ return static_module_list; }

int i_module_check_construct (i_construct *construct)	/* Check the constructs module exists, and perform sanity check */
{
  char *path;
  const char *errorstr;
  void *handle;
  int (*sanity_check) (i_construct *construct);

  /* Open module */
  path = i_path_glue (construct->module_path, construct->module_name);
  handle = dlopen (path, RTLD_LAZY);
  free (path);
  if (!handle) 
  { i_printf (1, "i_module_check_construct failed to open %s (%s)", construct->module_name, dlerror()); return -1; }

  /* Find and run sanity check func */
  sanity_check = dlsym (handle, "module_sanity_check");
  errorstr = dlerror ();
  if (!errorstr) 
  {
    if ((sanity_check (construct)) == -1)
    { i_printf (1, "i_module_check_construct sanity check failed for module %s", construct->module_name); 
      dlclose (handle); return -1; }
    else { i_printf (2, "i_module_check_construct sanity check passed for %s", construct->module_name); }
  }

  /* Close module */
  dlclose (handle);

  return 0;	
}

i_module* i_module_create ()
{
  i_module *module;

  module = (i_module *) malloc (sizeof(i_module));
  if (!module) return NULL;
  memset (module, 0, sizeof(i_module));
  module->flags = RTLD_LAZY|RTLD_GLOBAL;

  return module;
}

void i_module_free (i_module *module)
{
  int num;

  if (static_module_list)
  {
    num = i_list_search (static_module_list, module);
    if (num == 0)
    { i_list_delete (static_module_list); }
    if (static_module_list->size < 1)
    { i_list_free (static_module_list); static_module_list = NULL; }
  }

  if (!module) return;
  if (module->name) free (module->name);
  if (module->file) free (module->file);
  if (module->handle) num = dlclose (module->handle);

  free (module);
}

i_module* i_module_open (char *module_path, char *module_name)
{
  int num;
  const char *error_str;
  i_module *module;

  if (!module_name) 
  { i_printf (1, "i_module_open called with !module_name. failed!"); return NULL; }

  module = i_module_create ();
  if (!module) 
  { i_printf (1, "i_module_open unable to create module"); return NULL; }

  module->name = strdup (module_name);
  module->file = i_path_glue (module_path, module_name);
  if (!module->file) 
  { i_printf (1, "i_module_open unable to glue module_path"); i_module_free (module); return NULL; }

  module->handle = dlopen (module->file, module->flags);
  if (!module->handle) 
  { i_printf (1, "i_module_open unable to open module %s (%s)", module->file, dlerror()); i_module_free (module); return NULL; }

  module->init = dlsym (module->handle, "module_init");
  error_str = dlerror ();
  if (error_str) 
  { i_printf (1, "i_module_open module %s does not contain module_init()", module_name); i_module_free (module); return NULL; }

  module->shutdown = dlsym (module->handle, "module_shutdown");
  error_str = dlerror ();
  if (error_str) 
  { i_printf (1, "i_module_open module %s does not contain module_shutdown()", module_name); i_module_free (module); return NULL; }

  module->entry = dlsym (module->handle, "module_entry");
  error_str = dlerror ();
  if (error_str) 
  { i_printf (1, "i_module_open module %s does not contain module_entry()", module_name); i_module_free (module); return NULL; }

  if (!static_module_list)
  { static_module_list = i_list_create (); }
  num = i_list_enqueue (static_module_list, module);
  if (num != 0)
  { i_printf (1, "i_module_open failed to enqueue module %s in module_list", module_name); }

  return module;	
}
