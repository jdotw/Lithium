#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/hierarchy.h>

#include "lic.h"
#include "demorego.h"
#include "infstat.h"

/* infstat - Main form */

int form_infstat_main (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  char *frame_title;
  i_list *cat_list;
  l_infstat_cat *cat;

  /* Auth checking */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_INFSTAT)
  { return i_form_deliver_denied(self, reqdata); }

  /* Form setup */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_infstat_main failed to create form"); return -1; }

  /* Check license */
  l_lic_entitlement *lic = l_lic_static_entitlement();
  if (lic->free == 1)
  { form_demorego_main (self, reqdata); }
  
  /* Start frame */
  asprintf (&frame_title, "%s Infrastructure Status", self->hierarchy->customer_desc);
  i_form_frame_start (reqdata->form_out, "infstat_frame", frame_title);
  free (frame_title);

  /* Status information */
  cat_list = l_infstat_cat_list ();
  if (cat_list && cat_list->size > 0)
  {
    for (i_list_move_head(cat_list); (cat=i_list_restore(cat_list))!=NULL; i_list_move_next(cat_list))
    {
      char *err_str;
      num = l_infstat_cat_formsection (self, reqdata, cat);
      if (num != 0)
      {
        asprintf (&err_str, "Failed to display category '%s'", cat->desc_str); 
        i_form_string_add (reqdata->form_out, "cat_error", "", err_str);
        free (err_str);
      }
    }
  }
  else
  { i_form_string_add (reqdata->form_out, "cat_list", "Category List", "No infrastructure categories are currently configured"); }

  /* End main frame */
  i_form_frame_end (reqdata->form_out, "infstat_frame");
  
  /* Administrative links */
  l_infstat_form_adminlinks (self, reqdata);

  /* Finished */
  return 1;
}


