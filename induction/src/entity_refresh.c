#include <stdlib.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "hierarchy.h"
#include "cement.h"
#include "device.h"
#include "callback.h"
#include "entity.h"

/** \addtogroup entity_refresh Entity Refresh
 * @ingroup entity
 * @{
 */

extern i_resource *global_self;

/* Entity Refresh Functions 
 *
 * These functions are used by other functions to
 * manipulation the refresh process for entities
 */

int i_entity_refresh (i_resource *self, i_entity *ent, int flags, int (*cbfunc) (), void *passdata)
{
  /* Begin the refresh process for the specified
   * entity. This function will handle collision. 
   *
   * The ent->refresh_func will be used for all operations
   *
   * The function returns 0 if a refresh operation
   * has been started and the callback (if specified) will
   * be executed at the conclusion of the refresh.
   *
   * The function returns 1 if the refresh operation
   * is already concluded and hence the callback will
   * not be executed.
   *
   * The function returns -1 if a refresh operation
   * failed to be called. 
   */
  
  int num;

  /*
   * Admin state handling
   */
  if ((ent->adminstate & ENTADMIN_ALL) == ENTADMIN_DISABLED)
  {
    /* Entity is disabled, return refresh complete */
    ent->refresh_result = REFRESULT_OK;
    return 1;
  }

  /* 
   * Collision Checking and Handling
   */

  if ((ent->refresh_state & REFSTATE_BUSY) == REFSTATE_BUSY)
  {
    /* Refresh is in progress - Handle the collission */
    i_entity_refresh_collision (self, ent);
  }

  /* New Refresh Handling
   *
   * If at this point the refresh_state is REFSTATE_IDLE,
   * a new refresh operation will be started by calling
   * ent->refresh_func with the REFOP_REFRESH op.
   * The REFSTATE_IDLE state may be present either by virtue of no 
   * refresh having been in progress or the previous refresh being 
   * terminated for excess collisions earlier in this func
   */

  if (ent->refresh_state == REFSTATE_IDLE)
  {
    /* Set time stamp */
    gettimeofday (&ent->refresh_tstamp, NULL);

    /* Attempt to call refresh_func */
    if (ent->refresh_func)
    { 
      /* Call the refresh_func */
      num = ent->refresh_func (self, ent, REFOP_REFRESH|flags);
      if (num == 0)
      {
        /* Refresh started and is in progress */
        ent->refresh_state = REFSTATE_BUSY|flags;
        ent->refresh_colls = 0;
      }
      else if (num == 1)
      {
        /* Refresh started and is already finished 
         * Call i_entity_refresh_terminate to signify
         * that the refresh has been completed and then 
         * return 1 to the calling func to indicate
         * that the refresh is already completed and that
         * the specified callback (if specified) will
         * not be called.
         */
        i_entity_refresh_terminate (ent);
        ent->refresh_state = REFSTATE_IDLE;
        ent->refresh_colls = 0;

        return 1;
      }
      else
      { 
        i_printf (2, "i_entity_refresh failed to call ent->refresh_func for %s %s",
          i_entity_typestr (ent->ent_type), ent->name_str);
        ent->refresh_result = REFRESULT_TOTAL_FAIL;
        ent->refresh_state = REFSTATE_IDLE;
        ent->refresh_colls = 0;
        return -1;
      }
    } 
    else
    { 
      /* No refresh func, hence the refresh op
       * is already complete and the func should
       * return 1 to the caller 
       */
      i_printf (2, "i_entity_refresh failed %s %s has no ent->refresh_func set", 
        i_entity_typestr (ent->ent_type), ent->name_str); 
      ent->refresh_result = REFRESULT_TOTAL_FAIL;
      ent->refresh_state = REFSTATE_IDLE;
      ent->refresh_colls = 0;

      return 1;
    }
  }

  /* 
   * Callback Handling
   *
   * A callback is always added, whether a collision 
   * has occurred or not. This way, whenever the i_entity_refresh func
   * is called with a cbfunc specified and i_entity_refresh returns 0, 
   * that callback can be assured of 1-to-1 calling of the cbfunc for 
   * each call to i_entity_refresh func.
   */
  
  if (cbfunc)
  { i_entity_refreshcb_add (ent, cbfunc, passdata); }

  return 0;
}

/* Timer callback utility
 *
 * Used as a generic timer callback func for entities
 * who have their method set to REFMETHOD_STANDALONE.
 * The func simply calls i_entity_refresh for the specified 
 * entity in passdata with the REFFLAG_AUTO flag set and
 * does not add any refresh callback.
 */

int i_entity_refresh_timercb (i_resource *self, i_timer *timer, void *passdata)
{
  /* Generic timer callback that can be used to
   * request an automatic refresh of an entity
   *
   * ALWAYS RETURN 0 - By default the timer should be persistent
   */

  int num;
  i_entity *ent = passdata;

  num = i_entity_refresh (self, ent, REFFLAG_AUTO, NULL, NULL);
  if (num == -1)
  {
    i_printf (2, "i_entity_refresh_timercb failed to call i_entity_refresh for %s %s",
      i_entity_typestr (ent->ent_type), ent->name_str);
  }

  return 0;
}

/* Refresh Termination
 *
 * Used to terminate a refresh operation on an entity.
 * This func calls ent->refresh_func with the REFOP_TERMINATE
 * op code. The entities refresh state/variables are also
 * reset by this func.
 */

int i_entity_refresh_terminate (i_entity *ent)
{
  /* Terminate the current refresh operation 
   * for the specified entity
   */

  int num;

  /* Check for collission-forced term */
  if (ent->refresh_colls >= ent->refresh_maxcolls)
  {
    ent->refresh_forcedterm = 1;
  }
  else
  {
    ent->refresh_forcedterm = 0;
  }

  /* Attempt to call ent->refresh_func */
  if (ent->refresh_func)
  { 
    num = ent->refresh_func (global_self, ent, REFOP_TERMINATE);
    if (num != 0)
    { 
      /* Failed to perform REFOP_TERMINATE */
      i_printf (1, "i_entity_refresh_terminate warning, failed to call ent->refresh_func for %s %s",
        i_entity_typestr (ent->ent_type), ent->name_str); 
    }
  }
  else
  { 
    /* No refresh_func */
    i_printf (2, "i_entity_refresh_terminate warning, %s %s has no ent->refresh_func set", 
      i_entity_typestr (ent->ent_type), ent->name_str); 
  }

  /* Set state/variables */
  ent->refresh_state = REFSTATE_IDLE;
  ent->refresh_colls = 0;

  /* Execute entities refresh callbacks */
  num = i_entity_refreshcb_execlist (global_self, ent);
  if (num != 0)
  { 
    i_printf (1, "i_entity_refresh_terminate warning, failed to exec callbacks for %s %s", 
      i_entity_typestr (ent->ent_type), ent->name_str);
  }

  return 0;
}

/* Refresh Data Cleanup 
 *
 * Called just before an entity is freed if the
 * ent->refresh_data pointer is non-NULL. This
 * func calls ent->refresh_func with the REFOP_CLEANDATA
 * op code. 
 */

int i_entity_refresh_cleandata (i_entity *ent)
{
  /* Free any residual refresh data from the entity */
  int num;

  /* Attempt to call ent->refresh_func */
  if (ent->refresh_func)
  {
    num = ent->refresh_func (global_self, ent, REFOP_CLEANDATA);
    if (num != 0)
    {
      /* Failed to perform REFOP_CLEANDATA */
      i_printf (1, "i_entity_refresh_cleandata warning, failed to call ent->refresh_func for %s %s",
        i_entity_typestr (ent->ent_type), ent->name_str);
    }
  }
  else
  { 
    /* No refresh_func */
    i_printf (1, "i_entity_refresh_cleandata warning, %s %s has no ent->refresh_func set", 
      i_entity_typestr (ent->ent_type), ent->name_str); 
  }

  /* NULLify refresh_data pointer */
  ent->refresh_data = NULL;

  return 0;
}

/* Refresh Collision Handling
 *
 * This func compares the value of the refresh_colls and
 * refresh_maxcolls variables. If colls < maxcolls, the 
 * collision is considered non fatal and ent->refresh_func 
 * will be called with a REFOP_COLLISION operation. Should the
 * func return -1, the refresh op will be terminated.
 *
 * If colls >= maxcolls, the collision is considered fatal and 
 * the current refresh operation will be terminated.
 */

int i_entity_refresh_collision (i_resource *self, i_entity *ent)
{
  int num;

  /* Increment counter */
  ent->refresh_colls++;

  /* Check counter and max variables */
  if (ent->refresh_colls < ent->refresh_maxcolls)
  {
    /* Maximum count of collisions HAS NOT yet been reached.
     * Call the i_entity_refresh_collision to handle the collision
     */
    if (ent->refresh_func)
    {
      num = ent->refresh_func (self, ent, REFOP_COLLISION);
      if (num != 0)
      {
        /* Collision handling failed, terminate the refresh now */
        num = i_entity_refresh_terminate (ent);
        if (num != 0)
        {
          /* Failed to handle collision and failed to terminate */
          i_printf (1, "i_entity_refresh warning, failed to terminate %s %s refresh after collision handling failed",
            i_entity_typestr (ent->ent_type), ent->name_str);
        }
      }
    }
    else
    {
      /* No refresh func */
      i_printf (1, "i_entity_refresh warning, no refresh_func set for %s %s while handling a collision",
        i_entity_typestr (ent->ent_type), ent->name_str);
    }

    /* End handling of non-fatal collision */
  }
  else
  {
    /* The maximum count of collissions have occurred.
     * I.e the maximum allowed instances of a refresh being
     * requested whilst an existing refresh is in progress
     * has been reached
     *
     * Cancel the current refresh operation
     */

    i_printf (1, "i_entity_refresh cancelling refresh of %i:%s:%s due to too many collissions (stuck -- consider increasing the refresh interval.)", ent->ent_type, ent->parent->desc_str, ent->desc_str);

    ent->refresh_result = REFRESULT_PARTIAL_FAIL;

    if (ent->ent_type == 3) 
    {
      i_device *dev = (i_device *)ent;
      if (!dev->interval_warning_given)
      { i_printf (0, "i_entity_refresh warning, device refresh operation cancelled due to taking too long. Consider increasing the refresh interval."); }
    }
    num = i_entity_refresh_terminate (ent);
    if (num != 0)
    {
      i_printf (1, "i_entity_refresh warning, failed to terminate %s %s refresh due to excessive refresh collisions",
        i_entity_typestr (ent->ent_type), ent->name_str);
    }
  }

  return 0;
}

/*
 * String Utilities 
 */

char* i_entity_refresh_resultstr (unsigned short result)
{
  switch (result)
  {
    case REFRESULT_OK: return "Successful";
    case REFRESULT_PARTIAL_FAIL: return "Partial Failure";
    case REFRESULT_TOTAL_FAIL: return "Total Failure";
    case REFRESULT_TIMEOUT: return "Timeout";
    default: return "Unknown";
  }

  return NULL;
}

/* @} */
