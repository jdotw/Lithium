#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <libxml/parser.h>

#include "induction.h"
#include "list.h"
#include "module.h"
#include "construct.h"
#include "xml.h"

/** \addtogroup xml XML Functions
 * @{
 */

void* i_xml_func_get (i_resource *self, char *xml_name_raw, int *resultptr)
{
  int (*xml_func) (i_resource *self, i_xml_request *reqdata);
  const char *errstr;
  char *xml_name;

  asprintf (&xml_name, "xml_%s", xml_name_raw);  
    
  xml_func = dlsym (self->construct->module->handle, xml_name);      /* Check the construct's loaded module */
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
      xml_func = dlsym (module->handle, xml_name);
      errstr = dlerror();
      if (!errstr) break;     /* Found it, end the loop */
    }

    if (errstr)
    {
      /* Last resort, try local handle */
      void *handle;
      handle = dlopen (NULL, RTLD_LAZY);
      xml_func = dlsym (handle, xml_name);
      errstr = dlerror();
    }
  }

  free (xml_name);

  if (errstr)       /* Checked again incase the above code found the symbol */
  {
    /* Still Failed */
    *resultptr = -1;
    return NULL;
  }

  *resultptr = 0;
  return xml_func;
}

/* @} */
