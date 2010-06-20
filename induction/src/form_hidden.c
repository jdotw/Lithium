#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "form.h"

/** \addtogroup form_item Form Items
 * @ingroup form
 * @{
 */

i_form_item* i_form_hidden_add (i_form *form, char *name, char *value)   /* Name is the identifier, str is the string */
{
  i_form_item *item;

  if (!name || !form) return NULL;
  if (!value) value = "";

  item = i_form_create_item (FORM_ITEM_HIDDEN, name);
  if (!item) { i_printf (1, "i_form_add_string unable to create item"); return NULL; }
        
  i_form_item_add_option (item, ITEM_OPTION_VALUE, 0, 0, 0, value, (strlen(value)+1));

  i_form_add_item (form, item);

  return item;
}

/* @} */
