#include <stdio.h>
#include <stdlib.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/list.h>

extern i_hashtable *global_res_table;
extern i_list *global_spawn_schedule_list;

void module_fork_clean_up (i_resource *parent, i_resource *resource)                        /* Called when a process is forked to clean up instance specific data */
{
  /* Free global resource table */
  if (global_res_table)
  {
    /* Remove self (safely, i.e without freeing self) from table 
     *
     * NOTE : self will only be in the hashtable is this resource has been
     * spawned before. E.g. this is a restart of a resource. The first time
     * the resource is started it wont be in the res_table yet. However, its
     * important we attempt to safely remove it from the table just incase.
     */

    if (i_resource_local_get (global_res_table, RES_ADDR(resource)))
    {
      i_hashtable_set_destructor (global_res_table, NULL);
      i_resource_local_remove (resource, global_res_table, RES_ADDR(resource));
      i_hashtable_set_destructor (global_res_table, i_resource_free);
    }

    /* Free hashtable */
    i_hashtable_free (global_res_table);    /* Destructor will free the resources */
    global_res_table = NULL;
  }

  /* Free global_spawn_schedule_list */
  if (global_spawn_schedule_list)
  { i_list_free (global_spawn_schedule_list); global_spawn_schedule_list = NULL; }
}

