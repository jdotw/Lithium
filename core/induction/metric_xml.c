#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <libxml/parser.h>

#include "induction.h"
#include "entity.h"
#include "entity_xml.h"
#include "message.h"
#include "socket.h"
#include "data.h"
#include "msgproc.h"
#include "auth.h"
#include "list.h"
#include "respond.h"
#include "incident.h"
#include "metric.h"
#include "resource_xml.h"
#include "incident_xml.h"
#include "xml.h"
#include "metric_xml.h"

/*
 * Object-related XML functions
 */

/** \addtogroup metric Metrics
 * @{
 */

/* XML Functions */

void i_metric_xml (i_entity *ent, xmlNodePtr ent_node)
{
  /* Add object-specific data to entity node */
  char *str;
  i_metric *met = (i_metric *) ent;

  /* Type info */
  asprintf (&str, "%u", met->met_type);
  xmlNewChild (ent_node, NULL, BAD_CAST "met_type_int", BAD_CAST str);
  free (str);
  asprintf (&str, "%lu", met->alloc_unit);
  xmlNewChild (ent_node, NULL, BAD_CAST "alloc_unit", BAD_CAST str);
  free (str);
  /* FIX Doesnt handle alloc_unit_met */
  xmlNewChild (ent_node, NULL, BAD_CAST "units", BAD_CAST met->unit_str);
  asprintf (&str, "%u", met->kbase);
  xmlNewChild (ent_node, NULL, BAD_CAST "kbase", BAD_CAST str);
  free (str);

  /* Enum list */
//  for (i_list_move_head(met->enumstr_list); (enumstr=i_list_restore(met->enumstr_list))!=NULL; i_list_move_next(met->enumstr_list))
//  {
//    node = i_metric_enumstr_xml (enumstr);
//    if (node) xmlAddChild (ent_node, node);
//  }

  /* Recording */
  asprintf (&str, "%u", met->record_method);
  xmlNewChild (ent_node, NULL, BAD_CAST "record_method", BAD_CAST str);
  free (str);
  asprintf (&str, "%u", met->record_enabled);
  xmlNewChild (ent_node, NULL, BAD_CAST "record_enabled", BAD_CAST str);
  free (str);

  /* Time stamps */
  asprintf (&str, "%li", met->lrefresh_tv.tv_sec);
  xmlNewChild (ent_node, NULL, BAD_CAST "lastrefresh_sec", BAD_CAST str);
  free (str);
//  asprintf (&str, "%li", met->lrefresh_tv.tv_usec);
//  xmlNewChild (ent_node, NULL, BAD_CAST "lastrefresh_usec", BAD_CAST str);
//  free (str);
  asprintf (&str, "%li", met->lnormal_tv.tv_sec);
  xmlNewChild (ent_node, NULL, BAD_CAST "lastnormal_sec", BAD_CAST str);
  free (str);
//  asprintf (&str, "%li", met->lnormal_tv.tv_usec);
//  xmlNewChild (ent_node, NULL, BAD_CAST "lastnormal_usec", BAD_CAST str);
//  free (str);
  asprintf (&str, "%li", met->lvalchange_tv.tv_sec);
  xmlNewChild (ent_node, NULL, BAD_CAST "lastvalchange_sec", BAD_CAST str);
  free (str);
//  asprintf (&str, "%li", met->lvalchange_tv.tv_usec);
//  xmlNewChild (ent_node, NULL, BAD_CAST "lastvalchange_usec", BAD_CAST str);
//  free (str);
  asprintf (&str, "%li", met->lstatechange_tv.tv_sec);
  xmlNewChild (ent_node, NULL, BAD_CAST "laststatechange_sec", BAD_CAST str);
  free (str);
//  asprintf (&str, "%li", met->lstatechange_tv.tv_usec);
//  xmlNewChild (ent_node, NULL, BAD_CAST "laststatechange_usec", BAD_CAST str);
//  free (str);

  /* Minimum Value */
  if (met->min_val)
  {
    char *min_val_str = i_metric_valstr_raw (met, met->min_val);
    xmlNewChild (ent_node, NULL, BAD_CAST "min_valstr", BAD_CAST min_val_str);
    free (min_val_str);
  }

  /* Maximum Value */
  if (met->max_val)
  {
    char *max_val_str = i_metric_valstr_raw (met, met->max_val);
    xmlNewChild (ent_node, NULL, BAD_CAST "max_valstr", BAD_CAST max_val_str);
    free (max_val_str);
  }

  /* Trigger Presences */
  if (met->trg_list && met->trg_list->size)
  { xmlNewChild (ent_node, NULL, BAD_CAST "has_triggers", BAD_CAST "1"); }
  else
  { xmlNewChild (ent_node, NULL, BAD_CAST "has_triggers", BAD_CAST "0"); }

  /* Summary */
  asprintf (&str, "%u", met->summary_flag);
  xmlNewChild (ent_node, NULL, BAD_CAST "summary_flag", BAD_CAST str);
  free (str);
  
  /* Values */
  xmlNodePtr node = NULL;
  asprintf (&str, "%u", METRIC_DEF_MAXVAL);
  xmlNewChild (ent_node, NULL, BAD_CAST "val_list_maxsize", BAD_CAST str);
  free (str);
  asprintf (&str, "%f", met->aggregate_delta);
  xmlNewChild (ent_node, NULL, BAD_CAST "aggregate_delta", BAD_CAST str);
  free (str);
  i_list *recent_values = i_list_create ();
  int recent_count = 0;
  i_metric_value *val;
  for (i_list_move_head(met->val_list); (val=i_list_restore(met->val_list))!=NULL; i_list_move_next(met->val_list))
  {
    if (recent_count < METRIC_DEF_MAXVAL)
    {
      i_list_enqueue (recent_values, val);
      recent_count++;
    }
    else
    { break; }
  }
  for (i_list_move_last(recent_values); (val=i_list_restore(recent_values))!=NULL; i_list_move_prev(recent_values))
  {
      node = i_metric_value_xml (met, val);
      if (node) xmlAddChild (ent_node, node);
  }
  i_list_free (recent_values);

  return;
}

void i_metric_xml_summary (i_entity *ent, xmlNodePtr ent_node) 
{
  /* Add summary metric info */
  char *str;
  i_metric *met = (i_metric *) ent;

  /* Recording */
  asprintf (&str, "%u", met->record_method);
  xmlNewChild (ent_node, NULL, BAD_CAST "record_method", BAD_CAST str);
  free (str);
  asprintf (&str, "%u", met->record_enabled);
  xmlNewChild (ent_node, NULL, BAD_CAST "record_enabled", BAD_CAST str);
  free (str);

  /* Units */
  xmlNewChild (ent_node, NULL, BAD_CAST "units", BAD_CAST met->unit_str);

  /* Add one value, Max Vals and Aggregate Delta */
  asprintf (&str, "%u", METRIC_DEF_MAXVAL);
  xmlNewChild (ent_node, NULL, BAD_CAST "val_list_maxsize", BAD_CAST str);
  free (str);
  asprintf (&str, "%f", met->aggregate_delta);
  xmlNewChild (ent_node, NULL, BAD_CAST "aggregate_delta", BAD_CAST str);
  free (str);
  i_list_move_head(met->val_list);
  i_metric_value *val=i_list_restore(met->val_list);
  if (val)
  {
      xmlNodePtr node = i_metric_value_xml (met, val);
      if (node) xmlAddChild (ent_node, node);
  }
}

xmlNodePtr i_metric_value_xml (i_metric *met, i_metric_value *val)
{
  char *str;
  xmlNodePtr val_node;

  /* Create val_node */
  val_node = xmlNewNode (NULL, BAD_CAST "value");

  /* Value */
  str = i_metric_valstr (met, val);
  xmlNewChild (val_node, NULL, BAD_CAST "valstr", BAD_CAST str);
  free (str);
  str = i_metric_valstr_raw (met, val);
  xmlNewChild (val_node, NULL, BAD_CAST "valstr_raw", BAD_CAST str);
  free (str);
  
  /* Timestamp */
  asprintf (&str, "%li", val->tstamp.tv_sec);
  xmlNewChild (val_node, NULL, BAD_CAST "tstamp_sec", BAD_CAST str);
  free (str);
  
  return val_node;
}

xmlNodePtr i_metric_enumstr_xml (i_metric_enumstr *enumstr)
{
  char *str;
  xmlNodePtr enum_node;

  /* Create val_node */
  enum_node = xmlNewNode (NULL, BAD_CAST "enumstr");

  /* Enumstr */
  asprintf (&str, "%i", enumstr->val_int);
  xmlNewChild (enum_node, NULL, BAD_CAST "val_int", BAD_CAST str);
  free (str);
  xmlNewChild (enum_node, NULL, BAD_CAST "str", BAD_CAST enumstr->str);

  return enum_node;
}

/* @} */
