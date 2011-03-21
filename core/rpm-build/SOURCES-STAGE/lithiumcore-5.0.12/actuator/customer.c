#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/entity.h>
#include <induction/form.h>

#include "customer.h"
#include "shutdown.h"

int a_customer_get_addr_by_id_str (i_resource *self, int argc, char *argv[], int optind)
{
  char *customer_id = argv[optind+2];

  if (!self || !customer_id) return -1;

  /* Special handling for admin */
  if (!strcmp(customer_id, "admin"))
  { printf ("::%i:0:", RES_ADMIN); return 0; }

  /* Standard handling */
  printf ("::%i:0:%s\n", RES_CUSTOMER, customer_id);

  return 0;   /* Finished */
}

