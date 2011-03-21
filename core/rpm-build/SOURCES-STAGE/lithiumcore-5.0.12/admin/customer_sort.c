#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>

#include "customer.h"

int l_customer_sort_name (void *curptr, void *nextptr)
{
  i_customer *cur = curptr;
  i_customer *next = nextptr;

  if (!cur || !cur->name_str || !next || !next->name_str)
  { return 1; }

  if (strcmp(cur->name_str, next->name_str) > 0)
  { return 1; }

  return 0;
}

