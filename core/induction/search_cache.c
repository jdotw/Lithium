#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "induction.h"
#include "list.h"
#include "cement.h"
#include "entity.h"
#include "metric.h"

#include "search.h"

/* 
 * Search Cache Functions
 *
 * An sqlite layer stored in memory for server-side
 * search of entity descriptions and values
 */

/* Static Variables */

sqlite3 *static_cache = NULL;

/* Initialization */

int i_search_cache_init (i_resource *self)
{
  char *sqlerr = NULL;

#ifdef DEBUG
  char *str;
  asprintf (&str, "/var/tmp/ldebug-searchcache-%s-%s", i_resource_typestr(self->type), self->ident_str);
  int num = sqlite3_open (str, &static_cache);
  free (str);
#else
  int num = sqlite3_open (":memory:", &static_cache);
#endif
  if (num != SQLITE_OK)
  {
    i_printf (1, "i_search_cache_init failed to create in-memory sqlite table");
    return -1;
  }

  char *create = "CREATE TABLE search_cache (ent_type integer, address varchar, descr varchar, value varchar)";
  sqlite3_exec (static_cache, create, NULL, NULL, &sqlerr);

  return 0;
}

/* Cache Insert/Delete */

int i_search_cache_insert (i_resource *self, i_entity *entity)
{
  /* Check state */
  if (!static_cache) i_search_cache_init (self);

  /* Get current value */
  char *value_str;
  if (entity->ent_type == 6)
  {
    /* Metric */
    char *str = i_metric_valstr ((i_metric *)entity, NULL);
    asprintf (&value_str, "'%s'", str);
    free (str);
  }
  else
  { value_str = strdup ("NULL"); }

  /* Create query */
  char *entaddr_str = i_entity_address_string (entity, NULL);
  char *desc_str = entity->desc_str ? : entity->name_str;
  char *insert_str;
  asprintf (&insert_str, "INSERT INTO search_cache (ent_type, address, descr, value) VALUES (%i, '%s', '%s', %s)",
    entity->ent_type, entaddr_str, desc_str, value_str);
  free (entaddr_str);
  free (value_str);

  /* Execute */
  char *sqlerr = NULL;
  sqlite3_exec (static_cache, insert_str, NULL, NULL, &sqlerr);
  free (insert_str);

  return 0;
}

int i_search_cache_delete (i_resource *self, i_entity *entity)
{
  /* Check state */
  if (!static_cache) i_search_cache_init (self);

  /* Create query */
  char *entaddr_str = i_entity_address_string (entity, NULL);
  char *delete_str;
  asprintf (&delete_str, "DELETE FROM search_cache WHERE address='%s'", entaddr_str);
  free (entaddr_str);

  /* Execute */
  char *sqlerr = NULL;
  sqlite3_exec (static_cache, delete_str, NULL, NULL, &sqlerr);
  free (delete_str);

  return 0;
}

/* Query */

i_list* i_search_cache_query (i_resource *self, i_list *keywords, int max_type, int flags)
{
  /* Returns list of local entities using the
   * SQL-format query_str
   */

  /* Check state */
  if (!static_cache) i_search_cache_init (self);

  /* Determine logical operator */
  char *logical_op;
  if (flags & SEARCH_ALL) logical_op = "AND";
  else logical_op = "OR";

  /* Create SQL Query */  
  char *query_str = NULL;
  char *keyword;
  for (i_list_move_head(keywords); (keyword=i_list_restore(keywords))!=NULL; i_list_move_next(keywords))
  {
    if (query_str)
    {
      /* Add keyword to query */
      char *str;
      asprintf (&str, "%s %s (descr LIKE '%%%s%%' OR value LIKE '%%%s%%')", 
        query_str, logical_op, keyword, keyword);
      free (query_str);
      query_str = str;
    }
    else
    {
      asprintf (&query_str, "SELECT address FROM search_cache WHERE ent_type >= %i AND ((descr LIKE '%%%s%%' OR value LIKE '%%%s%%')",
        max_type, keyword, keyword);
    }
  }
  char *str;
  asprintf (&str, "%s)", query_str);
  free (query_str);
  query_str = str;
  str = NULL;

  /* Execute Query */
  i_list *results = i_list_create ();
  char **result = NULL;
  int rows = 0;
  int cols = 0;
  char *sqlerr = NULL;
  int num = sqlite3_get_table (static_cache, query_str, &result, &rows, &cols, &sqlerr);
  if (num == SQLITE_OK)
  {
    int y;
    for (y=1; y < (rows+1); y++)
    {
      /* Parse results */
      char *address_str = result[0 + (y * cols)];
      if (!address_str)
      { 
        i_printf (1, "i_search_cache_query warning, blank entity address found in search results for query '%s'", query_str);
        continue;
      }

      /* Get local entity */
      i_entity_address *entaddr = i_entity_address_struct (address_str);
      i_entity *entity = i_entity_local_get (self, entaddr);
      i_entity_address_free (entaddr);

      /* Enqueue */
      i_list_enqueue (results, entity);
    }
  }
  else
  {
    i_printf (1, "i_search_cache_query failed to execute query '%s'", query_str);
    i_list_free (results);
    results = NULL;
  }

  if (result) sqlite3_free_table (result);
  if (sqlerr) sqlite3_free (sqlerr);
  free (query_str);

  return results;
}

