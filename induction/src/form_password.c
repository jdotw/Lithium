#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "induction.h"
#include "form.h"

/** \addtogroup form_item Form Items
 * @ingroup form
 * @{
 */

i_form_item* i_form_password_add (i_form *form, char *name, char *label, char *value)   /* Name is the identifier, label is the text string show, value is the value in the entry box */
{
  i_form_item *item;

  if (!form || !name || !label) return NULL;
  if (!value) value = "";

  item = i_form_create_item (FORM_ITEM_PASSWORD, name);
  if (!item) { i_printf (1, "i_form_add_password unable to create item"); return NULL; }
  
  i_form_item_add_option (item, ITEM_OPTION_LABEL, 0, 0, 0, label, (strlen(label)+1));
  i_form_item_add_option (item, ITEM_OPTION_VALUE, 0, 0, 0, value, (strlen(value)+1));
  
  i_form_add_item (form, item);

  return item;
}

/* @} */
