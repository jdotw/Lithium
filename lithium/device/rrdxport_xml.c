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

#include "rrdxport.h"

int xml_rrdxport (i_resource *self, i_xml_request *xmlreq)
{
  int period = 0;
  time_t ref_sec = 0;
  time_t start_sec;
  time_t end_sec;
  struct tm ref_tm;
  struct tm start_tm;
  struct tm end_tm;
  struct timeval now_tv;
  char *rrdfilename;
  char *fsroot;
  i_rrdtool_cmd *cmd;
  unsigned short path_flag;
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;
  i_entity_descriptor *entdesc = NULL;

  /* Get ref_sec, period and entity descriptor from XML */
  if (xmlreq->xml_in)
  {
    /* Iterate through XML */
    root_node = xmlDocGetRootElement (xmlreq->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;
      
      if (!strcmp((char *)node->name, "entity_descriptor"))
      { 
        /* Clear existing */
        if (entdesc) i_entity_descriptor_free (entdesc);

        /* Create entity descriptor */
        entdesc = i_entity_descriptor_fromxml (xmlreq->xml_in, node);
        if (!entdesc)
        { i_printf (1, "xml_rrdxport failed to convert an entity from xml"); continue; }

        /* Move on */
        continue;
      }
      
      str = (char *) xmlNodeListGetString (xmlreq->xml_in->doc, node->xmlChildrenNode, 1);

      if (!strcmp((char *)node->name, "ref_sec")) ref_sec = (time_t) atol (str);
      else if (!strcmp((char *)node->name, "period")) period = atoi (str);

      xmlFree (str);
    }
  } 
  else
  {
    i_printf (1, "xml_xmlgraph_render error, no xml data received");
    return -1;
  }

  /* Set period-specific variables */
  localtime_r (&ref_sec, &ref_tm);
  switch (period)
  {
    case GRAPHPERIOD_LAST48HR:
      gettimeofday (&now_tv, NULL);
      end_sec = now_tv.tv_sec;
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
    default:
      i_printf (1, "xml_xmlgraph_render unsupported graph period %i specified; no graph rendered", period);
      return -1;
  }

  /* Loop through entities */
  i_metric *met;
  char *rrdfullpath;
    
  /* Get metric */
  met = (i_metric *) i_entity_local_get (self, (i_entity_address *)entdesc);
  if (!met)
  { i_printf (1, "xml_rrdxport failed to find local entity for metric"); return 1; }
  if (met->record_method != RECMETHOD_RRD) return 1;

  /* Create fullpath string to RRD file */
  fsroot = i_entity_path (self, ENTITY(met), ref_sec, path_flag);
  rrdfullpath = i_path_glue (fsroot, rrdfilename);
  free (fsroot);

  /* Call export */
  cmd = i_rrd_xport (self, start_sec, end_sec, rrdfullpath, l_rrdxport_xportcb, xmlreq);
  if (!cmd)
  {
    i_printf (1, "xml_rrdxport failed to render graph");
    return -1;
  }

  return 0;
}

int l_rrdxport_xportcb (i_resource *self, i_rrdtool_cmd *cmd, int result, void *passdata)
{
  i_xml_request *xmlreq = passdata;

  /* Copy result */
  if (cmd->output_str)
  {
    xmlreq->xml_out = i_xml_create ();
    char *xml_start = strstr (cmd->output_str, "<?xml");
    if (xml_start) xmlreq->xml_out->raw_data = strdup (xml_start); 
  }

  /* Deliver xml */
  i_xml_deliver (self, xmlreq);

  return 0;
}
