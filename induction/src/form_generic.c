#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "form.h"

/** \addtogroup form_item Form Items
 * @ingroup form
 * @{
 */

void i_form_generic_add (i_form *form, char *name, void *data, int datasize)   
{
  i_form_item *item;

  if (!name || !form) return;

  item = i_form_create_item (FORM_ITEM_UNKNOWN, name);
  if (!item) { i_printf (1, "i_form_add_string unable to create item"); return; }
        
  i_form_item_add_option (item, ITEM_OPTION_VALUE, 0, 0, 0, data, datasize);

  i_form_add_item (form, item);
}

void* i_form_generic_get (i_form *form, char *name)
{
  i_form_item *item;
  i_form_item_option *opt;

  if (!form || !name) return NULL;

  item = i_form_find_item (form, name); 
  if (!item) return NULL;

  opt = i_form_item_find_option (item, ITEM_OPTION_VALUE, 0, 0, 0);
  if (!opt) return NULL; 

  return opt->data;
}

int i_form_generic_item_set_value (i_form_item *item, void *data, int datasize)
{
  i_form_item_option *opt;

  if (!item || !data) return -1;
  
  opt = i_form_item_find_option (item, ITEM_OPTION_VALUE, 0, 0, 0);
  if (!opt) return -1;

  if (opt->data) free (opt->data);
  opt->data = NULL;
  opt->datasize = 0;

  if (data && datasize > 0)
  {
    opt->data = (char *) malloc (datasize);
    memcpy (opt->data, data, datasize);
    opt->datasize = datasize;
  }

  return 0;
}

/* @} */
