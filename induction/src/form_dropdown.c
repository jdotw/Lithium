#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "induction.h"
#include "form.h"
#include "list.h"

/** \addtogroup form_item Form Items
 * @ingroup form
 * @{
 */

/* FIX: The way things are marked as selected is very inefficient */

i_form_item* i_form_dropdown_create (char *name, char *label)
{
  i_form_item *item;

  item = i_form_create_item (FORM_ITEM_DROPDOWN, name);
  if (!item) { i_printf (1, "i_form_dropdown_create unable to create item"); return NULL; }

  i_form_item_add_option (item, ITEM_OPTION_LABEL, 0, 0, 0, label, strlen(label)+1);

  return item;
}

void i_form_dropdown_add_option (i_form_item *item, char *value, char *label, int selected)
{
  i_form_item_option *opt;

  if (!item || !value || !label) 
  { i_printf (1, "i_form_dropdown_add_option not enough params passed"); return; }
  
  opt = i_form_item_append_option (item, ITEM_OPTION_LABEL, 0, 0, label, strlen(label)+1);            /* Append a ITEM_OPTION_VALUE option (i.e calculate the 'i' int automatically) */
  i_form_item_add_option (item, ITEM_OPTION_VALUE, opt->i, 0, 0, value, strlen(value)+1);
  if (selected == 1) i_form_item_add_option (item, ITEM_OPTION_SELECTED, opt->i, 0, 0, NULL, 0);
}

int i_form_dropdown_set_selected (i_form_item *item, char *value)
{
  i_form_item_option *opt;

  /* First find the item which is to be marked as selected */
  
  for (i_list_move_head(item->options); (opt=i_list_restore(item->options)) != NULL; i_list_move_next(item->options))
  {
    /* Loop through all options for item */ 
    if (opt->type == ITEM_OPTION_VALUE && !strcmp((char *) opt->data, value)) break;  /* if this is the value to be selected, break the loop */
  }

  /* Then adjust any existing selected markers or create a new one */
  
  if (opt) 
  {
    /* The item to be selected was found in the above for loop */

    i_form_item_option *selected_opt;

    for (i_list_move_head(item->options); (selected_opt=i_list_restore(item->options)) != NULL; i_list_move_next(item->options))
    {
      if (selected_opt->type == ITEM_OPTION_SELECTED) { selected_opt->i = opt->i; break; }        /* Change the selected item numer */
    }

    if (!selected_opt) i_form_item_add_option (item, ITEM_OPTION_SELECTED, opt->i, 0, 0, NULL, 0);  /* Nothing existing was selected, hence we add a new selected marker */
  } 
  else return -1; /* Opt not found */

  return 0;
}

i_form_item_option* i_form_dropdown_get_selected_value (i_form_item *item)
{
  i_form_item_option *selected_opt;   /* The opt that indicates which item it selected */
  i_form_item_option *opt;

  /* Find the selected marker */
  
  for (i_list_move_head(item->options); (selected_opt=i_list_restore(item->options)) != NULL; i_list_move_next(item->options)) 
  {
    if (selected_opt->type == ITEM_OPTION_SELECTED) break;
  }

  if (!selected_opt) 
  { return NULL;  }

  opt = i_form_item_find_option (item, ITEM_OPTION_VALUE, selected_opt->i, 0, 0);

  return opt;
}

/* @} */
