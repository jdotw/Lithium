#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>

#include "induction.h"
#include "form.h"
#include "list.h"
#include "module.h"
#include "construct.h"

/** \addtogroup form Web Forms
 * @{
 */

void* i_form_func_get (i_resource *self, char *form_name_raw, int *resultptr)
{
  int (*form_func) (i_resource *self, i_form_reqdata *reqdata);
  const char *errstr;
  char *form_name;

  asprintf (&form_name, "form_%s", form_name_raw);  
    
  form_func = dlsym (self->construct->module->handle, form_name);      /* Check the construct's loaded module */
  errstr = dlerror ();
  if (errstr)
  {
    /* Try the following
     * - 1. All loaded modules 
     * - 2. Local handle
     */

    i_list *list;
    i_module *module;

    list = i_module_list ();
    for (i_list_move_head(list); (module=i_list_restore(list))!=NULL; i_list_move_next(list))
    {
      /* Try each loaded module */
      form_func = dlsym (module->handle, form_name);
      errstr = dlerror();
      if (!errstr) break;     /* Found it, end the loop */
    }

    if (errstr)
    {
      /* Last resort, try local handle */
      void *handle;
      handle = dlopen (NULL, RTLD_LAZY);
      form_func = dlsym (handle, form_name);
      errstr = dlerror();
    }
  }

  free (form_name);

  if (errstr)       /* Checked again incase the above code found the symbol */
  {
    /* Still Failed */
    *resultptr = -1;
    return NULL;
  }

  *resultptr = 0;
  return form_func;
}

/* @} */
