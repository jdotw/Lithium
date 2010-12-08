#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/postgresql.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/site.h>
#include <induction/path.h>
#include <induction/customer.h>
#include <induction/hierarchy.h>

#include "lic.h"
#include "infstat.h"
#include "demorego.h"
#include "site.h"
#include "device.h"

int form_main (i_resource *self, i_form_reqdata *reqdata)
{
  i_entity *ent;
  
  if (!self || !reqdata) return -1;

  /* Check for entity */
  if (reqdata->entaddr)
  {
    /* Get entity */
    ent = i_entity_local_get (self, reqdata->entaddr);
    if (!ent)
    { return -1; }
  }
  else
  { 
    /* No entity = customer */
    ent = ENTITY(self->hierarchy->cust); 
  }

  /* Determine page to display */
  switch (ent->ent_type)
  {
    case ENT_CUSTOMER:
      /* Customer entity, display infstat or site list */
      if (l_infstat_enabled() == 1)
      { return form_infstat_main (self, reqdata); }
      else
      { return form_site_list (self, reqdata); }
      break;
    case ENT_SITE:
      /* Site entity, display device list */
      return form_device_list (self, reqdata);
      break;
  }

  return -1;
}

