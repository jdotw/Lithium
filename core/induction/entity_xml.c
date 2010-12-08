#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <libxml/parser.h>
#include <string.h>

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

/** \addtogroup entity Monitored Entities
 * @{
 */

/*
 * Entity-related XML functions
 */

extern i_resource* global_self;

/* XML Functions */

xmlNodePtr i_entity_xml (i_entity *ent, unsigned short flags, time_t sync_version)
{
  /* Creates an XML tree for the specified entity */

  char *str;
  xmlNodePtr ent_node;
  xmlNodePtr node;
  int add_detail = 1;
  int add_summary = 1;

  /* Create entity node */
  ent_node = xmlNewNode (NULL, BAD_CAST "entity");

  /* Check on detail level */
  if (flags & ENTXML_TREE_SUMMARY && !ent->authorative)
  { add_detail = 0;  }

  /* Check on sync */
  
  if (flags & ENTXML_SYNC && ent->version < sync_version)
  {
    /* Entity hasn't changed since last sync */
    add_detail = 0;
    add_summary = 0; 
  }

  /* 
   * Essential info 
   */

  asprintf (&str, "%li", ent->version);
  xmlNewChild (ent_node, NULL, BAD_CAST "version", BAD_CAST str);         /* Actual version */
  free (str); 
  if (ent->name_str) xmlNewChild (ent_node, NULL, BAD_CAST "name", BAD_CAST ent->name_str);
  char uuid_buf[37];
  uuid_unparse_lower (ent->uuid, uuid_buf);
  xmlNewChild (ent_node, NULL, BAD_CAST "uuid", BAD_CAST uuid_buf);

  /*
   * Summary info 
   */

  if (add_summary)
  {
    if (ent->desc_str) xmlNewChild (ent_node, NULL, BAD_CAST "desc", BAD_CAST ent->desc_str);
    asprintf (&str, "%u", ent->ent_type);
    xmlNewChild (ent_node, NULL, BAD_CAST "type_num", BAD_CAST str);
    free (str); 
    asprintf (&str, "%i", ent->adminstate);
    xmlNewChild (ent_node, NULL, BAD_CAST "adminstate_num", BAD_CAST str);
    free (str); 
    asprintf (&str, "%i", ent->opstate);
    xmlNewChild (ent_node, NULL, BAD_CAST "opstate_num", BAD_CAST str);
    free (str);
  }

  /*
   * Detailed Info
   */

  if (add_detail == 1)
  {
    /* Priority */
    asprintf (&str, "%lu", ent->prio);
    xmlNewChild (ent_node, NULL, BAD_CAST "prio", BAD_CAST str);
    free (str);

    /* Resource */
    char *resaddrstr = NULL;
    if (ent->ent_type == 1 || ent->ent_type == 3)
    { resaddrstr = i_resource_address_struct_to_string (ent->resaddr); }
    else if (ent->ent_type == 2)
    { resaddrstr = i_resource_address_struct_to_string (ent->parent->resaddr); }
    else if (ent->ent_type == 4)
    { resaddrstr = i_resource_address_struct_to_string (ent->parent->resaddr); }
    else if (ent->ent_type == 5)
    { resaddrstr = i_resource_address_struct_to_string (ent->parent->parent->resaddr); }
    else if (ent->ent_type == 6)
    { resaddrstr = i_resource_address_struct_to_string (ent->parent->parent->parent->resaddr); }
    else if (ent->ent_type == 7)
    { resaddrstr = i_resource_address_struct_to_string (ent->parent->parent->parent->parent->resaddr); }
    if (resaddrstr)
    { xmlNewChild (ent_node, NULL, BAD_CAST "resaddr", BAD_CAST resaddrstr); free (resaddrstr); }

    /* Refresh info */
    if (ent->ent_type == 3 || ent->ent_type == 6)
    {
      /* For Device and Metric only */
      asprintf (&str, "%u", ent->refresh_method);
      xmlNewChild (ent_node, NULL, BAD_CAST "refresh_method", BAD_CAST str);
      free (str);
      asprintf (&str, "%li", ent->refresh_int_sec);
      xmlNewChild (ent_node, NULL, BAD_CAST "refresh_int_sec", BAD_CAST str);
      free (str);
      asprintf (&str, "%u", ent->refresh_maxcolls);
      xmlNewChild (ent_node, NULL, BAD_CAST "refresh_maxcolls", BAD_CAST str);
      free (str);
      asprintf (&str, "%u", ent->refresh_state);
      xmlNewChild (ent_node, NULL, BAD_CAST "refresh_state", BAD_CAST str);
      free (str);
      asprintf (&str, "%u", ent->refresh_colls);
      xmlNewChild (ent_node, NULL, BAD_CAST "refresh_colls", BAD_CAST str);
      free (str);
      asprintf (&str, "%u", ent->refresh_result);
      xmlNewChild (ent_node, NULL, BAD_CAST "refresh_result", BAD_CAST str);
      free (str);
      asprintf (&str, "%li", ent->refresh_tstamp.tv_sec);
      xmlNewChild (ent_node, NULL, BAD_CAST "refresh_tstamp_sec", BAD_CAST str);
      free (str);
      asprintf (&str, "%li", ent->refresh_tstamp.tv_usec);
      xmlNewChild (ent_node, NULL, BAD_CAST "refresh_tstamp_usec", BAD_CAST str);
      free (str);
    }

    /* Entity-specifc XML */
    switch (ent->ent_type)
    {
      case ENT_CUSTOMER:
        break;
      case ENT_SITE:
        i_site_xml (ent, ent_node);
        break;
      case ENT_DEVICE:
        i_device_xml (ent, ent_node);
        break;
      case ENT_CONTAINER:
        i_container_xml (ent, ent_node);
        break;
      case ENT_OBJECT:
        i_object_xml (ent, ent_node);
        break;
      case ENT_METRIC:
        i_metric_xml (ent, ent_node);
        break;
      case ENT_TRIGGER:
        i_trigger_xml (ent, ent_node);
        break;
    }

  }
  else if (add_summary)
  {
    /* Special-case summary info */
    if (ent->ent_type == ENT_METRIC)
    { i_metric_xml_summary (ent, ent_node); }
  }

  /* Add local entity-specific handling */
  if (ent->xml_func)
  { ent->xml_func (ent, ent_node); }
  
  /* Children */
  if (flags & ENTXML_TREE || flags & ENTXML_TREE_ONE_LEVEL || flags & ENTXML_TREE_AUTHORATIVE || flags & ENTXML_TREE_SUMMARY)
  {
    i_entity *child;
    for (i_list_move_head(ent->child_list); (child=i_list_restore(ent->child_list))!=NULL; i_list_move_next(ent->child_list))
    {
      /* Check for hidden */
      if (child->hidden) continue;

      /* If the child is a container, check objects are present */
      if (child->ent_type == ENT_CONTAINER && (!child->child_list || child->child_list->size < 1)) continue;

      /* Check authorative */
      if (flags & ENTXML_TREE_AUTHORATIVE && !child->authorative)
      { 
        continue; 
      }

      /* Adjust flags */
      if (flags & ENTXML_TREE_ONE_LEVEL)
      {
        if (ent->child_list->size == 1)
        { flags = ENTXML_TREE_ONE_LEVEL; }
        else
        { flags = 0; }
      }

      /* Add Child */
      node = i_entity_xml (child, flags, sync_version);
      if (node) xmlAddChild (ent_node, node); 
    }
  }

  return ent_node;
}

/* Entity descriptor */

xmlNodePtr i_entity_descriptor_xml (i_entity_descriptor *entdesc)
{
  char *str;
  xmlNodePtr ent_node;

  ent_node = xmlNewNode (NULL, BAD_CAST "entity_descriptor");

  asprintf (&str, "%u", entdesc->type);
  xmlNewChild (ent_node, NULL, BAD_CAST "type_num", BAD_CAST str);
  free (str);

  if (entdesc->name_str)
  {
    xmlNewChild (ent_node, NULL, BAD_CAST "name", BAD_CAST entdesc->name_str);
    xmlNewChild (ent_node, NULL, BAD_CAST "desc", BAD_CAST (entdesc->desc_str ? : entdesc->name_str));
  }
  else
  {
    switch (entdesc->type)
    {
      case 1:
        xmlNewChild (ent_node, NULL, BAD_CAST "name", BAD_CAST entdesc->cust_name);
        xmlNewChild (ent_node, NULL, BAD_CAST "desc", BAD_CAST (entdesc->cust_desc ? : entdesc->cust_name));
        break;
      case 2:
        xmlNewChild (ent_node, NULL, BAD_CAST "name", BAD_CAST entdesc->site_name);
        xmlNewChild (ent_node, NULL, BAD_CAST "desc", BAD_CAST (entdesc->site_desc ? : entdesc->site_name));
        break;
      case 3:
        xmlNewChild (ent_node, NULL, BAD_CAST "name", BAD_CAST entdesc->dev_name);
        xmlNewChild (ent_node, NULL, BAD_CAST "desc", BAD_CAST (entdesc->dev_desc ? : entdesc->dev_name));
        break;
      case 4:
        xmlNewChild (ent_node, NULL, BAD_CAST "name", BAD_CAST entdesc->cnt_name);
        xmlNewChild (ent_node, NULL, BAD_CAST "desc", BAD_CAST (entdesc->cnt_desc ? : entdesc->cnt_name));
        break;
      case 5:
        xmlNewChild (ent_node, NULL, BAD_CAST "name", BAD_CAST entdesc->obj_name);
        xmlNewChild (ent_node, NULL, BAD_CAST "desc", BAD_CAST (entdesc->obj_desc ? : entdesc->obj_name));
        break;
      case 6:
        xmlNewChild (ent_node, NULL, BAD_CAST "name", BAD_CAST entdesc->met_name);
        xmlNewChild (ent_node, NULL, BAD_CAST "desc", BAD_CAST (entdesc->met_desc ? : entdesc->met_name));
        break;
      case 7:
        xmlNewChild (ent_node, NULL, BAD_CAST "name", BAD_CAST entdesc->trg_name);
        xmlNewChild (ent_node, NULL, BAD_CAST "desc", BAD_CAST (entdesc->trg_desc ? : entdesc->trg_name));
        break;
    }
  }
  asprintf (&str, "%u", entdesc->adminstate);
  xmlNewChild (ent_node, NULL, BAD_CAST "adminstate_num", BAD_CAST str);
  free (str);
  asprintf (&str, "%u", entdesc->opstate);
  xmlNewChild (ent_node, NULL, BAD_CAST "opstate_num", BAD_CAST str);
  free (str);
  if (entdesc->resaddr) 
  {
    char *resaddrstr = i_resource_address_struct_to_string (entdesc->resaddr);
    if (resaddrstr)
    { xmlNewChild (ent_node, NULL, BAD_CAST "resaddr", BAD_CAST resaddrstr); free (resaddrstr); }
  }

  if (entdesc->cust_name) xmlNewChild (ent_node, NULL, BAD_CAST "cust_name", BAD_CAST entdesc->cust_name);
  if (entdesc->cust_desc) xmlNewChild (ent_node, NULL, BAD_CAST "cust_desc", BAD_CAST entdesc->cust_desc);
  if (entdesc->site_name) xmlNewChild (ent_node, NULL, BAD_CAST "site_name", BAD_CAST entdesc->site_name);
  if (entdesc->site_desc) xmlNewChild (ent_node, NULL, BAD_CAST "site_desc", BAD_CAST entdesc->site_desc);
  if (entdesc->site_suburb) xmlNewChild (ent_node, NULL, BAD_CAST "site_suburb", BAD_CAST entdesc->site_suburb);
  if (entdesc->dev_name) xmlNewChild (ent_node, NULL, BAD_CAST "dev_name", BAD_CAST entdesc->dev_name);
  if (entdesc->dev_desc) xmlNewChild (ent_node, NULL, BAD_CAST "dev_desc", BAD_CAST entdesc->dev_desc);
  if (entdesc->cnt_name) xmlNewChild (ent_node, NULL, BAD_CAST "cnt_name", BAD_CAST entdesc->cnt_name);
  if (entdesc->cnt_desc) xmlNewChild (ent_node, NULL, BAD_CAST "cnt_desc", BAD_CAST entdesc->cnt_desc);
  if (entdesc->obj_name) xmlNewChild (ent_node, NULL, BAD_CAST "obj_name", BAD_CAST entdesc->obj_name);
  if (entdesc->obj_desc) xmlNewChild (ent_node, NULL, BAD_CAST "obj_desc", BAD_CAST entdesc->obj_desc);
  if (entdesc->met_name) xmlNewChild (ent_node, NULL, BAD_CAST "met_name", BAD_CAST entdesc->met_name);
  if (entdesc->met_desc) xmlNewChild (ent_node, NULL, BAD_CAST "met_desc", BAD_CAST entdesc->met_desc);
  if (entdesc->trg_name) xmlNewChild (ent_node, NULL, BAD_CAST "trg_name", BAD_CAST entdesc->trg_name);
  if (entdesc->trg_desc) xmlNewChild (ent_node, NULL, BAD_CAST "trg_desc", BAD_CAST entdesc->trg_desc);
  
  asprintf (&str, "%i", entdesc->cust_adminstate);
  xmlNewChild (ent_node, NULL, BAD_CAST "cust_adminstate_num", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", entdesc->site_adminstate);
  xmlNewChild (ent_node, NULL, BAD_CAST "site_adminstate_num", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", entdesc->dev_adminstate);
  xmlNewChild (ent_node, NULL, BAD_CAST "dev_adminstate_num", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", entdesc->cnt_adminstate);
  xmlNewChild (ent_node, NULL, BAD_CAST "cnt_adminstate_num", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", entdesc->obj_adminstate);
  xmlNewChild (ent_node, NULL, BAD_CAST "obj_adminstate_num", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", entdesc->met_adminstate);
  xmlNewChild (ent_node, NULL, BAD_CAST "met_adminstate_num", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", entdesc->trg_adminstate);
  xmlNewChild (ent_node, NULL, BAD_CAST "trg_adminstate_num", BAD_CAST str);
  free (str);

  asprintf (&str, "%i", entdesc->cust_opstate);
  xmlNewChild (ent_node, NULL, BAD_CAST "cust_opstate_num", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", entdesc->site_opstate);
  xmlNewChild (ent_node, NULL, BAD_CAST "site_opstate_num", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", entdesc->dev_opstate);
  xmlNewChild (ent_node, NULL, BAD_CAST "dev_opstate_num", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", entdesc->cnt_opstate);
  xmlNewChild (ent_node, NULL, BAD_CAST "cnt_opstate_num", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", entdesc->obj_opstate);
  xmlNewChild (ent_node, NULL, BAD_CAST "obj_opstate_num", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", entdesc->met_opstate);
  xmlNewChild (ent_node, NULL, BAD_CAST "met_opstate_num", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", entdesc->trg_opstate);
  xmlNewChild (ent_node, NULL, BAD_CAST "trg_opstate_num", BAD_CAST str);
  free (str);

  if (entdesc->cust_resaddr) 
  {
    char *resaddrstr = i_resource_address_struct_to_string (entdesc->cust_resaddr);
    if (resaddrstr)
    { xmlNewChild (ent_node, NULL, BAD_CAST "cust_resaddr", BAD_CAST resaddrstr); free (resaddrstr); }
  }
  if (entdesc->dev_resaddr) 
  {
    char *resaddrstr = i_resource_address_struct_to_string (entdesc->dev_resaddr);
    if (resaddrstr)
    { xmlNewChild (ent_node, NULL, BAD_CAST "dev_resaddr", BAD_CAST resaddrstr); free (resaddrstr); }
  }
  
  return ent_node;
}

i_entity_descriptor* i_entity_descriptor_fromxml (i_xml *xml, xmlNodePtr ent_node)
{
  xmlNodePtr node;
  i_entity_descriptor *ent;

  /* Create entity descriptor */
  ent = i_entity_descriptor_create ();

  /* Parse tree */ 
  for (node = ent_node->children; node; node = node->next)
  {
    char *str;
    str = (char *) xmlNodeListGetString (xml->doc, node->xmlChildrenNode, 1);
    if (!str) continue;

    if (!strcmp((char *)node->name, "type_num") && str) ent->type = atoi (str);
    else if (!strcmp((char *)node->name, "name") && str) ent->name_str = strdup (str);
    else if (!strcmp((char *)node->name, "desc") && str) ent->desc_str = strdup (str);
    else if (!strcmp((char *)node->name, "adminstate_num") && str) ent->adminstate = atoi (str);
    else if (!strcmp((char *)node->name, "opstate_num") && str) ent->opstate = atoi (str);
    else if (!strcmp((char *)node->name, "resaddr") && str) ent->resaddr = i_resource_address_string_to_struct (str);

    else if (!strcmp((char *)node->name, "cust_name") && str) ent->cust_name = strdup (str);
    else if (!strcmp((char *)node->name, "cust_desc") && str) ent->cust_desc = strdup (str);
    else if (!strcmp((char *)node->name, "cust_adminstate_num") && str) ent->cust_adminstate = atoi (str);
    else if (!strcmp((char *)node->name, "cust_opstate_num") && str) ent->cust_opstate = atoi (str);
    else if (!strcmp((char *)node->name, "cust_resaddr") && str) ent->cust_resaddr = i_resource_address_string_to_struct (str);

    else if (!strcmp((char *)node->name, "site_name") && str) ent->site_name = strdup (str);
    else if (!strcmp((char *)node->name, "site_desc") && str) ent->site_desc = strdup (str);
    else if (!strcmp((char *)node->name, "site_adminstate_num") && str) ent->site_adminstate = atoi (str);
    else if (!strcmp((char *)node->name, "site_opstate_num") && str) ent->site_opstate = atoi (str);
    else if (!strcmp((char *)node->name, "site_suburb") && str) ent->site_suburb = strdup (str);

    else if (!strcmp((char *)node->name, "dev_name") && str) ent->dev_name = strdup (str);
    else if (!strcmp((char *)node->name, "dev_desc") && str) ent->dev_desc = strdup (str);
    else if (!strcmp((char *)node->name, "dev_adminstate_num") && str) ent->dev_adminstate = atoi (str);
    else if (!strcmp((char *)node->name, "dev_opstate_num") && str) ent->dev_opstate = atoi (str);
    else if (!strcmp((char *)node->name, "dev_resaddr") && str) ent->dev_resaddr = i_resource_address_string_to_struct (str);

    else if (!strcmp((char *)node->name, "cnt_name") && str) ent->cnt_name = strdup (str);
    else if (!strcmp((char *)node->name, "cnt_desc") && str) ent->cnt_desc = strdup (str);
    else if (!strcmp((char *)node->name, "cnt_adminstate_num") && str) ent->cnt_adminstate = atoi (str);
    else if (!strcmp((char *)node->name, "cnt_opstate_num") && str) ent->cnt_opstate = atoi (str);

    else if (!strcmp((char *)node->name, "obj_name") && str) ent->obj_name = strdup (str);
    else if (!strcmp((char *)node->name, "obj_desc") && str) ent->obj_desc = strdup (str);
    else if (!strcmp((char *)node->name, "obj_adminstate_num") && str) ent->obj_adminstate = atoi (str);
    else if (!strcmp((char *)node->name, "obj_opstate_num") && str) ent->obj_opstate = atoi (str);

    else if (!strcmp((char *)node->name, "met_name") && str) ent->met_name = strdup (str);
    else if (!strcmp((char *)node->name, "met_desc") && str) ent->met_desc = strdup (str);
    else if (!strcmp((char *)node->name, "met_adminstate_num") && str) ent->met_adminstate = atoi (str);
    else if (!strcmp((char *)node->name, "met_opstate_num") && str) ent->met_opstate = atoi (str);

    else if (!strcmp((char *)node->name, "trg_name") && str) ent->trg_name = strdup (str);
    else if (!strcmp((char *)node->name, "trg_desc") && str) ent->trg_desc = strdup (str);
    else if (!strcmp((char *)node->name, "trg_adminstate_num") && str) ent->trg_adminstate = atoi (str);
    else if (!strcmp((char *)node->name, "trg_opstate_num") && str) ent->trg_opstate = atoi (str);

    xmlFree (str);
  }

  return ent;
}

/* xml_ wrapper funcs */

int i_xml_entity_handler (i_resource *self, i_xml_request *req, int flags)
{
  i_xml *xml;
  i_entity *ent;
  xmlNodePtr node;
  xmlNodePtr root_node;

  /* Find entity */
  ent = i_entity_local_get (self, req->entaddr);
  if (!ent) return -1;

  /* Get sync timstamp from xml */
  time_t sync_version = 0;
  if (req->xml_in) 
  {
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;
    root_node = xmlDocGetRootElement (req->xml_in->doc);
    if (!root_node)
    { i_printf (1, "i_xml_entity_handler invalid XML received, no root node found"); return -1; }
    for (node = root_node->children; node; node = node->next)
    {
      char *str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);

      if (strcmp((char *)node->name, "sync_version") == 0 && str)
      { sync_version = atol (str); }

      xmlFree (str);
    }
  }
  if (sync_version > 0) flags = flags | ENTXML_SYNC;

  /* Create/setup doc */
  xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "entity_xml_data");
  xmlDocSetRootElement(xml->doc, root_node);

  /* Get entity data */
  node = i_entity_xml (ent, flags, sync_version);
  if (node)
  {
    /* Add special top-level entity properties */
    char *str;

    /* Auth Level */
    asprintf (&str, "%i", req->auth->level);
    xmlNewChild (node, NULL, BAD_CAST "auth_level", BAD_CAST str);
    free (str);

    /* Absolutely latest version */
    asprintf (&str, "%li", time(NULL));
    xmlNewChild (node, NULL, BAD_CAST "sync_version", BAD_CAST str);    /* Absolute highest version */
    free (str); 

    /* Add node */
    xmlAddChild (root_node, node); 
  }

  /* Finished */
  req->xml_out = xml;

  return 1;
} 

int xml_entity (i_resource *self, i_xml_request *req)
{
  return i_xml_entity_handler (self, req, 0);
}

int xml_entity_tree_authorative (i_resource *self, i_xml_request *req)
{
  return i_xml_entity_handler (self, req, ENTXML_TREE_AUTHORATIVE);
}

int xml_entity_tree (i_resource *self, i_xml_request *req)
{
  if (self->type == RES_CUSTOMER)
  { 
    /* Force use of authorative for customer resource */
    return xml_entity_tree_authorative (self, req); 
  }

  return i_xml_entity_handler (self, req, ENTXML_TREE);
} 

int xml_entity_tree_one_level (i_resource *self, i_xml_request *req)
{
  return i_xml_entity_handler (self, req, ENTXML_TREE_ONE_LEVEL);
}

int xml_entity_tree_summary (i_resource *self, i_xml_request *req)
{
  return i_xml_entity_handler (self, req, ENTXML_TREE_SUMMARY);
}

/* @} */
