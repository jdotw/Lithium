#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/hierarchy.h>

#include "demorego.h"

int form_demorego_main (i_resource *self, i_form_reqdata *reqdata)
{
  /* Start frame */
  i_form_frame_start (reqdata->form_out, "demorego", "FREE Lithium 30-Day Trial License");

  i_form_item *item = i_form_string_add (reqdata->form_out, "cat_list", "Register Now", "Register now to receive a Free Lithium 30-Day Trial License. Registration is quick, easy and immediately allows you to trial Lithium for 30-Days monitoring as many devices as you like. You can choose not to register, but will only be able to monitor three (3) devices.");
  item = i_form_string_add (reqdata->form_out, "cat_list", "", "Click Here for Free 30-Day Trial License");
  i_form_item_add_link (item, 0, 0, 0, RES_ADDR(self), NULL, "demorego_register", 0, NULL, 0);

  /* End main frame */
  i_form_frame_end (reqdata->form_out, "demorego");
  
  /* Finished */
  return 1;
}


