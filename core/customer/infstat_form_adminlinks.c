#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/postgresql.h>
#include <induction/list.h>

#include "infstat.h"

int l_infstat_form_adminlinks (i_resource *self, i_form_reqdata *reqdata)
{
  i_form_item *item;

  if (!reqdata || !reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return 0; }

  i_form_frame_start (reqdata->form_out, "infstat_admin_frame", "Infrastructure Status Administration");

  if (l_infstat_cat_load_inprogress() == 0)
  {
    item = i_form_string_add (reqdata->form_out, "cat_add", "Add New Category", NULL);
    i_form_item_add_link (item, 0, 0, 0, RES_ADDR(self), NULL, "infstat_cat_edit", 0, NULL, 0);
  }
  else 
  { i_form_string_add (reqdata->form_out, "msg", "Note", "Category Administration features will be active once the category list has been loaded"); }

  i_form_frame_end (reqdata->form_out, "infstat_admin_frame");

  return 0;
}
