#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/parser.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/socket.h"
#include "induction/entity.h"
#include "induction/navtree.h"
#include "induction/navform.h"
#include "induction/hierarchy.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "device/snmp.h"

#include "plist.h"

/*
 * Xraid plist info 
 */

char* v_plist_data_from_dict (xmlDocPtr plist, xmlNodePtr dictNode, char *key)
{
  xmlNodePtr node;
  char *data = NULL;
  char *curkey = NULL;
  
  for (node = dictNode->children; node; node = node->next)
  {
    if (strcmp((char *) node->name, "key") == 0)
    {
      char *str = (char *) xmlNodeListGetString (plist, node->xmlChildrenNode, 1);
      if (curkey) free (curkey);
      if (str) 
      { 
        curkey = strdup (str);
        xmlFree(str);
      }
      else
      { curkey = NULL; }
    }

    if (strcmp((char *) node->name, "string") == 0 || strcmp((char *) node->name, "integer") == 0) 
    {
      if (!key || (curkey && strcmp(key, curkey) == 0))
      {
        char *str = (char *) xmlNodeListGetString (plist, node->xmlChildrenNode, 1);
        if (str)
        {
          data = strdup (str);
          xmlFree (str);
        }
      }
    }
  }

  if (curkey) free (curkey);
  
  return data;
}

xmlNodePtr v_plist_node_from_dict (xmlDocPtr plist, xmlNodePtr dictNode, char *key)
{
  xmlNodePtr node;
  
  for (node = dictNode->children; node; node = node->next)
  {
    if (strcmp((char *)node->name, "key") == 0)
    {
      char *str = (char *) xmlNodeListGetString (plist, node->xmlChildrenNode, 1);
      if (str)
      {
        if (strcmp(str, key) == 0)
        {
          xmlFree(str);
          return node->next->next;
        }
        xmlFree(str);
      }
    }
  }

  return NULL;
}

int v_plist_bool_from_dict (xmlDocPtr plist, xmlNodePtr dictNode, char *key)
{
  int retval = -1;
  xmlNodePtr node;
  char *curkey = NULL;

  for (node = dictNode->children; node; node = node->next)
  {
    if (strcmp((char *)node->name, "key") == 0)
    {
      char *str = (char *) xmlNodeListGetString (plist, node->xmlChildrenNode, 1);
      if (curkey) free (curkey);
      if (str)
      {
        curkey = strdup (str);
        xmlFree(str);
      }
      else
      { curkey = NULL; }
    }

    if (strcmp((char *)node->name, "true") == 0 || strcmp((char *)node->name, "false") == 0)
    {
      if (!key || (curkey && strcmp(key, curkey) == 0))
      {
        if (strcmp((char *)node->name, "true") == 0)
        { retval = 1; }
        else
        { retval = 0; }
      }
    }
  }

  if (curkey) free (curkey);

  return retval;
}

int v_plist_int_from_dict (xmlDocPtr plist, xmlNodePtr dictNode, char *key)
{
  xmlNodePtr node;
  int retval = 0;
  char *curkey = NULL;
  
  for (node = dictNode->children; node; node = node->next)
  {
    if (strcmp((char *)node->name, "key") == 0)
    {
      char *str = (char *) xmlNodeListGetString (plist, node->xmlChildrenNode, 1);
      if (curkey) free (curkey);
      if (str) 
      {
        curkey = strdup (str);
        xmlFree(str);
      }
      else curkey = NULL;
    } 
    
    if (strcmp((char *)node->name, "integer") == 0)
    {
      if (!key || (curkey && strcmp(key, curkey) == 0))
      {
        char *str = (char *) xmlNodeListGetString (plist, node->xmlChildrenNode, 1);
        if (str) 
        { 
          retval = atoi (str);
          xmlFree (str);
        }
      } 
    } 
  } 
  
  if (curkey) free (curkey);
  
  return retval;
}

