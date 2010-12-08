#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "path.h"
#include "rrd.h"
#include "rrdtool.h"
#include "cement.h"
#include "hierarchy.h"
#include "customer.h"
#include "entity.h"
#include "device.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "timeutil.h"

/** \addtogroup metric_record Recording and Graphing
 * @ingroup metric
 * @{
 */

/* 
 * Metric RRD Recording
 */

int i_metric_record_rrd (i_resource *self, i_metric *met)
{
  /* Perform the configured recording procedure 
   * for the given metric.
   */

  struct timeval now_tv;
  i_metric_value *curval;

  gettimeofday (&now_tv, NULL);

  /* Check for a current value */
  curval = i_metric_curval (met);
  if (!curval) 
  { return -1; }

  /* Enqueue value */
  double valflt = i_metric_valflt (met, curval);
  if (!met->rrd_update_args)
  { asprintf (&met->rrd_update_args, "%li:%f", curval->tstamp.tv_sec, valflt); }
  else
  {
    char *temp;
    asprintf (&temp, "%s %li:%f", met->rrd_update_args, curval->tstamp.tv_sec, valflt);
    free (met->rrd_update_args);
    met->rrd_update_args = temp;
  }
  met->rrd_update_count++;
    
  /* Check 5-min recording interval */
  if ((now_tv.tv_sec - met->rrd_5min_tstamp.tv_sec) >= 300 || strlen(met->rrd_update_args) > 9000)
  {
    /* Perform actual RRDtool update */
    i_metric_record_rrd_queued (self, met, RRDQUEUE_5MIN);
    gettimeofday (&met->rrd_5min_tstamp, NULL);
    i_metric_record_rrd_queued (self, met, RRDQUEUE_6HR);
    gettimeofday (&met->rrd_6hr_tstamp, NULL);

    /* Cleanup */
    free (met->rrd_update_args);
    met->rrd_update_args = NULL;
    met->rrd_update_count = 0;
  }
  
  return 0;
}

int i_metric_record_rrd_queued (i_resource *self, i_metric *met, unsigned short queueflag)
{
  int num;
  char *rrdarg;
  char *fullpath;
  char *rrdfilename = NULL;
  char *fsroot = NULL;
  char *rra_defs = NULL;
  int path_flags = 0;
  struct timeval now_tv;
  unsigned short rrdcheck_flag = 0;       /* 0=Do not check RRD   1=Check RRD */
  i_rrdtool_cmd *cmd;

  gettimeofday (&now_tv, NULL);

  switch (queueflag)
  {
    case RRDQUEUE_5MIN:
      /* Month archive */
      rrdfilename = "month_archive.rrd";
      path_flags = ENTPATH_MONTH;
      rra_defs = RRD_RRA_MONTH;
      if (now_tv.tv_sec >= met->rrd_month_vsec)
      { rrdcheck_flag = 1; }
      break;
    case RRDQUEUE_6HR:
      /* Year archive */
      rrdfilename = "year_archive.rrd";
      path_flags = ENTPATH_YEAR;
      rra_defs = RRD_RRA_YEAR;
      if (now_tv.tv_sec >= met->rrd_year_vsec)
      { rrdcheck_flag = 1; }
      break;
  }

  /* Get fsroot string */
  fsroot = i_entity_path (self, ENTITY(met), now_tv.tv_sec, path_flags|ENTPATH_CREATE);
  if (!fsroot)
  { 
    i_printf (1, "i_metric_record_rrd_queued warning, failed to get the file system path for %s %s %s", 
      rrdfilename, i_entity_typestr (met->ent_type), met->name_str);
    return -1;
  }

  /* Create RRD fullpath */      
  fullpath = i_path_glue (fsroot, rrdfilename);
  free (fsroot);

  /* Check/Create the RRD */
  if (rrdcheck_flag == 1)
  {
    struct tm end_tm;
      
    num = i_rrd_check_exists (self, fullpath);
    if (num != 0)
    {
      /* RRD Does not exist, create it */
      char *dststr;
      double maxflt;
      double minflt;
      char *maxstr = NULL;
      char *minstr = NULL;
      
//      if (met->met_type == METRIC_COUNT)
//      { dststr = "COUNTER"; }
//      else
//      { dststr = "GAUGE"; }

      dststr = "GAUGE";

      /* Check for min_val minimum value */
      if (met->min_val)
      { 
        minflt = i_metric_valflt (met, met->min_val); 
        minflt *= met->alloc_unit;
        asprintf (&minstr, "%f", minflt);
      }
      if (!minstr)
      { minstr = strdup ("U"); }

      /* Check for max_val maximum value */
      if (met->max_val)
      { 
        maxflt = i_metric_valflt (met, met->max_val); 
        maxflt *= met->alloc_unit;
        asprintf (&maxstr, "%f", maxflt);
      }
      if (!maxstr)
      { maxstr = strdup ("U"); }

      /* Create the RRD */
      asprintf (&rrdarg, "DS:%s:%s:600:%s:%s %s", METRIC_RRD_DATASOURCE, dststr, minstr, maxstr, rra_defs);
      cmd = i_rrd_create (self, fullpath, 0, 0, rrdarg, NULL, NULL);

      /* Cleanup */
      free (minstr);
      free (maxstr);
      free (rrdarg);

      /* End of RRD file creation */
    }

    /* Set valid sec time */
    switch (queueflag)
    {
      case RRDQUEUE_5MIN:
        /* Month Handling */
        met->rrd_month_vsec = i_time_monthend (now_tv.tv_sec, &end_tm);
        break;
      case RRDQUEUE_6HR:
        /* Year Handling */
        met->rrd_year_vsec = i_time_yearend (now_tv.tv_sec, &end_tm);
        break;
    }
    
    /* End of RRD file check/create */
  }

  /* Update RRD */
  i_rrd_update (self, met->record_priority, fullpath, met->rrd_update_args, NULL, NULL);
  free (fullpath);

  return 0;
}

/* @} */
