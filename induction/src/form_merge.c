#include <stdlib.h>

#include "induction.h"
#include "form.h"
#include "list.h"

/** \addtogroup form Web Forms
 * @{
 */

int i_form_merge (i_form *base, i_form *overlay)
{
  /* Take the 'base' form and overlay all items
   * present in the overlay form. If an item in the overlay form
   * is _not_ present in the base it is discarded.
   */

  i_form_item *item;

  if (!base || !overlay) return -1;

  /* Handle form items */

  for (i_list_move_head(overlay->items); (item=i_list_restore(overlay->items))!=NULL; i_list_move_next(overlay->items))
  {
    /* Loop through each item in the overlay */
    i_form_item *base_item;

    base_item = i_form_find_item (base, item->name);
    if (base_item && base_item->type == item->type)
    {
      i_form_item_option *opt;
      
      for (i_list_move_head(item->options); (opt=i_list_restore(item->options))!=NULL; i_list_move_next(item->options))
      {
        i_form_item_option *base_opt;
        
        /* Generic VALUE item_option handling */

        base_opt = i_form_item_find_option (base_item, opt->type, opt->i, opt->x, opt->y);
        if (base_opt)
        {
          /* Matching item option in base form */
  
          if (opt->type == ITEM_OPTION_VALUE && item->type != FORM_ITEM_DROPDOWN)
          {
            /* Remove the old option, add the new */
            i_form_item_delete_option (base_item, opt->type, opt->i, opt->x, opt->y);
            i_form_item_add_option (base_item, opt->type, opt->i, opt->x, opt->y, opt->data, opt->datasize);
          }

        }

        /* Handle drop-down selection */

        if (opt->type == ITEM_OPTION_SELECTED && item->type == FORM_ITEM_DROPDOWN)
        {
          void *p;
          i_form_item_option *selected_value;

          p = item->options->p;
          selected_value = i_form_item_find_option (item, ITEM_OPTION_VALUE, opt->i, 0, 0);
          item->options->p = p;
          if (selected_value)
          { i_form_dropdown_set_selected (base_item, (char *)selected_value->data); }
        }

        /* End of item_option handling */
      }

      /* End of item handling */
    }
  }

  /* Finished */

  return 0;
}
/* @} */
