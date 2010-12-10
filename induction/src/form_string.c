#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "form.h"

/** \addtogroup form_item Form Items
 * @ingroup form
 * @{
 */

i_form_item* i_form_string_add (i_form *form, char *name, char *label, char *value)   /* Name is the identifier, str is the string */
{
  i_form_item *item;

  if (!name || !label || !form) return NULL;

  item = i_form_create_item (FORM_ITEM_STRING, name);
  if (!item) { i_printf (1, "i_form_add_string unable to create item"); return NULL; }
        
  i_form_item_add_option (item, ITEM_OPTION_LABEL, 0, 0, 0, label, (strlen(label)+1));
  if (value) i_form_item_add_option (item, ITEM_OPTION_VALUE, 0, 0, 0, value, (strlen(value)+1));

  i_form_add_item (form, item);

  return item;
}

int i_form_string_set_fgcolor (i_form_item *item, char *label_color_str, char *value_color_str)
{ 
  if (label_color_str)
  { i_form_item_add_option (item, ITEM_OPTION_FGCOLOR, 0, 0, 0, label_color_str, (strlen(label_color_str)+1)); }
  if (value_color_str)
  { i_form_item_add_option (item, ITEM_OPTION_FGCOLOR, 0, 1, 0, value_color_str, (strlen(value_color_str)+1)); }
  return 0;
}

int i_form_string_set_bgcolor (i_form_item *item, char *label_color_str, char *value_color_str)
{ 
  if (label_color_str)
  { i_form_item_add_option (item, ITEM_OPTION_BGCOLOR, 0, 0, 0, label_color_str, (strlen(label_color_str)+1)); }
  if (value_color_str)
  { i_form_item_add_option (item, ITEM_OPTION_BGCOLOR, 0, 1, 0, value_color_str, (strlen(value_color_str)+1)); }
  return 0;
}


/* @} */
