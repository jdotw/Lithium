#include <stdio.h>
#include <stdlib.h>

#include <induction.h>
#include <induction/postgresql.h>

int l_customersql_create (i_resource *self, char *id_str)
{
  /* Create the SQL database for a customer */
  int num;
  char *dbname;

  asprintf (&dbname, "customer_%s", id_str);
  num = i_pg_create_db (self, dbname);
  free (dbname);
  if (num != 0)
  { i_printf (1, "l_customersql_create failed to create SQL database for customer %s", id_str); return -1; }

  return 0;
}

int l_customersql_drop (i_resource *self, char *id_str)
{
  /* Drop the SQL database for a customer */
  int num;
  char *dbname;
  
  asprintf (&dbname, "customer_%s", id_str);
  num = i_pg_drop_db (self, dbname);
  free (dbname);
  if (num != 0)
  { i_printf (1, "l_customersql_drop failed to drop SQL database for customer %s", id_str); return -1; }

  return 0; 
}
