#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/module.h>
#include <induction/auth.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/construct.h>
#include <induction/list.h>

#include "customer.h"

int form_main (i_resource *self, i_form_reqdata *reqdata)
{ return form_customer_list (self, reqdata); }
