#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <paths.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "timeutil.h"
#include "rrd.h"
#include "rrdtool.h"
#include "hierarchy.h"
#include "path.h"
#include "cement.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"

/** \addtogroup metric_record Recording and Graphing
 * @ingroup metric
 * @{
 */

/* 
 * Cement Metric - Custom/Combined Graph Functions
 *
 * Custom/Combined graphs (cgraphs) are graphs
 * that display multiple metrics with a customised
 * graph-specific rendering arrangement. 
 */

/* Struct Manipulation */

i_metric_cgraph* i_metric_cgraph_create (i_object *obj, char *name_str, char *yaxis_str)
{
  i_metric_cgraph *cgraph;

  /* Create/Config CGraph Struct */
  cgraph = (i_metric_cgraph *) malloc (sizeof(i_metric_cgraph));
  if (!cgraph)
  { i_printf (1, "i_metric_cgraph_create failed to malloc i_metric_cgraph struct"); return NULL; }
  memset (cgraph, 0, sizeof(i_metric_cgraph));
  cgraph->obj = obj;
  if (name_str) cgraph->name_str = strdup (name_str);
  if (yaxis_str) cgraph->yaxis_str = strdup (yaxis_str);
  cgraph->width_small = METRIC_CGRAPH_WIDTH_SMALL;
  cgraph->width_medium = METRIC_CGRAPH_WIDTH_MEDIUM;
  cgraph->width_large = METRIC_CGRAPH_WIDTH_LARGE;
  cgraph->height_small = METRIC_CGRAPH_HEIGHT_SMALL;
  cgraph->height_medium = METRIC_CGRAPH_HEIGHT_MEDIUM;
  cgraph->height_large = METRIC_CGRAPH_HEIGHT_LARGE;
  cgraph->kbase = 1000;   /* Default kbase */

  /* Create Metric List */
  cgraph->met_list = i_list_create ();
  if (!cgraph->met_list)
  { i_printf (1, "i_metric_cgraph_create failed to create cgraph->met_list"); i_metric_cgraph_free (cgraph); return NULL; }

  /* Add to obj's cgraph list */
  if (obj)
  {
    if (!obj->cgraph_list)
    { obj->cgraph_list = i_list_create (); }
    i_list_enqueue (obj->cgraph_list, cgraph); 
  }

  return cgraph;
}

void i_metric_cgraph_free (void *cgraphptr)
{
  i_metric_cgraph *cgraph = cgraphptr;

  if (!cgraph) return;

  if (cgraph->name_str) free (cgraph->name_str);
  if (cgraph->title_str) free (cgraph->title_str);
  if (cgraph->yaxis_str) free (cgraph->yaxis_str); 
  if (cgraph->render_str) free (cgraph->render_str); 
  if (cgraph->met_list) i_list_free (cgraph->met_list); 
  if (cgraph->obj && cgraph->obj->cgraph_list)
  {
    if (i_list_search(cgraph->obj->cgraph_list, cgraph) == 0)
    { 
      i_list_delete (cgraph->obj->cgraph_list);
      if (cgraph->obj->cgraph_list->size < 1)
      { i_list_free (cgraph->obj->cgraph_list); cgraph->obj->cgraph_list = NULL; }
    }
  }

  free (cgraph);
}

/* Custom Graph Retrieval */

i_metric_cgraph* i_metric_cgraph_get (i_object *obj, char *name_str)
{
  i_metric_cgraph *cgraph;

  for (i_list_move_head(obj->cgraph_list); (cgraph=i_list_restore(obj->cgraph_list))!=NULL; i_list_move_next(obj->cgraph_list))
  { if (!strcmp(cgraph->name_str, name_str)) return cgraph; }

  return NULL;
}

/* Custom Graph Rendering */

i_rrdtool_cmd* i_metric_cgraph_render (i_resource *self, i_metric_cgraph *cgraph, int graphsize, time_t ref_sec, int period, int (*cbfunc) (), void *passdata)
{
  /* Renders a cgraph and returns the rrdtool cmd
   * for the graph command. The image 
   * will be stored in a temporary location and 
   * should be unlinked immediately after use.
   */
  int width;                /* Graph width */
  int height;               /* Graph height */
  int path_flag;            /* Period specific path flag */
  char *temp_str;           /* General purpose temp_str */
  char *rrdparams;          /* Basic params for the RRD graph command */
  char *rrddefs = NULL;     /* The DEF portion of the RRD arguments */
  char *rrdargs;            /* Full RRD arguments string (params, defs and render string) */
  char *periodstr;          /* Period string (e.g 'the day of 1 January 2004') */
  char *rrdfilename;        /* Period-specific RRD filename */
  char *graphfullpath;      /* Full path to graph image file */
  char *ul_str;
  char *ll_str;
  char *rigid_str;
  time_t start;
  time_t end;
  struct tm ref_tm;
  struct tm start_tm;
  struct tm end_tm;
  struct timeval now_tv;
  i_metric *met;
  i_rrdtool_cmd *cmd;

  /* Check/Set ref_sec */
  if (ref_sec < 1)
  { ref_sec = time (NULL); }

  /* Set width/height depending on the graph size flag */
  switch (graphsize)
  {
    case GRAPHSIZE_SMALL:
      width = cgraph->width_small;
      height = cgraph->height_small;
      break;
    case GRAPHSIZE_MEDIUM:
      width = cgraph->width_medium;
      height = cgraph->height_medium;
      break;
    case GRAPHSIZE_LARGE:
      width = cgraph->width_large;
      height = cgraph->height_large;
      break;
    default:
      i_printf (1, "i_metric_cgraph_render unsupported graphsize %i specified. Using medium height/width.", graphsize);
      width = cgraph->width_medium;
      height = cgraph->height_medium;
  }
  
  /* Set period-specific variables */
  localtime_r (&ref_sec, &ref_tm);
  switch (period)
  {
    case GRAPHPERIOD_LAST48HR:
      gettimeofday (&now_tv, NULL);
      end = now_tv.tv_sec;
      start = end - (60*60*48);  
      rrdfilename = "month_archive.rrd";
      path_flag = ENTPATH_MONTH;
      break;
    case GRAPHPERIOD_DAY:
      start = i_time_daystart (ref_sec, &end_tm);
      start -= (60*60*24);
      end = start + (60*60*48);
      rrdfilename = "month_archive.rrd";
      path_flag = ENTPATH_MONTH;
      break;
    case GRAPHPERIOD_WEEK:
      start = i_time_weekof (ref_sec, &start_tm);
      end = start + (7*60*60*24);
      rrdfilename = "month_archive.rrd";
      path_flag = ENTPATH_MONTH;
      break;
    case GRAPHPERIOD_MONTH:
      start = i_time_monthstart (ref_sec, &start_tm);
      end = i_time_monthend (ref_sec, &end_tm);
      rrdfilename = "month_archive.rrd";
      path_flag = ENTPATH_MONTH;
      break;
    case GRAPHPERIOD_YEAR:
      start = i_time_yearstart (ref_sec, &start_tm);
      end = i_time_yearend (ref_sec, &end_tm);
      rrdfilename = "year_archive.rrd";
      path_flag = ENTPATH_YEAR;
      break;
    default:
      i_printf (1, "i_metric_cgraph_render unsupported graph period %i specified; no graph %s for %s not generated", 
        cgraph->title_str, cgraph->obj->name_str);
      return NULL;
  }

  /* Get the period string */
  periodstr = i_metric_graph_periodstr (ref_sec, period);
  if (!periodstr)
  { i_printf (1, "i_metric_cgraph_render failed to retrieve period string"); return NULL; }
  temp_str = i_rrd_comment_escape (periodstr);
  free (periodstr);
  periodstr = temp_str;

  /* Limits/rigidity */
  if (cgraph->upper_limit_set == 1)
  { asprintf (&ul_str, "--upper-limit %i", cgraph->upper_limit); }
  else
  { ul_str = strdup(""); }
  if (cgraph->lower_limit_set == 1)
  { asprintf (&ll_str, "--lower-limit %i", cgraph->lower_limit); }
  else
  { ll_str = strdup(""); }
  if (cgraph->rigid == 1)
  { asprintf (&rigid_str, "--rigid"); }
  else
  { rigid_str = strdup(""); }

  /* Create the parameters string */
  asprintf (&rrdparams, "--width=%i --height=%i %s %s %s \"COMMENT:%s (%s) at %s (%s)\\n\" \"COMMENT:%s for the %s\\n\"",
    width, height, ul_str, ll_str, rigid_str, self->hierarchy->device_desc, self->hierarchy->device_id, self->hierarchy->site_desc, self->hierarchy->site_id, cgraph->title_str, periodstr);
  free (periodstr);
  free (ul_str);
  free (ll_str);
  free (rigid_str);

  /* Create the defs string */
  for (i_list_move_head(cgraph->met_list); (met=i_list_restore(cgraph->met_list))!=NULL; i_list_move_next(cgraph->met_list))
  {
    char *metdefs_str;
    char *rrdfullpath;        /* Full path to the period-specific RRD file */
    char *fsroot;             /* Period-specific file system root string */
    
    /* Create fullpath string to RRD file */
    fsroot = i_entity_path (self, ENTITY(met), ref_sec, path_flag);
    rrdfullpath = i_path_glue (fsroot, rrdfilename);
    free (fsroot);

    /* Create and append defs */
    metdefs_str = i_metric_cgraph_defs (self, met, rrdfullpath);
    free (rrdfullpath);
    if (rrddefs)
    { asprintf (&temp_str, "%s %s", rrddefs, metdefs_str); free (rrddefs); rrddefs = temp_str; free (metdefs_str); }
    else
    { rrddefs = metdefs_str; }
  }

  /* Create the full RRD args string */
  asprintf (&rrdargs, "%s %s %s", rrdparams, rrddefs, cgraph->render_str);
  free (rrdparams);
  free (rrddefs);

  /* Create temporary file */
  asprintf (&graphfullpath, "%s%s_%i-XXXXXX", _PATH_TMP, cgraph->obj->name_str, getpid());
  graphfullpath = mktemp (graphfullpath);
  if (!graphfullpath)
  { 
    i_printf (1, "i_metric_cgraph_render failed to create temporary filename for graph %s for %s", 
      cgraph->title_str, cgraph->obj->name_str);
    free (rrdargs);
    return NULL;
  }
  asprintf (&temp_str, "%s.png", graphfullpath);
  free (graphfullpath);
  graphfullpath = temp_str;

  /* Render the graph */
  cmd = i_rrd_graph (self, graphfullpath, start, end, NULL, cgraph->yaxis_str, cgraph->kbase, rrdargs, cbfunc, passdata);
  free (rrdargs);
  free (graphfullpath);
  if (!cmd)
  { 
    i_printf (1, "i_metric_cgraph_render failed to render graph %s for %s",
      cgraph->title_str, cgraph->obj->name_str);
    return NULL;
  }

  return cmd;
}
      
char* i_metric_cgraph_defs (i_resource *self, i_metric *met, char *rrdfullpath)
{ 
  char *str;

  asprintf (&str, "DEF:met_%s_%s_min='%s':%s:MIN DEF:met_%s_%s_avg='%s':%s:AVERAGE DEF:met_%s_%s_max='%s':%s:MAX", 
    met->obj->name_str, met->name_str, rrdfullpath, METRIC_RRD_DATASOURCE,
    met->obj->name_str, met->name_str, rrdfullpath, METRIC_RRD_DATASOURCE,
    met->obj->name_str, met->name_str, rrdfullpath, METRIC_RRD_DATASOURCE);

  return str;
}

/* @} */
