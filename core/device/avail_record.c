#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>

#include "avail.h"

/* Counter incrementing */

int l_avail_record_ok (i_object *obj, struct timeval *start, struct timeval *end)
{ 
  /* Record that an operation has completed
   * successfully for the specified l_avail 
   * object. The start and end times are 
   * used (if specified) for the response time
   * calculation.
   */
  int num;
  l_avail_item *item = obj->itemptr;

  /* Increment the interim counter variable */
  item->ok_interim++;
  item->interim++;

  /* Check for start/end times */
  if (start && end)
  {
    struct timeval diff_tv;

    /* Calculate difference */
    num = i_time_subtract (end, start, &diff_tv);
    if (num == 0)
    {
      double rt_ms;
      double *rt_ms_ptr;

      /* Create the rt_ms float value */
      rt_ms = diff_tv.tv_sec * 1000;               /* Multiply the seconds */
      rt_ms = diff_tv.tv_usec / 1000;              /* Divide the microseconds */

      /* Create and enqueue rt_ms value for the item */
      rt_ms_ptr = (double *) malloc (sizeof(double));
      memcpy (rt_ms_ptr, &rt_ms, sizeof(double));
      num = i_list_enqueue (item->rt_list, rt_ms_ptr);
      if (num != 0)
      { i_printf (1, "l_avail_record_ok warning, failed to enqueue rt_ms for the %s item", obj->name_str); free (rt_ms_ptr); }
    }
    else
    { i_printf (1, "l_avail_record_ok warning, start time is after end time; clock skew?"); }
  }

  return 0; 
}

int l_avail_record_fail (i_object *obj)
{ 
  /* Record that an operation has failed
   * for the specified l_avail object.
   */
  
  /* Increment the interim counter variable */
  l_avail_item *item = obj->itemptr;
  item->fail_interim++; 
  item->interim++; 

  return 0; 
}


