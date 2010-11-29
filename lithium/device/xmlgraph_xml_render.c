#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/configfile.h>
#include <induction/files.h>
#include <induction/postgresql.h>
#include <induction/auth.h>
#include <induction/entity.h>
#include <induction/entity_xml.h>
#include <induction/hierarchy.h>
#include <induction/customer.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/timeutil.h>
#include <induction/path.h>
#include <induction/rrd.h>
#include <induction/rrdtool.h>
#include <induction/xml.h>

#include "xmlgraph.h"

typedef struct l_xmlgraph_entity_s
{
  i_entity_descriptor *entdesc;
  i_metric *met;

  char *unit_str;
  
} l_xmlgraph_entity;

l_xmlgraph_entity* l_xmlgraph_entity_create ()
{ 
  l_xmlgraph_entity *g_ent = (l_xmlgraph_entity *) malloc (sizeof(l_xmlgraph_entity));
  memset (g_ent, 0, sizeof(l_xmlgraph_entity));
  return g_ent;
}

void l_xmlgraph_entity_free (void *entptr)
{
  l_xmlgraph_entity *g_ent = entptr;
  if (!g_ent) return;
  if (g_ent->entdesc) i_entity_descriptor_free (g_ent->entdesc);
  if (g_ent->unit_str) free (g_ent->unit_str);
  free (g_ent);
}

int xml_xmlgraph_render (i_resource *self, i_xml_request *xmlreq)
{
  int period = 0;
  int metric_index = 0;
  time_t ref_sec = 0;
  time_t start_sec = 0;
  time_t end_sec = 0;
  struct tm ref_tm;
  struct tm start_tm;
  struct tm end_tm;
  struct timeval now_tv;
  char *rrdfilename;
  char *rrddefs = NULL;
  char *renderargs = NULL;
  char *rrdargs;
  char *temp_str;
  char *imagefile;
  char *imagefullpath;
  char *format = strdup ("PDF");
  char *width = strdup ("320");
  char *height = strdup ("240");
  char *y_label = NULL;
  char *fsroot;
  unsigned kbase = 1000;
  int all_white = 0;
  i_rrdtool_cmd *cmd;
  unsigned short path_flag;
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;
  i_list *ent_list;
  int has_external_graphs = 0;
  int has_percentages = 0;
  
  double min_line = 0.0;     /* Used to encourage the graph canvas to a given size */
  int min_line_set = 0;
  double max_line = 0.0; 
  int max_line_set = 0;

  /* Create ent_list */
  ent_list = i_list_create ();
  i_list_set_destructor (ent_list, l_xmlgraph_entity_free);
  
  /* Get ref_sec, period and list of entity descriptor from XML */
  if (xmlreq->xml_in)
  {
    /* Iterate through XML */
    root_node = xmlDocGetRootElement (xmlreq->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;
      
      if (!strcmp((char *)node->name, "entity_descriptor"))
      { 
        /* Create graph entity struct */
        l_xmlgraph_entity *gent = l_xmlgraph_entity_create ();
        gent->entdesc = i_entity_descriptor_fromxml (xmlreq->xml_in, node);
        if (!gent->entdesc)
        { 
          i_printf (1, "xml_xmlgraph_render failed to convert an entity from xml"); 
          l_xmlgraph_entity_free (gent);
          continue; 
        }

        /* Extract extra fields */
        xmlNodePtr ent_node;
        for (ent_node = node->children; ent_node; ent_node=ent_node->next)
        {
          str = (char *) xmlNodeListGetString (xmlreq->xml_in->doc, ent_node->xmlChildrenNode, 1);
          if (!strcmp((char *)ent_node->name, "units") && str)
          { 
            gent->unit_str = strdup (str); 
          }
          xmlFree (str);
        }

        /* Attempt to locate local entity.. it's OK for this to fail */
        if (gent->entdesc)
        { gent->met = (i_metric *) i_entity_local_get (self, (i_entity_address *)gent->entdesc); }
        else
        { gent->met = NULL; }
        if (gent->met && gent->met->unit_str && !gent->unit_str)
        { gent->unit_str = strdup (gent->met->unit_str); }
        if (!gent->unit_str)
        { gent->unit_str = strdup (""); }
        if (!gent->met)
        { has_external_graphs = 1; }

        /* Enqueue entity descriptor */
        i_list_enqueue (ent_list, gent);

        /* Move on */
        continue;
      }
      
      str = (char *) xmlNodeListGetString (xmlreq->xml_in->doc, node->xmlChildrenNode, 1);

      if (!strcmp((char *)node->name, "ref_sec")) ref_sec = (time_t) atol (str);
      else if (!strcmp((char *)node->name, "start_sec")) start_sec = (time_t) atol (str);
      else if (!strcmp((char *)node->name, "end_sec")) end_sec = (time_t) atol (str);
      else if (!strcmp((char *)node->name, "period")) period = atoi (str);
      else if (!strcmp((char *)node->name, "format")) format = strdup (str);
      else if (!strcmp((char *)node->name, "height")) height = strdup (str); 
      else if (!strcmp((char *)node->name, "width")) width = strdup (str); 
      else if (!strcmp((char *)node->name, "all_white")) all_white = atoi (str); 

      xmlFree (str);
    }
  } 
  else
  {
    i_printf (2, "xml_xmlgraph_render error, no xml data received");
    return -1;
  }

  /* Set period-specific variables */
  localtime_r (&ref_sec, &ref_tm);
  switch (period)
  {
    case GRAPHPERIOD_LAST48HR:
      gettimeofday (&now_tv, NULL);
      if (ref_sec > 0)
      { end_sec = ref_sec; }
      else
      { end_sec = now_tv.tv_sec; }
      start_sec = end_sec - (60*60*48);
      rrdfilename = "month_archive.rrd";
      path_flag = ENTPATH_MONTH;
      break;
    case GRAPHPERIOD_DAY:
      start_sec = i_time_daystart (ref_sec, &end_tm);
      start_sec -= (60*60*24);
      end_sec = start_sec + (60*60*48);
      rrdfilename = "month_archive.rrd";
      path_flag = ENTPATH_MONTH;
      break;
    case GRAPHPERIOD_WEEK:
      start_sec = i_time_weekof (ref_sec, &start_tm);
      end_sec = start_sec + (7*60*60*24); 
      rrdfilename = "month_archive.rrd";
      path_flag = ENTPATH_MONTH;
      break;
    case GRAPHPERIOD_MONTH:
      start_sec = i_time_monthstart (ref_sec, &start_tm);
      end_sec = i_time_monthend (ref_sec, &end_tm);
      rrdfilename = "month_archive.rrd";
      path_flag = ENTPATH_MONTH;
      break;
    case GRAPHPERIOD_YEAR:
      start_sec = i_time_yearstart (ref_sec, &start_tm);
      end_sec = i_time_yearend (ref_sec, &end_tm);
      rrdfilename = "year_archive.rrd";
      path_flag = ENTPATH_YEAR;
      break;
    case GRAPHPERIOD_CUSTOM:
      /* start_sec and end_sec must be specified in the XML
       * and the month_archive is always used (duration is expected to be
       * short for rendering tiles)
       */
      ref_sec = start_sec;
      rrdfilename = "month_archive.rrd";
      path_flag = ENTPATH_MONTH;
      break;
    default:
      i_printf (1, "xml_xmlgraph_render unsupported graph period %i specified; no graph rendered", period);
      return -1;
  }

  /* Loop through entities */
  l_xmlgraph_entity *gent;
  for (i_list_move_head(ent_list); (gent=i_list_restore(ent_list))!=NULL; i_list_move_next(ent_list))
  {
    char *min_colour;
    char *avg_colour;
    char *max_colour;
    char *rrdfullpath;
    char *metdefs_str;
    char *metrender_str;

    /* Check if unit_str needs escaping */
    if (!strcmp(gent->unit_str, "%"))
    {
      free (gent->unit_str);
      gent->unit_str = strdup ("%%");
      has_percentages = 1;
    }
    
    /* Check for specified minimum value */
    if (gent->met && gent->met->min_val)
    {
      double met_min = i_metric_valflt(gent->met, gent->met->min_val);
      if (met_min < min_line) min_line = met_min;
      min_line_set = 1;
    }

    /* Check for specified maximum value */
    if (gent->met && gent->met->max_val)
    {
      double met_max = i_metric_valflt(gent->met, gent->met->max_val);
      if (met_max > max_line) max_line = met_max;
      max_line_set = 1;
    }
    
    /* Create fullpath string to RRD file */
    fsroot = i_entity_path_extdev (self, gent->entdesc, ref_sec, path_flag);
    rrdfullpath = i_path_glue (fsroot, rrdfilename);
    free (fsroot);

    /* Set colours */
    switch (metric_index)
    {
      case 0: /* Blue */
        min_colour = "000E73";
        avg_colour = "001EFF";
        max_colour = "00B4FF";
        break;
      case 1: /* Green */
        min_colour = "006B00";
        avg_colour = "009B00";
        max_colour = "00ED00";
        break;
      case 2: /* Red */
        min_colour = "6B0000";
        avg_colour = "9B0000";
        max_colour = "ED0000";
        break;
      case 3: /* Purple */
        min_colour = "620585";
        avg_colour = "8e0abf";
        max_colour = "ba00ff";
        break;
      case 4: /* Yellow */
        min_colour = "6f7304";
        avg_colour = "a7ad04";
        max_colour = "f6ff00";
        break;
      case 5: /* Cyan */
        min_colour = "038a8a";
        avg_colour = "01bcbc";
        max_colour = "00ffff";
        break;
      case 6: /* Orange */
        min_colour = "8a5f03";
        avg_colour = "c38501";
        max_colour = "ffae00";
        break;
      case 7: /* Pink */
        min_colour = "790352";
        avg_colour = "a51c78";
        max_colour = "f80baa";
        break;
      default:
        min_colour = "555555";
        avg_colour = "AAAAAA";
        max_colour = "FFFFFF";
        break;
    }
    if (all_white == 1)
    {
      min_colour = NULL;
      avg_colour = "FFFFFF";
      max_colour = NULL;
    }

    /* Set base (first) metric properties */
    if (metric_index == 0)
    {
      y_label = strdup (gent->unit_str);
      if (gent->met)
      { kbase = gent->met->kbase; }
    }

    /* Increment metric index */
    metric_index++;

    /* Check the RRDtool file exists */
    if (i_rrd_check_exists (self, rrdfullpath) != 0)
    {
      free (rrdfullpath);
      continue; 
    }
    
    /* Check metric is recorded */
    if (!gent->met || gent->met->record_method == RECMETHOD_RRD)
    {
      char *devdesc_esc;
      char *cntdesc_esc;
      char *objdesc_esc;
      char *metdesc_esc;
      
      /* Create and append defs */
      asprintf (&metdefs_str, "DEF:met%i_min='%s':%s:MIN DEF:met%i_avg='%s':%s:AVERAGE DEF:met%i_max='%s':%s:MAX",
        metric_index, rrdfullpath, METRIC_RRD_DATASOURCE,
        metric_index, rrdfullpath, METRIC_RRD_DATASOURCE,
        metric_index, rrdfullpath, METRIC_RRD_DATASOURCE);
      free (rrdfullpath);
      if (rrddefs)
      { 
        asprintf (&temp_str, "%s %s", rrddefs, metdefs_str); 
        free (metdefs_str);
        free (rrddefs);
        rrddefs = temp_str; 
      }
      else
      { rrddefs = metdefs_str; }

      /* Create and append render arguments */
      if (gent->entdesc->dev_desc)
      { devdesc_esc = i_rrd_comment_escape (gent->entdesc->dev_desc); }
      else
      { devdesc_esc = strdup (""); }
      if (gent->entdesc->cnt_desc)
      { cntdesc_esc = i_rrd_comment_escape (gent->entdesc->cnt_desc); }
      else
      { cntdesc_esc = strdup (""); }
      if (gent->entdesc->obj_desc)
      { objdesc_esc = i_rrd_comment_escape (gent->entdesc->obj_desc); }
      else
      { objdesc_esc = strdup (""); }
      if (gent->entdesc->met_desc)
      { metdesc_esc = i_rrd_comment_escape (gent->entdesc->met_desc); }
      else
      { metdesc_esc = strdup (""); }
      if (all_white == 1)
      {
        /* Special handling for all-white graphs, draw avg only */
        asprintf (&metrender_str, "\"LINE1:met%i_avg#%s:Avg.\" ",
          metric_index, avg_colour);
      }
      else
      {
        /* Normal drawing */
        asprintf (&metrender_str, "\"LINE1:met%i_max#%s:Min.\" \"LINE1:met%i_avg#%s:Avg.\" \"LINE1:met%i_max#%s:Max.  %s %s %s \" \"GPRINT:met%i_min:MIN:Min %%.2lf%%s%s\" \"GPRINT:met%i_avg:AVERAGE:Avg %%.2lf%%s%s\" \"GPRINT:met%i_max:MAX:Max %%.2lf%%s%s\\n\" ",
          metric_index, min_colour,
          metric_index, avg_colour,
          metric_index, max_colour,
          cntdesc_esc, objdesc_esc, metdesc_esc,
          metric_index, gent->unit_str,
          metric_index, gent->unit_str,
          metric_index, gent->unit_str);
      }
      if (devdesc_esc) free (devdesc_esc);
      if (cntdesc_esc) free (cntdesc_esc);
      if (objdesc_esc) free (objdesc_esc);
      if (metdesc_esc) free (metdesc_esc);
      if (renderargs)
      {
        asprintf (&temp_str, "%s %s", renderargs, metrender_str);
        free (metrender_str);
        free (renderargs);
        renderargs = temp_str;
      }
      else
      { renderargs = metrender_str; }
    }
  }

  /* Free entity list */
  i_list_free (ent_list);
  
  /* Create min/max line args */
  char *min_max_line_args;
  if (min_line_set && max_line_set) asprintf (&min_max_line_args, "LINE1:%f LINE1:%f", min_line, max_line);
  else if (min_line_set) asprintf (&min_max_line_args, "LINE1:%f", min_line);
  else if (max_line_set) asprintf (&min_max_line_args, "LINE1:%f", max_line);
  else min_max_line_args = strdup("");

  /* Create full RRD args */
  asprintf (&rrdargs, "--imgformat %s --width=%s --height=%s %s %s %s", format, width, height, rrddefs, renderargs, min_max_line_args);
  if (has_percentages && !min_line_set && !max_line_set)
  { 
    char *tmp;
    asprintf (&tmp, "%s LINE1:100 LINE1:0", rrdargs);
    free (rrdargs);
    rrdargs = tmp;
  }
  free (rrddefs);
  free (renderargs);
  free (height);
  free (width);
  free (min_max_line_args);

  /* Create image filename */
  asprintf (&imagefile, "%i--XXXXXX", getpid());
  imagefile = mktemp (imagefile);
  asprintf (&temp_str, "%s.%s", imagefile, format);
  free (format);
  free (imagefile);
  imagefile = temp_str;

  /* Create return XML */
  xmlreq->xml_out = i_xml_create ();
  xmlreq->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "xmlgraph");
  xmlDocSetRootElement (xmlreq->xml_out->doc, root_node);

  /* Add filename to xml */
  xmlNewChild (root_node, NULL, BAD_CAST "imagefile", BAD_CAST imagefile);

  /* Create full (filesystem) path to image */
  asprintf (&imagefullpath, "/Library/Application Support/Lithium/ClientService/Resources/htdocs/%s/image_cache/%s", self->hierarchy->cust->name_str, imagefile); 
  free (imagefile);

  /* Render graph */
  cmd = i_rrd_xmlgraph (self, imagefullpath, start_sec, end_sec, y_label, kbase, rrdargs, l_xmlgraph_xml_render_graphcb, xmlreq);
  free (imagefullpath);
  free (rrdargs);
  if (y_label) free (y_label);
  if (!cmd)
  {
    i_printf (1, "xml_xmlgraph_render failed to render graph");
    return -1;
  }

  return 0;
}

int l_xmlgraph_xml_render_graphcb (i_resource *self, i_rrdtool_cmd *cmd, int result, void *passdata)
{
  i_xml_request *xmlreq = passdata;

  /* Add Output to XML */
  if (cmd->output_str)
  {
    unsigned int i;
    char *parsed_str = strdup (cmd->output_str);
    for (i=0; i < strlen(parsed_str); i++)
    {
      if (parsed_str[i] == '\n')
      { parsed_str[i] = ','; }
    }
    xmlNodePtr root_node = xmlDocGetRootElement (xmlreq->xml_out->doc);
    xmlNewChild (root_node, NULL, BAD_CAST "output", BAD_CAST parsed_str);
    free (parsed_str);
  }

  /* Deliver xml */
  i_xml_deliver (self, xmlreq);

  return 0;
}
