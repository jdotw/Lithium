#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include "induction.h"
#include "timer.h"
#include "loop.h"
#include "list.h"
#include "hashtable.h"
#include "timeutil.h"
#include "memcheck.h"

/*
 * static_timer_list
 *  |
 *  |-> i_list of next-run-sorted timers with the same interval
 *  |-> i_list of next-run-sorted timers with the same interval
 *  |-> etc
 *
 * static_timer_table
 *  |-> key = interval ... value = i_list of next-run-sorted timers with same interval
 *
 */

i_list *static_timer_list = NULL;         /* A next-run sorted list of lists of next-run-sorted timers of the same interval */
i_hashtable *static_timer_table = NULL;   /* Hash table keyed on interval containing lists of next-run-sorted timers of the same interval */

struct timeval static_last_run;

/* Initialisation */

int i_timer_init ()
{
  if (static_timer_list)
  {
    i_list *list;
    for (i_list_move_head(static_timer_list); (list=i_list_restore(static_timer_list))!=NULL; i_list_move_next(static_timer_list))
    {
      i_timer *timer;
      for (i_list_move_head(list); (timer=i_list_restore(list))!=NULL; i_list_move_next(list))
      { i_timer_free (timer); }
      i_list_free (list);
    }
    i_list_free (static_timer_list);
  }
  static_timer_list = i_list_create ();

  if (static_timer_table)
  {
    i_hashtable_free (static_timer_table);
  }
  static_timer_table = i_hashtable_create (1000);
  
  gettimeofday (&static_last_run, NULL);

  return 0;
}

/* Struct Manipulation */

i_timer* i_timer_create ()
{
  i_timer *timer;

  timer = (i_timer *) malloc (sizeof(i_timer));
  if (!timer)
  { i_printf (1, "i_timer_create failed to malloc timer struct"); return NULL; }
  memset (timer, 0, sizeof(i_timer));

  return timer;
}

void i_timer_free (void *timer_ptr)
{
  i_timer *timer = timer_ptr;

  if (!timer) return;

  free (timer);
}

/* List Manipulation */

i_list* i_timer_same_interval_list (time_t sec)
{
  /* Finds/Creates the list of timers with the
   * same interval as the specific time 
   */
  i_hashtable_key *key = i_hashtable_create_key_long (sec, static_timer_table->size);
  i_list *list = i_hashtable_get (static_timer_table, key);
  if (!list)
  {
    /* Create the list and store it */
    list = i_list_create ();
//    i_printf (1, "i_timer_same_interval_list created list %p for interval %li", list, sec);
    i_hashtable_put (static_timer_table, key, list);
    i_list_enqueue (static_timer_list, list);
    static_timer_list->sorted = 0;
  }
  i_hashtable_key_free (key);

  return list;
}

/* Process Timers */

int i_timer_process (i_resource *self)
{
  int num;
  struct timeval now;

  gettimeofday (&now, NULL);

  /* Check to see if a timer *could* have expired */
  struct timeval shortest_timeout;
  i_timer_select_timeout (self, &shortest_timeout);
  struct timeval earliest_event;
  earliest_event.tv_sec = static_last_run.tv_sec + shortest_timeout.tv_sec;
  earliest_event.tv_usec = static_last_run.tv_usec + shortest_timeout.tv_usec;
  if (i_time_check_past(&now, &earliest_event) != 1)
  { 
//    i_printf (1, "i_timer_process suppressing timer processing, nothing *could* be ready");
    return 0;
  }

  /* Sort list of sorted timer lists, if required */
  if (static_timer_list && static_timer_list->sorted == 0)
  {
    /* This will sort the static_timer_list to being a 
     * next-run sorted list of next-run sorted timers
     * of the same interval
     */ 
//    i_printf (0, "i_timer_process having to perform list sort (%i)", static_timer_list->size);
    i_list_sort (static_timer_list, i_timer_listsort_nextrun_list); 
  }

  /* Create tag */
  long tag = random ();

  /* Check for timers */
//  i_printf (0, "---[timer run begins]------------------------------------------------------");
  i_list *list;
  for (i_list_move_head(static_timer_list); (list=i_list_restore(static_timer_list)) != NULL; i_list_move_next(static_timer_list))
  {
    i_timer *timer;
    time_t timer_interval = 0;

    /* Loop through each timer */
    for (i_list_move_head(list); (timer=i_list_restore(list)) != NULL; i_list_move_next(list))
    {
      /* Check the tag */
      if (timer->tag == tag)
      {
        /* We've already processed this timer! */
        timer = NULL;
        break;
      }
      timer->tag = tag;

      /* Establish next-run time */
      struct timeval next_run;
      next_run.tv_sec = timer->last_run.tv_sec + timer->timeout.tv_sec;
      next_run.tv_usec = timer->last_run.tv_usec + timer->timeout.tv_usec;
      timer_interval = timer->timeout.tv_sec;

      /* Check if timeout has expired */
      if (i_time_check_past(&now, &next_run) == 1)
      {
        /* Time-out on timer has expired */
        if (timer->callback_func) 
        {
          /* Record mem usage */
          float vss_before = 0.0;
          if (self->perflog) vss_before = i_memcheck_rss ();

          /* ** 
           * Run timer callback present 
           **/
          num = timer->callback_func (timer->resource, timer, timer->data);

          /* Check mem usage */
          if (self->perflog)
          {
            float vss_after = i_memcheck_rss ();
            i_printf (1, "PERF: After processing timer for callback %p vss memory usage increased %.0fbytes", timer->callback_func, (vss_after - vss_before));
          }

          if (num == -1) 
          {
            /* Callback returned -1, remove the timer */
            i_timer_remove (timer);
          }
          else
          {
            /* Callback returned 0, timers stays and is reset */
            timer->uses++;
            i_timer_reset (timer);
          }
        }
        else
        {  
          /* No callback, remove it */
          i_list_delete (list);
          i_timer_free (timer);
        }
      }
      else
      {
        /* Time-out on current timer hasnt expired.
         * Because the timer list is sorted by next_run due time,
         * the for loop can now be exited because no other timers
         * will be due/
         */
        break;
      }
    }
    
    /* Check to see if the parsing broke early due to future
     * timers being encountered. If so, break
     */
    if (timer)
    { 
      break;
    }
  }

  gettimeofday (&static_last_run, NULL);

  return 0;
}

/* Calcualte suitable select timeout for i_loop */

int i_timer_select_timeout (i_resource *self, struct timeval *select_timeout)
{
  /* Set value of select_timeout to the interval 
   * between now and when the next time is due 
   *
   * Return 0 if select_timeout has not been set (no timers)
   * Return 1 if select_timeout has been set to something meaningful
   * Return -1 on error
   */

  int num;

  if (static_timer_list && static_timer_list->sorted == 0)
  { 
 //   i_printf (0, "i_timer_select_timeout having to perform list sort (%i)", static_timer_list->size);
    i_list_sort (static_timer_list, i_timer_listsort_nextrun_list); 
  }

  /* Get first timer in first timer list */
  i_list *list;
  i_timer *timer = NULL;
  for (i_list_move_head (static_timer_list); (list = i_list_restore (static_timer_list))!=NULL; i_list_move_next(static_timer_list))
  {
    i_list_move_head (list);
    timer = i_list_restore (list);
    if (timer) break;
  }
  
  if (timer)
  {
    struct timeval now;
    struct timeval earliest;
    gettimeofday (&now, NULL);

    earliest.tv_sec = timer->last_run.tv_sec + timer->timeout.tv_sec;
    earliest.tv_usec = timer->last_run.tv_usec + timer->timeout.tv_usec;

    if (i_time_check_past(&now, &earliest))
    {
      /* Timer should have already run */
      select_timeout->tv_sec = 0;
      select_timeout->tv_usec = 0;
    }
    else
    {
      /* Timer is in the future */
      num = i_time_subtract (&earliest, &now, select_timeout);
      if (num != 0)
      {
        /* Result was negative or an error occurred */
        select_timeout->tv_sec = 0;
        select_timeout->tv_usec = 0;
      }
    }
    
    return 1;
  }
  else
  { return 0; }
}

/* Add */
    
i_timer* i_timer_add (i_resource *resource, time_t sec, time_t usec, int (*callback_func) (), void *data)
{ 
  i_timer *timer;

  if (!callback_func) return NULL;

  /* Create the struct */
  timer = i_timer_create ();
  timer->resource = resource;
  timer->callback_func = callback_func;
  timer->data = data;
  gettimeofday (&timer->last_run, NULL);  /* Initial value */

  /* Set timer parameters */
  timer->timeout.tv_sec = sec;
  timer->timeout.tv_usec = usec;
  gettimeofday (&timer->last_run, NULL);    /* Reset the last-run timer to now */

  /* Add ourselves to the back of the queue in our same-interval list */
  i_list *list = i_timer_same_interval_list (sec);
  i_list_enqueue (list, timer);

  return timer;
}

/* Remove */
  
int i_timer_remove (i_timer *timer)
{   
  /* Find the timer and NULLifies it, the timer is
   * then delete from the list by the timer processor
   */

  if (!timer) return -1;

  timer->resource = NULL;
  timer->callback_func = NULL;
  timer->data = NULL;
  i_timer_set_timeout (timer, 0, 0);    /* Ensure its removed on next timer processing */

  return 0;
}

/* Timeout Adjustment */
  
int i_timer_set_timeout (i_timer *timer, time_t sec, time_t usec)
{
  /* Check timer order change (actual change in value) */
  if (timer->timeout.tv_sec != sec)
  { 
    /* Timer has changed, will need to remove
     * from the current same-interval list
     * and move it to the new same-interval list
     */
    i_list *list = i_timer_same_interval_list (timer->timeout.tv_sec);
    if (i_list_search(list, timer) == 0)
    { i_list_delete (list); }
    else
    { i_printf (1, "i_timer_set_timeout ERROR: Did not find timer %p in should-be-owner list %p", timer, list); }

    /* Set timeout value */
    timer->timeout.tv_sec = sec;
    timer->timeout.tv_usec = usec;
    gettimeofday (&timer->last_run, NULL);    /* Reset the last-run timer to now */

    /* Add to new list */
    list = i_timer_same_interval_list (sec);
    i_list_enqueue (list, timer);
  }

  return 0;
} 

/* Timer reset */

int i_timer_reset (i_timer *timer)
{
  /* Set the last-run timer to now */
  gettimeofday (&timer->last_run, NULL);

  /* Shunt the timer to the back of its same-interval list */
  i_list *list = i_timer_same_interval_list (timer->timeout.tv_sec);
  if (i_list_search(list, timer) == 0)
  {
//    i_printf (1, "i_timer_reset shunted timer %p in list %p", timer, list); 
    i_list_shunt (list); 
  }
  static_timer_list->sorted = 0;

  return 0;
}

/* Timer list sort */

int i_timer_listsort_nextrun (void *curptr, void *nextptr)
{
  /* List-sort function to sort a list of timers
   * based on the next-running timer being first 
   * in the list
   */
  
  i_timer *cur = curptr;
  struct timeval cur_nextrun;
  i_timer *next = nextptr;
  struct timeval next_nextrun;

  cur_nextrun.tv_sec = cur->last_run.tv_sec + cur->timeout.tv_sec;
  cur_nextrun.tv_usec = cur->last_run.tv_usec + cur->timeout.tv_usec;
  next_nextrun.tv_sec = next->last_run.tv_sec + next->timeout.tv_sec;
  next_nextrun.tv_usec = next->last_run.tv_usec + next->timeout.tv_usec;
  
  if (i_time_check_past(&cur_nextrun, &next_nextrun))
  {
    /* The cur_nextrun time is past (later) than the 
     * next_nextrun time. Swapping required.
     */
    return 1;
  }

  /* cur_nextrun time is earlier than the next_nextrun time.
   * No swapping required
   */

  return 0;
}

int i_timer_listsort_nextrun_list (void *curptr, void *nextptr)
{
  i_list *cur = curptr;
  i_list *next = nextptr;
  i_list_move_head (cur);
  i_list_move_head (next);
  i_timer *cur_timer = i_list_restore (cur);
  i_timer *next_timer = i_list_restore (next);

  if (!cur_timer) return 1;
  else if (!next_timer) return 0;
  else return i_timer_listsort_nextrun (cur_timer, next_timer);
}
