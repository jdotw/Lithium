#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "form.h"

/** \addtogroup form_item Form Items
 * @ingroup form
 * @{
 */

i_form_item* i_form_frame_start (i_form *form, char *name_str, char *desc_str)
{
  i_form_item *item;

  item = i_form_create_item (FORM_ITEM_FRAMESTART, name_str);
  if (!item) { i_printf (1, "i_form_frame_start failed to create item"); return NULL; }

  if (desc_str)
  { i_form_frame_setdesc (item, desc_str); }

  i_form_add_item (form, item);

  return item;
}

i_form_item* i_form_frame_end (i_form *form, char *name_str)
{
  i_form_item *item;

  item = i_form_create_item (FORM_ITEM_FRAMEEND, name_str);
  if (!item) { i_printf (1, "i_form_frame_end failed to create item"); return NULL; }
                
  i_form_add_item (form, item);

  return item;
}

int i_form_frame_setdesc (i_form_item *item, char *desc_str)
{
  i_form_item_add_option (item, ITEM_OPTION_LABEL, 0, 0, 0, desc_str, (strlen(desc_str)+1));
  return 0;
}

/* @} */
