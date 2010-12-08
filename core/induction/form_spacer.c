#include <stdlib.h>

#include "induction.h"
#include "form.h"

/** \addtogroup form_item Form Items
 * @ingroup form
 * @{
 */

i_form_item* i_form_spacer_add (i_form *form)
{
  i_form_item *item;

  if (!form) return NULL;

  item = i_form_create_item (FORM_ITEM_SPACER, "spacer");
  if (!item) { i_printf (1, "i_form_spacer_add unable to create item"); return NULL; }

  i_form_add_item (form, item);

  return item;
}

/* @} */
