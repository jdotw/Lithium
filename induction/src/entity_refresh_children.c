#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "timeutil.h"
#include "hierarchy.h"
#include "cement.h"
#include "callback.h"
#include "entity.h"

/** \addtogroup entity_refresh Entity Refresh
 * @ingroup entity
 * @{
 */

extern i_resource *global_self;

/*
 * Refresh function for entitys with children
 * (Device/Container/Object)
 *
 * Calls the i_entity_refresh function for all
 * child entities that have their refresh method set
 * to REFMETHOD_PARENT
 */

/* Refresh Data Struct manipulation */

i_entity_refresh_data* i_entity_refresh_data_create ()
{
  i_entity_refresh_data *data;

  data = (i_entity_refresh_data *) malloc (sizeof(i_entity_refresh_data));
  if (!data)
  { i_printf (1, "i_entity_refresh_data_create failed to malloc i_entity_refresh_data struct"); return NULL; }
  memset (data, 0, sizeof(i_entity_refresh_data));

  return data;
}

void i_entity_refresh_data_free (void *dataptr)
{
  i_entity_refresh_data *data = dataptr;

  if (!data) return;

  if (data->child_list) i_list_free (data->child_list);

  free (data);
}

/* Entity Refresh Function */

int i_entity_refresh_children (i_resource *self, i_entity *ent, int opcode)
{
  /* Iterate through all child entities
   * and begin the refresh process for each that
   * has its refresh method set to RMETHOD_PARENT
   */

  int num;
  int refsuccess_count = 0;
  i_entity *child;
  i_entity_refresh_data *data = ent->refresh_data;

  /* Examine/Handle the op code */
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:                                 /* Refresh the entity */
      
      /* Create refresh data struct 
       * Because the vars in the data struct are
       * used by REFOP_TERMINATE to determine the
       * result of the refresh operation, the struct should
       * always be added to the entity when the func retrns either
       * 1 (nothing to do - refresh complete) or 0 (refresh
       * in progress)
       */
      data = i_entity_refresh_data_create ();
      if (!data)
      { i_printf (1, "i_entity_refresh_children failed to create refresh data"); return -1; }
      gettimeofday (&data->start_tv, NULL);
      data->opcode = opcode;
      if (ent->refresh_data)
      { i_printf (1, "i_entity_refresh_children warning, ent->refresh_data already present for %s. it will be leaked! Contact support@lithiumcorp.com", ent->name_str); }
      ent->refresh_data = data;

      /* Create list of entities to refresh */
      data->child_list = i_list_create ();
      for (i_list_move_head(ent->child_list); (child=i_list_restore(ent->child_list))!=NULL; i_list_move_next(ent->child_list))
      {
        if ((child->refresh_method & REFMETHOD_ALL) == REFMETHOD_PARENT)
        {
          num = i_list_enqueue (data->child_list, child);
          if (num != 0)
          { i_printf (1, "i_entity_refresh_children warning, failed to enqueue %s %s for refresh", i_entity_typestr (ent->ent_type), ent->name_str); }
        }
      }

      /* Check list size. If no list or an empty
       * list is present, return 1 to signal that
       * the refresh is already complete. REFOP_TERMINATE
       * will then be called by i_entity_refresh which will
       * calculate the refresh result using the ent->refresh_data
       * and free the ent->refresh_data struct
       */
      if (!data->child_list || data->child_list->size == 0)
      { 
        /* Nothing to refresh, return 1
         * Done free data so that it can be used to
         * calculate the refresult 
         */
        return 1;
      }

      /* Sort the list by last refresh time. 
       * Entities with the oldest last refresh time (lowest time value)
       * will be at the top of the list
       */
      i_list_sort (data->child_list, i_entity_refresh_children_sortfunc);

      /* Begin refresh process */
      ent->refresh_data = data;
      for (i_list_move_head(data->child_list); (child=i_list_restore(data->child_list))!=NULL; i_list_move_next(data->child_list))
      {
        /* Call i_entity_refresh to refresh the first 
         * child entity. 
         * NOTE: i_entity_refresh will return 0 if a refresh is started
         * and the specified callback will be called when complete. It may
         * also return 1 indicating that the refresh is already complete (in
         * which case the next child should be refreshed). If it returns -1 
         * then a failure occured whilst attepting to perform the refresh
         */
        num = i_entity_refresh (self, child, opcode & REFFLAG_ALL, i_entity_refresh_children_refcb, ent);
        if (num == 0)
        { refsuccess_count++; break; }   /* Refresh in progress */
        else if (num == 1)
        { refsuccess_count++; }          /* Refresh already successfully completed */
        else
        {
          /* Failed to refresh child */ 
          i_printf (2, "i_entity_refresh_children warning, failed to call refresh %s %s", 
            i_entity_typestr (ent->ent_type), ent->name_str); 
        }
      }

      /* Check if a successful child refresh was started */
      if (refsuccess_count == 0)
      {
        i_printf (2, "i_entity_refresh_children failed to call i_entity_refresh for any child entity");
        i_entity_refresh_data_free (data);
        ent->refresh_data = NULL;
        return -1;
      }

      /* Check if a refresh is in progress 
       * It is possible that all child entities
       * have already been refreshed. If no 
       * child entities could be refreshed, the 
       * above counter checking would have found 
       * it. Hence, if the child is NULL, everything
       * is already complete and this func should 
       * return 1
       */
      if (!child)
      {
        /* All done, no refresh in progress 
         * DONT free data, so that it can
         * be used to calculate the refresult
         */
        return 1;
      }
      else
      { data->cur_child = child; }

      /* End of REFOP_REFRESH handling */
      break;

    case REFOP_COLLISION:
      /* Handle a refresh collision by calling 
       * i_entity_refresh_collision the child entity
       * currently being refreshed.
       */

      /* Call i_entity_refresh_collision for the current child */
      if (data->cur_child && (data->cur_child->refresh_state & REFSTATE_ALL) == REFSTATE_BUSY)
      { i_entity_refresh_collision (self, data->cur_child); }

      return 0;
      
    case REFOP_TERMINATE:                               /* Current refresh is to be terminate */
      /* NULLify the ent->refresh_data struct to 
       * indicate to ensure that no further refresh
       * activity can occur. Specifically, when the terminate
       * is called of the childs refresh op, the refresh op of
       * the next child will NOT begin. By NULLifying the
       * ent->refresh_data this func take 'exclusive' control 
       * of it. No other func can free it.
       */
      ent->refresh_data = NULL;
      
      /* Set refresh result */
      ent->refresh_result = i_entity_refresh_children_refresult (data);

      /* Terminate the refresh op for the child
       * currently being refreshed. Because the 
       * ent->refresh_data is now NULL, the next
       * child entity refresh will NOT begin.
       */
      if (data->cur_child && (data->cur_child->refresh_state & REFSTATE_ALL) == REFSTATE_BUSY)
      {
        i_entity_refresh_terminate (data->cur_child); 
      }

      /* Free the refresh data struct. This must be done
       * by calling i_entity_refresh_data_free becase the
       * ent->refresh_data struct is now NULL
       */
      i_entity_refresh_data_free (data);

      return 0;
      
    case REFOP_CLEANDATA:                               /* Refresh data is to be cleaned */
      /* Cleanup refresh data */
      i_entity_refresh_data_free (ent->refresh_data);
      ent->refresh_data = NULL;
      return 0;

    default:
      i_printf (1, "i_entity_refresh_children called with unknown/unsupported opcode %i", opcode); 
      return -1;
  }

  return 0;
}

/* Entity refresh callback */

int i_entity_refresh_children_refcb (i_resource *self, i_entity *child, void *passdata)
{
  /* Called when the refresh of a child entity is completed 
   *
   * ALWAYS RETURN -1 ... this callback should not be persistent 
   */

  i_entity *parent = passdata;
  i_entity_refresh_data *data = parent->refresh_data;

  /* Check child's refrest result */
  if (child->refresh_result != REFRESULT_OK)
  {
    /* FIX HANDLE ERRORS */
    i_printf (2, "i_entity_refresh_children_refcb %s %s refresh %s occurred",
      i_entity_typestr (child->ent_type), child->name_str, i_entity_refresh_resultstr (child->refresh_result)); 
    switch (child->refresh_result)
    {
      case REFRESULT_TOTAL_FAIL:
      case REFRESULT_PARTIAL_FAIL:
        /* FIX Handle a failure */
        break;
      case REFRESULT_TIMEOUT:
        /* FIX Handle a timeout */
        break;
    }
    if (parent->refresh_data)
    { data->failcount++; }
  }

  /* Refresh next container */
  i_entity_refresh_children_next (self, parent);

  return -1;
}

/* Refresh next child */

int i_entity_refresh_children_next (i_resource *self, i_entity *ent)
{
  int num;
  i_entity *next_child;
  i_entity_refresh_data *data = ent->refresh_data;

  /* Check to see if ent->refresh_data is present.
   * If it is not present, that means that no further
   * refresh activity is to be undertaken
   */
  if (ent->refresh_data == NULL)
  { return 0; }
  
  /* Refresh next child */
  i_list_move_next (data->child_list);
  next_child = i_list_restore (data->child_list);
  if (next_child)
  {
    /* Call next refresh */
    num = i_entity_refresh (self, next_child, data->opcode & REFFLAG_ALL, i_entity_refresh_children_refcb, ent);
    if (num != 0)
    {
      /* Either an error occurred (-1) , or the 
       * refresh operation is already complete (1)
       */
      if (num == -1)
      {
        /* Failed to call refresh */
        i_printf (2, "i_entity_refresh_children_next failed to refresh %s entity %s",
          i_entity_typestr (ent->ent_type), ent->name_str);
      }

      /* Use recursion to try the next entity.
       * Calling the refcb with the failure will cause the
       * proper refresh error handling to be done, and will
       * result in the refcb calling this func again
       */
      i_entity_refresh_children_refcb (self, next_child, ent);
    }
    else
    { data->cur_child = next_child; }
  }
  else
  {
    /* No more children, terminate the refresh */
    i_entity_refresh_terminate (ent);
  }

  return 0;
}

/* Remove a child from an entity's refresh op */

int i_entity_refresh_children_removechild (i_entity *parent, i_entity *child)
{
  i_entity_refresh_data *data = parent->refresh_data;

  if (data && data->child_list)
  {
    int num;
    void *p;
    
    p = data->child_list->p;
    num = i_list_search (data->child_list, child);
    if (num == 0)
    {
      /* Child found */
      i_list_delete (data->child_list);                 /* Will move to 'next' child */
      i_list_move_prev (data->child_list);              /* Move back one child to ensure the refresh order is observed */
    }
    else
    { 
      /* No child found */
      data->child_list->p = p;
    }
  }

  return 0;
}

/* Refresh result calculation */

unsigned short i_entity_refresh_children_refresult (i_entity_refresh_data *data)
{
  if (data->failcount == 0)
  {
    /* No Errors! */ 
    return REFRESULT_OK; 
  }
  
  if (data->failcount >= data->child_list->size)
  {
    /* Total failure */
    return REFRESULT_TOTAL_FAIL;
  }

  if (data->failcount < data->child_list->size)
  {
    /* Partial failure */
    return REFRESULT_PARTIAL_FAIL;
  }

  /* Should never reach this point */
  return 0;
}

/* Child List Sorting Func */

int i_entity_refresh_children_sortfunc (void *curptr, void *nextptr)
{
  i_entity *cur = curptr;
  i_entity *next = nextptr;

  return i_time_check_past (&cur->refresh_tstamp, &next->refresh_tstamp);
}

/* @} */
