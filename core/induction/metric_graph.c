#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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
#include "trigger.h"
#include "value.h"
#include "colour.h"

/** \addtogroup metric_record Recording and Graphing
 * @ingroup metric
 * @{
 */

/* 
 * Cement Metric - Single Metric Graph Function
 */

/* Metric Graph Rendering */

i_rrdtool_cmd* i_metric_graph_render (i_resource *self, i_metric *met, int graphsize, time_t ref_sec, int period, int (*cbfunc) (), void *passdata)
{
  /* Create a graph for a single metric.
   * This is achieved by generating a 
   * cgraph with a single metric and a
   * standard render string
   */
  int num;
  char *descstr_esc;
  char *escunit_str;
  i_trigger *trg;
  i_rrdtool_cmd *cmd;
  i_metric_cgraph *cgraph;

  /* Check/Set ref_sec */
  if (ref_sec < 1)
  { ref_sec = time (NULL); }

  /* Check/Set unit_str */
  if (met->unit_str)
  { escunit_str = met->unit_str; }
  else
  { escunit_str = ""; }
  
  /* Check if unit_str needs escaping */
  if (!strcmp(escunit_str, "%"))
  { escunit_str = "%%"; }
  
  /* Create cgraph struct */
  cgraph = i_metric_cgraph_create (met->obj, met->name_str, met->unit_str);
  if (!cgraph) 
  { i_printf (1, "i_metric_graph_render failed to create cgraph struct"); return NULL; }
  cgraph->kbase = met->kbase;
  descstr_esc = i_rrd_comment_escape (met->obj->desc_str);
  asprintf (&cgraph->title_str, "%s %s", descstr_esc, met->desc_str);
  free (descstr_esc);
  asprintf (&cgraph->render_str, "\"LINE1:met_%s_%s_min#000E73:Min.\" \"LINE1:met_%s_%s_avg#001EFF:Avg.\" \"LINE1:met_%s_%s_max#00B4FF:Max.  %s \" \"GPRINT:met_%s_%s_min:MIN:Min %%.2lf%%s%s\" \"GPRINT:met_%s_%s_avg:AVERAGE:Avg %%.2lf%%s%s\" \"GPRINT:met_%s_%s_max:MAX:Max %%.2lf%%s%s\\n\" ",
    met->obj->name_str, met->name_str, 
    met->obj->name_str, met->name_str, 
    met->obj->name_str, met->name_str, met->desc_str, 
    met->obj->name_str, met->name_str, escunit_str, 
    met->obj->name_str, met->name_str, escunit_str, 
    met->obj->name_str, met->name_str, escunit_str);
  num = i_list_enqueue (cgraph->met_list, met);
  if (num != 0)
  { i_printf (1, "i_metric_graph_render failed to enqueue metric %s", met->name_str); i_metric_cgraph_free (cgraph); return NULL; }

  /* Trigger display */

  for (i_list_move_head(met->trg_list); (trg=i_list_restore(met->trg_list))!=NULL; i_list_move_next(met->trg_list))
  {
    char *valstr;
    char *yvalstr;
    char *oldrender_str = cgraph->render_str;

    valstr = i_value_valstr_raw (trg->val_type, trg->val);

    if (trg->trg_type == TRGTYPE_RANGE)
    {
      /* Range triggers */
      yvalstr = i_value_valstr_raw (trg->val_type, trg->yval);
      asprintf (&cgraph->render_str, "%s \"HRULE:%s%s:%s Trigger (%s) - Range %s to %s\\n\" \"HRULE:%s%s\"",
        oldrender_str, 
        valstr, i_colour_fg_str (trg->effect), trg->desc_str, i_entity_opstatestr(trg->effect), valstr, yvalstr,
        yvalstr, i_colour_fg_str (trg->effect));
      free (oldrender_str);
      if (yvalstr) free (yvalstr);
    }
    else
    {
      /* Other triggers */
      char *trgtype_str = "";
        
      switch (trg->trg_type)
      {
        case TRGTYPE_LT: trgtype_str = "Less than";
                         break;
        case TRGTYPE_GT: trgtype_str = "Greater than";
                         break;
        case TRGTYPE_EQUAL: trgtype_str = "Equal to";
                            break;
        case TRGTYPE_NOTEQUAL: trgtype_str = "Not equal to";
                               break;
      }

      asprintf (&cgraph->render_str, "%s \"HRULE:%s%s:%s Trigger (%s) - %s %s\\n\"",
        oldrender_str, valstr, i_colour_fg_str (trg->effect), trg->desc_str, i_entity_opstatestr(trg->effect), 
        trgtype_str,valstr);
      free (oldrender_str);
    }

    free (valstr);
  }

  /* Render the graph */
  cmd = i_metric_cgraph_render (self, cgraph, graphsize, ref_sec, period, cbfunc, passdata);
  i_metric_cgraph_free (cgraph);

  return cmd;
}

/* Period String Utility */

char* i_metric_graph_periodstr (time_t ref_sec, int period)
{
  char *periodstr;
  char *tempstr;
  time_t start;
  struct tm ref_tm;
  struct tm start_tm;
  struct timeval now_tv;
  
  /* Create periodstr */
  localtime_r (&ref_sec, &ref_tm);
  switch (period)
  {
    case GRAPHPERIOD_LAST48HR:
      gettimeofday (&now_tv, NULL);
      tempstr = i_time_ctime (now_tv.tv_sec);
      asprintf (&periodstr, "48 Hours up to %s", tempstr);
      free (tempstr);
      break;
    case GRAPHPERIOD_DAY:
      asprintf (&periodstr, "48 Hours up to midnight, %i %s %i", ref_tm.tm_mday, i_time_month_string (ref_tm.tm_mon), ref_tm.tm_year+1900);
      break;
    case GRAPHPERIOD_WEEK:
      start = i_time_weekof (ref_sec, &start_tm);
      asprintf (&periodstr, "Week of %i %s %i", start_tm.tm_mday, i_time_month_string (start_tm.tm_mon), start_tm.tm_year+1900);
      break;
    case GRAPHPERIOD_MONTH:
      start = i_time_monthstart (ref_sec, &start_tm);
      asprintf (&periodstr, "Month of %s %i", i_time_month_string (start_tm.tm_mon), start_tm.tm_year+1900);
      break;
    case GRAPHPERIOD_YEAR:
      start = i_time_yearstart (ref_sec, &start_tm);
      asprintf (&periodstr, "Year %i", start_tm.tm_year+1900);
      break;
    default:
      i_printf (1, "i_metric_graph_periodstr unsupported graph period %i specified", period);
      return NULL;
  }

  return periodstr;
}
/* @} */
