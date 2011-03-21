#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/callback.h>
#include <induction/list.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/metric.h>
#include <induction/metric_xml.h>
#include <induction/hierarchy.h>
#include <induction/xml.h>
#include <induction/entity_xml.h>
#include <induction/auth.h>
#include <induction/postgresql.h>

/*
 * Server-side search using the searchcache table
 */

#define SEARCH_ANY 0    /* Match any of the keywords -- OR */
#define SEARCH_ALL 2    /* Match all of the keywords -- AND */
#define SEARCH_REGEX 4  /* Use REGEX */  

int l_searchcache_xml_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);

int xml_search (i_resource *self, i_xml_request *req)
{
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_RO)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  int max_type = 6;
  int flags = 0;
  i_list *keyword_list = i_list_create ();
  i_list_set_destructor (keyword_list, free);
  if (req->xml_in)
  {
    /* Set variables  */
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;
    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;

      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
 
      if (!strcmp((char *)node->name, "keyword") && str) i_list_enqueue (keyword_list, strdup(str));
      else if (!strcmp((char *)node->name, "operator") && str)
      {
        if (strcmp(str, "OR")) flags = flags & SEARCH_ANY;
        else if (strcmp(str, "AND")) flags = flags & SEARCH_ALL;
      }
      else if (!strcmp((char *)node->name, "regex") && str) 
      {
        if (atoi(str) == 1) flags = flags & SEARCH_REGEX;
      }
      else if (!strcmp((char *)node->name, "max_type") && str) max_type = atoi (str);

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_search failed, no xml data received");
    return -1; 
  }
  if (keyword_list->size < 1)
  {
    i_printf (1, "xml_search no keywords found!");
  }

  /* Determine logical operator */
  char *logical_op;
  if (flags & SEARCH_ALL) logical_op = "AND";
  else logical_op = "OR";

  /* Create SQL Query */
  char *query_str = NULL;
  char *keyword;
  for (i_list_move_head(keyword_list); (keyword=i_list_restore(keyword_list))!=NULL; i_list_move_next(keyword_list))
  {
    if (query_str)
    {
      /* Add keyword to query */
      char *str;
      asprintf (&str, "%s %s (descr ILIKE '%%%s%%')",
        query_str, logical_op, keyword);
      free (query_str);
      query_str = str;
    }
    else
    {
      asprintf (&query_str, "SELECT ent_type, descr, ent_address, res_address, cust_desc, site_desc, dev_desc, cnt_desc, obj_desc FROM searchcache WHERE ((descr ILIKE '%%%s%%')", keyword);
    }
  }
  char *str;
  asprintf (&str, "%s)", query_str);
  free (query_str);
  query_str = str;
  str = NULL;
  i_list_free (keyword_list);

  /* Open conn */
  i_pg_async_conn *conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "xml_search failed to open SQL database connection"); return -1; }

  /* Execute query */
  int num = i_pg_async_query_exec (self, conn, query_str, 0, l_searchcache_xml_sqlcb, req);
  free (query_str);
  if (num != 0)
  { i_printf (1, "xml_search failed to execute initial SELECT query"); return -1; }

  return 0; // Request in progress
}

int l_searchcache_xml_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  i_xml_request *req = (i_xml_request *)passdata;

  /* Create XML */
  i_xml *xml = i_xml_create ();
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode (NULL, BAD_CAST "results");
  xmlDocSetRootElement (xml->doc, root_node);
  req->xml_out = xml;

  /* Check results */
  if (res && (PQresultStatus(res) == PGRES_COMMAND_OK || PQresultStatus(res) == PGRES_TUPLES_OK))
  {
    int row_count =  PQntuples (res);
    int row;
    for (row=0; row < row_count; row++)
    {
      /* Create result node */
      xmlNodePtr result_node = xmlNewNode (NULL, BAD_CAST "result");

      /* Add Result fields */
      xmlNewChild (result_node, NULL, BAD_CAST "ent_type", BAD_CAST PQgetvalue(res, row, 0));
      xmlNewChild (result_node, NULL, BAD_CAST "desc", BAD_CAST PQgetvalue(res, row, 1));
      xmlNewChild (result_node, NULL, BAD_CAST "ent_addr", BAD_CAST PQgetvalue(res, row, 2));
      xmlNewChild (result_node, NULL, BAD_CAST "res_addr", BAD_CAST PQgetvalue(res, row, 3));
      xmlNewChild (result_node, NULL, BAD_CAST "cust_desc", BAD_CAST PQgetvalue(res, row, 4));
      xmlNewChild (result_node, NULL, BAD_CAST "site_desc", BAD_CAST PQgetvalue(res, row, 5));
      xmlNewChild (result_node, NULL, BAD_CAST "dev_desc", BAD_CAST PQgetvalue(res, row, 6));
      xmlNewChild (result_node, NULL, BAD_CAST "cnt_desc", BAD_CAST PQgetvalue(res, row, 7));
      xmlNewChild (result_node, NULL, BAD_CAST "obj_desc", BAD_CAST PQgetvalue(res, row, 8));

      /* Add result to root node */
      xmlAddChild (root_node, result_node);
    }
  }

  /* Deliver */
  i_xml_deliver(self, req);

  return 0; // Free's the results list
}

