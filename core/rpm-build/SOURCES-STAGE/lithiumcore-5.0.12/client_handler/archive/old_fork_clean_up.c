#include <stdio.h>
#include <stdlib.h>

#include <induction.h>

extern i_hashtable *global_client_table;
extern i_list *global_sockfd_set_list;

void module_fork_clean_up_restable_iterate (i_resource *self, i_hashtable *res_table, void *data)
{
  i_resource *res = (i_resource *) data;

  if (res != self) i_resource_free (res);    /* Free the entry in the resource table except ourselves (which is the new resource) */
}


void module_fork_clean_up (i_resource *parent, i_resource *resource)                        /* Called when a process is forked to clean up instance specific data */
{
  if (global_client_table)
  {
    i_hashtable_iterate (resource, global_client_table, module_fork_clean_up_restable_iterate);      /* Free all the entries in the table */
    i_hashtable_free (global_client_table);
    global_client_table = NULL;
  }

  if (global_sockfd_set_list) i_list_free (global_sockfd_set_list);
  global_sockfd_set_list = NULL;
}

