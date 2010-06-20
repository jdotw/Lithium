#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <libxml/parser.h>
#include <string.h>

#include "induction.h"
#include "entity.h"
#include "entity_xml.h"
#include "entity_xmlsync.h"
#include "message.h"
#include "socket.h"
#include "data.h"
#include "msgproc.h"
#include "auth.h"
#include "list.h"
#include "respond.h"
#include "incident.h"
#include "resource_xml.h"
#include "incident_xml.h"
#include "timer.h"
#include "xml.h"
#include "site.h"
#include "site_xml.h"
#include "device.h"
#include "device_xml.h"
#include "container.h"
#include "container_xml.h"
#include "object.h"
#include "object_xml.h"
#include "metric.h"
#include "metric_xml.h"
#include "trigger.h"
#include "trigger_xml.h"
#include "memcheck.h"

/** \addtogroup entity Monitored Entities
 * @{
 */

/*
 * Entity XML-Sync functions
 */

static float static_xmlsync_rssmem = 0.0;

/* XML Sync */

int i_entity_xmlsync_parse_entity_node (i_resource *self, i_entity *parent, i_entity *entity, i_xml *xml, xmlNodePtr parent_node, long sync_tag, int (*delegate) ())
{
  time_t received_version = 0;
  int operation = XMLSYNC_SYNC;
  xmlNodePtr node;

  if (entity)
  { entity->xmlsync_tag = sync_tag; }

  for (node = parent_node->children; node; node = node->next)
  {
    char *str;
	
    str = (char *) xmlNodeListGetString (xml->doc, node->xmlChildrenNode, 1);

	if (!strcmp((char *)node->name, "entity")) 
	{
      /* Recursively call to handle child */
      i_entity_xmlsync_parse_entity_node (self, entity, NULL, xml, node, sync_tag, delegate);
    }
    else if (operation == XMLSYNC_NOCHANGE)
    { 
      /* Entity has been confirmed as in-sync, do not
       * perform any further parsing 
       */
	  xmlFree (str);
      continue;
    }
    else if (!strcmp((char *)node->name, "version") && str)
    {
      received_version = atol (str);
    }
	else if (!strcmp((char *)node->name, "name") && str && !entity && parent) 
	{
      /* Try to find existing */
	  entity = i_entity_child_get (parent, str);
	  if (!entity)
	  {
		/* No existing entity found, create new */
        size_t entity_size;
		switch (parent->ent_type)
		{
			case 1:
			  entity_size = sizeof (i_site);
			  break;
			case 2:
			  entity_size = sizeof (i_device);
			  break;
			case 3:
			  entity_size = sizeof (i_container);
			  break;
			case 4:
			  entity_size = sizeof (i_object);
			  break;
			case 5:
			  entity_size = sizeof (i_metric);
			  break;
			case 6:
			  entity_size = sizeof (i_trigger);
			  break;
			default:
			  entity_size = sizeof (i_entity);
        }
        entity = i_entity_create (str, str, parent->ent_type+1, entity_size);
        entity->authorative = 0;
        operation = XMLSYNC_NEW;

        /* Entity-Specific setup */
        if (entity->ent_type == ENT_METRIC)
        {
          i_metric *met = (i_metric *) entity;
          met->val_list = i_list_create ();
          i_list_set_destructor (met->val_list, i_metric_value_free);
        }
      }
      else
      {
        /* Existing Entity */
        if (received_version <= entity->xmlsync_version)
        {
          /* No change in this entity */
          operation = XMLSYNC_NOCHANGE;
        }
      }

      /* Set version info */
      entity->version = received_version; 
      entity->xmlsync_version = received_version;

      /* Set sync tag */
      entity->xmlsync_tag = sync_tag;
	}
	else if (!strcmp((char *)node->name, "desc") && str && entity) 
    { 
      if (entity->desc_str) free (entity->desc_str); 
      entity->desc_str = strdup(str); 

      if (!entity->registered)
      {
        i_entity_register (self, parent, entity);
      }
    }
	else if (!strcmp((char *)node->name, "adminstate_num") && str && entity) 
	{ entity->adminstate = atoi (str); }
	else if (!strcmp((char *)node->name, "opstate_num") && str && entity) 
	{ entity->opstate = atoi (str); }
	else if (!strcmp((char *)node->name, "prio") && str && entity) 
	{ entity->prio = atol (str); }
	else if (!strcmp((char *)node->name, "refresh_result") && str && entity) 
    { entity->refresh_result = atoi (str); }
    else if (!strcmp((char *)node->name, "sync_version") && str && entity)
    { entity->xmlsync_version = atol (str); }

	else if (!strcmp((char *)node->name, "resaddr") && str && entity) 
	{
	  i_resource_address_free (entity->resaddr);
	  entity->resaddr = i_resource_address_string_to_struct (str);
    }
    else if (!strcmp((char *)node->name, "value") && entity)
    {
      /* Metric Value */
      i_metric *metric = (i_metric *)entity;
      i_metric_value *val = i_metric_value_create ();
      xmlNodePtr valuenode;
      for (valuenode = node->children; valuenode; valuenode = valuenode->next)
      {
        char *valnode_str = (char *) xmlNodeListGetString (xml->doc, valuenode->xmlChildrenNode, 1);
        if (!strcmp((char *)valuenode->name, "valstr_raw") && valnode_str)
        { i_metric_valstr_set (metric, val, valnode_str); }
        else if (!strcmp((char *)valuenode->name, "tstamp_sec") && valnode_str)
        { 
          val->tstamp.tv_sec = atol (valnode_str); 
          val->tstamp.tv_usec = 0; 
        }
        xmlFree (valnode_str);
      }
      i_metric_value_enqueue (self, metric, val);
    }
	else if (!strcmp((char *)node->name, "alloc_unit") && str && entity) 
    { 
      i_metric *metric = (i_metric *)entity;
      metric->alloc_unit = atol (str); 
    }
	else if (!strcmp((char *)node->name, "met_type_int") && str && entity) 
    { 
      i_metric *metric = (i_metric *)entity;
      metric->met_type = atoi (str); 
    }
	else if (!strcmp((char *)node->name, "units") && str && entity) 
    { 
      i_metric *metric = (i_metric *)entity;
      if (metric->unit_str) free (metric->unit_str);
      metric->unit_str = strdup(str);
    }
	else if (!strcmp((char *)node->name, "kbase") && str && entity) 
    { 
      i_metric *metric = (i_metric *)entity;
      metric->kbase = atoi (str); 
    }
	else if (!strcmp((char *)node->name, "record_enabled") && str && entity) 
    { 
      i_metric *metric = (i_metric *)entity;
      metric->record_enabled = atoi (str); 
    }
	else if (!strcmp((char *)node->name, "record_method") && str && entity) 
    { 
      i_metric *metric = (i_metric *)entity;
      metric->record_method = atoi (str); 
    }
	else if (!strcmp((char *)node->name, "aggregate_delta") && str && entity) 
    { 
      i_metric *metric = (i_metric *)entity;
      metric->aggregate_delta = atof (str); 
    }
	else if (!strcmp((char *)node->name, "val_list_maxsize") && str && entity) 
    { 
      i_metric *metric = (i_metric *)entity;
      metric->val_list_maxsize = atoi (str); 
    }
	
	xmlFree (str);
  }

  /* Check to make sure the entity was registered (and wont leak) */
  if (entity && !entity->registered)
  {
    i_printf (2, "i_entity_xmlsync_parse_entity_node error, did not register %i:%s belonging to %i:%s -- freeing entity", 
      entity->ent_type, entity->name_str, parent ? parent->ent_type : 0, parent ? parent->name_str : NULL);
    i_entity_free (entity);
    entity = NULL;
  }

  /* Check for entity obsolescence */
  if (entity)
  {
    i_entity *child;
    for (i_list_move_head(entity->child_list); (child=i_list_restore(entity->child_list))!=NULL; i_list_move_next(entity->child_list))
    {
      if (child->xmlsync_tag != sync_tag)
      {
        /* Entity is obsolete */
        if (delegate)
        { delegate (self, XMLSYNC_OBSOLETE, child); }
        i_entity_deregister (self, child);
        i_entity_free (child);
      }
    }
  }

  /* Call delegate */
  if (delegate && entity)
  { delegate (self, operation, entity); }

  return 0; 
}

int i_entity_xmlsync_xmlcb (i_resource *self, i_xml *xml, int result, void *passdata)
{
  /* Called when the XML has been retrieved 
   * from the remote resource
   */

  i_entity *entity = passdata;

  /* Check for XML Response */
  if (xml)
  {
    /* Performance logging */
    float rss_start = i_memcheck_rss ();
    struct timeval tv_start;
    gettimeofday (&tv_start, NULL);
    
    /* Parse */
    struct timeval now;
    gettimeofday (&now, NULL);
    long sync_tag = random ();
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;
    root_node = xmlDocGetRootElement (xml->doc);

    if (root_node)
    {
      /* Parse the XML */
      for (node = root_node->children; node; node = node->next)
      {
        if (!strcmp((char *)node->name, "entity"))
        { 
          i_entity_xmlsync_parse_entity_node (self, NULL, entity, xml, node, sync_tag, entity->xmlsync_delegate);
        }
      }
    }
    else 
    { i_printf (1, "i_entity_xmlsync_xmlcb failed, no root node present in XML"); }

    /* Performance Logging */
    float rss_end = i_memcheck_rss ();
    struct timeval tv_end;
    gettimeofday (&tv_end, NULL);
    static_xmlsync_rssmem = static_xmlsync_rssmem + (rss_end - rss_start);
    if (self->perflog) i_printf (0, "PERF: i_entity_xmlsync_xmlcb after parse of XML from %i:%s(%s) took %i seconds RSS usage increased %.2fKbytes (total now %.0fkB / %.0fkB = %.1f%%)", 
      entity->ent_type, entity->name_str, entity->desc_str, 
      tv_end.tv_sec - tv_start.tv_sec, rss_end - rss_start,
      static_xmlsync_rssmem, rss_end, (static_xmlsync_rssmem / rss_end) * 100.0);
  }
  else
  { i_printf (1, "i_entity_xmlsync_xmlcb failed, no XML received"); }

  entity->xmlsync_req = NULL;
  
  return 0;  
}

int i_entity_xmlsync (i_resource *self, i_entity *entity)
{
  /* Synchronises the local non-authoritize entity tree 
   * with the authorative remote resource. 
   */

  /* Perform XML Get */
  if (entity->xmlsync_req && entity->xmlsync_collisions < 20)
  {
    entity->xmlsync_collisions++;
    i_printf (1, "i_entity_xmlsync %i:%s warning, sync attempt already in progress (%i consecutive collisions)", entity->ent_type, entity->name_str, entity->xmlsync_collisions); 
  }
  else
  {
    if (entity->xmlsync_req)
    { i_printf (1, "i_entity_xmlsync %i:%s warning %i sync collissions occurred, restarting xmlsync", entity->ent_type, entity->name_str, entity->xmlsync_collisions); }
    entity->xmlsync_collisions = 0;

    if (entity->resaddr)
    {

      /* Create XML Request with sync version */
      i_xml *xml_out = i_xml_create ();
      xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
      xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "entity_sync");
      xmlDocSetRootElement (xml_out->doc, root_node);
      char *str;
      asprintf (&str, "%li", entity->xmlsync_version);
      xmlNewChild (root_node, NULL, BAD_CAST "sync_version", BAD_CAST str);
      free (str);

      /* Request XML */
      entity->xmlsync_req = i_xml_get (self, entity->resaddr, ENT_ADDR(entity), "entity_tree", 0, xml_out, i_entity_xmlsync_xmlcb, entity); 
      i_xml_free (xml_out);
    }
    else
    { entity->xmlsync_req = NULL; }
  }
  
  return 0;
}

int i_entity_xmlsync_timercb (i_resource *self, i_timer *timer, void *passdata)
{
  i_entity *entity = passdata;
  return i_entity_xmlsync (self, entity);
}

int i_entity_xmlsync_enable (i_resource *self, i_entity *entity, time_t refresh_interval, int (*delegate) (i_resource *self, int operation, i_entity *entity))
{
  entity->xmlsync_delegate = delegate;
  entity->xmlsync_timer = i_timer_add (self, refresh_interval, 0, i_entity_xmlsync_timercb, entity);
  return 0;
}

int i_entity_xmlsync_disable (i_resource *self, i_entity *entity)
{
  i_timer_remove (entity->xmlsync_timer);
  return 0;
}


/* @} */
