#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "form.h"

/** \addtogroup form Web Forms
 * @{
 */

i_form* i_form_create_denied ()
{
  i_form *form;
  char *form_title = "Authentication Message";

  form = i_form_create (NULL, 0);
  if (!form)
  {
    i_printf (1, "i_form_create_denied failed to create form");
    return NULL;
  }

  i_form_option_add (form, FORM_OPTION_TITLE, form_title, strlen(form_title)+1);

  i_form_string_add (form, "error", "Authentication", "Access Denied");

  return form;
}

/* @} */
