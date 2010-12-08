#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>

#include "avail.h"

/* Master Object Refresh */

int l_avail_refresh_master_obj (i_resource *self, i_object *masterobj, int opcode)
{
  /* Refrsh the master object and the metrics
   * for all non-master objects
   */

  i_object *obj;
  i_metric_value *val;
  l_avail_item *masteritem = masterobj->itemptr;
  float master_success_aggregate = 0.0;
  int master_success_count = 0;
  float master_failure_aggregate = 0.0;
  int master_failure_count = 0;
  float master_resptime_aggregate = 0.0;
  int master_resptime_count = 0;
  
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:       /* Begin the refresh process */

      /* Refresh non-master objects */
      for (i_list_move_head(masterobj->cnt->obj_list); (obj=i_list_restore(masterobj->cnt->obj_list))!=NULL; i_list_move_next(masterobj->cnt->obj_list))
      {
        if (obj == masterobj) continue;
        l_avail_item *item = obj->itemptr;

        /* OK/Failed Operations */
        if (item->interim > 0)
        {
          /* Refreshing the ok_ops metric */
          val = i_metric_value_create ();
          val->count = item->ok_interim;
          i_metric_value_enqueue (self, item->ok_ops, val);
          item->ok_ops->refresh_result = REFRESULT_OK;
          i_entity_refresh_terminate (ENTITY(item->ok_ops));
      
          /* Refreshing the fail_ops metric */
          val = i_metric_value_create ();
          val->count = item->fail_interim;
          i_metric_value_enqueue (self, item->fail_ops, val);
          item->fail_ops->refresh_result = REFRESULT_OK;
          i_entity_refresh_terminate (ENTITY(item->fail_ops));
        }

        /* Response Time */
        if (item->rt_list && item->rt_list->size > 0)
        {
          /* Set value */
          val = i_metric_value_create ();
          double *rt_ms;
          for (i_list_move_head(item->rt_list); (rt_ms=i_list_restore(item->rt_list))!=NULL; i_list_move_next(item->rt_list))
          { 
            val->flt = val->flt + *rt_ms; 
          }
          val->flt = val->flt / (float) item->rt_list->size;

          /* Enqueue the value */
          i_metric_value_enqueue (self, item->resptime, val);
          item->resptime->refresh_result = REFRESULT_OK;
          i_entity_refresh_terminate (ENTITY(item->resptime));
        }

        /* Reset counters */
        item->ok_interim = 0;
        item->fail_interim = 0;
        item->interim = 0;
        i_list_free (item->rt_list);
        item->rt_list = i_list_create ();
        i_list_set_destructor (item->rt_list, free);

        /* Add values to master counts */
        val = i_metric_curval (item->ok_pc);
        if (val) 
        {
          master_success_aggregate = master_success_aggregate + i_metric_valflt (item->ok_pc, val);
          master_success_count++;
        }
        val = i_metric_curval (item->fail_pc);
        if (val) 
        {
          master_failure_aggregate = master_failure_aggregate + i_metric_valflt (item->fail_pc, val);
          master_failure_count++;
        }
        val = i_metric_curval (item->resptime);
        if (val) 
        {
          master_resptime_aggregate = master_resptime_aggregate + i_metric_valflt (item->resptime, val);
          master_resptime_count++;
        }
      }

      /* Refresh master OK Ops */
      if (master_success_count > 0)
      {
        val = i_metric_value_create ();
        val->flt = master_success_aggregate / (float) master_success_count;
        i_metric_value_enqueue (self, masteritem->ok_pc, val);
        masteritem->ok_pc->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(masteritem->ok_pc));
      }

      if (master_failure_count)
      {
        /* Refresh master Failed Ops */
        val = i_metric_value_create ();
        val->flt = master_failure_aggregate / (float) master_failure_count;
        i_metric_value_enqueue (self, masteritem->fail_pc, val);
        masteritem->fail_pc->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(masteritem->fail_pc));
      }

      if (master_resptime_count)
      {
        /* Refresh master Response Time */
        val = i_metric_value_create ();
        val->flt = master_resptime_aggregate / (float) master_resptime_count;
        i_metric_value_enqueue (self, masteritem->resptime, val);
        masteritem->resptime->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(masteritem->resptime));
      }

      masterobj->refresh_result = REFRESULT_OK;

      return 1;   /* Refresh Complete */

    case REFOP_COLLISION:     /* Handle a refresh collision */
      break;
    case REFOP_TERMINATE:     /* Terminate an existing refresh */
      break;
    case REFOP_CLEANDATA:     /* Cleanup the refresh_data struct */
      break;
  }

  return 0;
}


