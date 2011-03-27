#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "userdb.h"
#include "user.h"
#include "bdb.h"
#include "list.h"
#include "files.h"
#include "auth.h"

static i_list *static_usercache_list;

/* Process profile SQL Operations */

int i_user_sql_init (i_resource *self)
{
  /* Initialize the SQL user table. If the table doesn't
   * exist, it should be created and the existing user.db
   * records imported into the new SQL table
   */

  /* Connect to SQL db */
  PGconn *pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "i_user_sql_init failed to connect to postgres database"); return -1; }

  /* Check sites SQL table */
  PGresult *result = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'users' AND relkind = 'r'");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* Clear first result */
    if (result) PQclear (result);

    /* users table not in database */
    result = PQexec (pgconn, "CREATE TABLE users (username varchar, fullname varchar, password varchar, access integer)");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "i_user_sql_init failed to create users table (%s)", PQresultErrorMessage (result)); }
    PQclear(result);

    /* Import old userdb records */
    i_list *userdb = i_userdb_get_all(self);
    i_user *user;
    for (i_list_move_head(userdb); (user=i_list_restore(userdb))!=NULL; i_list_move_next(userdb))
    {
      char *username_esc;
      if (user->auth->username_str) asprintf(&username_esc, "'%s'", user->auth->username_str);
      else username_esc = strdup("NULL");
      char *fullname_esc;
      if (user->fullname_str) asprintf(&fullname_esc, "'%s'", user->fullname_str);
      else fullname_esc = strdup("NULL");
      char *password_esc;
      if (user->auth->password_str) asprintf(&password_esc, "'%s'", user->auth->password_str);
      else password_esc = strdup("NULL");

      char *query;
      asprintf(&query, "INSERT INTO users (username, fullame, password, access) VALUES (%s, %s, %s, %i)", 
        username_esc, fullname_esc, password_esc, user->auth->access);
      free(username_esc);
      free(fullname_esc);
      free(password_esc);
      
      result = PQexec(pgconn, query);
      free(query);
      if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
      { i_printf (1, "i_user_sql_init failed to import user.db record for %s (%s)", user->auth->username_str, PQresultErrorMessage (result)); }
      PQclear(result);
    }
    if (userdb) i_list_free(userdb);

    /* Import master admin user record */
    char *master_user = i_configfile_get (self, NODECONF_FILE, "master_user", "username", 0);
    if (master_user && strlen(master_user) > 0)
    {
      char *master_pass = i_configfile_get (self, NODECONF_FILE, "master_user", "password", 0);

      char *query;
      asprintf(&query, "INSERT INTO users (username, fullname, password, access) VALUES ('%s', '%s', '%s', %i)",
        master_user, "Global Admin", master_pass, AUTH_LEVEL_MASTER);

      result = PQexec(pgconn, query);
      free(query);
      if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
      { i_printf (1, "i_user_sql_init failed to import global admin user (%s)", PQresultErrorMessage (result)); }
      PQclear(result);
      
      if (master_pass) free(master_pass);
      free(master_user);
    }
    
  }
  else if (result)
  { PQclear (result); }

  /* Close DB */
  i_pg_close (pgconn);

  return 0;
}

/* Insert/Update/Delete */

i_user* i_user_sql_get (i_resource *self, char *username_str)
{
  /* First, check the cache */
  if (!static_usercache_list)
  { i_user_sql_init_cache(self); }

  /* Loop through records */
  i_user *user;
  for(i_list_move_head(static_usercache_list); (user=i_list_restore(static_usercache_list))!=NULL; i_list_move_next(static_usercache_list))
  {
    if (strcmp(user->auth->username, username_str)==0) return user;
  }
  return NULL;
}

i_list* i_user_sql_list (i_resource *self)
{
  if (!static_usercache_list) i_user_sql_init_cache(self); 

  return static_usercache_list;
}

int i_user_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_procpro_sql_cb failed to execute query (%s)", PQresultErrorMessage (result)); return -1; }

  /* Close conn */
  i_pg_async_conn_close (conn);

  /* Invalidate the cache */
  i_user_sql_invalidate_cache();

  return 0;
}

int i_user_sql_insert (i_resource *self, i_user *user)
{
  char *query;

  /* Connect to SQL */
  /* Open conn */
  i_pg_async_conn *conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "i_user_sql_insert failed to open SQL db connection"); return -1; }

  /* Create query */
  char *fullname_esc = i_postgres_escape (user->fullname_str);
  char *username_esc = i_postgres_escape (user->auth->username);
  char *password_esc = i_postgres_escape (user->auth->password);
  asprintf (&query, "INSERT INTO users (fullname, username, password, access) VALUES ('%s', '%s', '%s', '%i');",
    fullname_esc, username_esc, password_esc, user->auth->level);
  free (fullname_esc);
  free (username_esc);
  free (password_esc);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, i_user_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "i_user_sql_insert failed to insert user record for %s (%s)", user->auth->username_str, user->fullname_str); return -1; }
  PQclear (pgres);

  /* Cache is invalidated on callback */

  return 0;
}

int i_user_sql_update (i_resource *self, l_procpro *procpro)
{
  int num;

  /* Open conn */
  i_pg_async_conn *conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "i_user_sql_update failed to open SQL db connection"); return -1; }

  /* Create query */
  char *fullname_esc = i_postgres_escape (user->fullname_str);
  char *username_esc = i_postgres_escape (user->auth->username);
  char *password_esc = i_postgres_escape (user->auth->password);
  char *query;
  asprintf (&query, "UPDATE users SET fullname='%s', password='%s', access=%i WHERE username='%s'",
    fullname_esc, password_esc, user->auth->level, username_esc);
  free (fullname_esc);
  free (username_esc);
  free (password_esc);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, i_user_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "i_user_sql_update failed to execute UPDATE for procpro %li", procpro->id); return -1; }
  
  /* Cache is invalidated on callback */

  return 0;
}

int i_user_sql_delete (i_resource *self, char *username_str)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "i_user_sql_delete failed to open SQL db connection"); return -1; }

  /* Create query */
  char *username_esc = i_postgres_escape (username_str);
  asprintf (&query, "DELETE FROM users WHERE username='%s'", username_esc);
  free(userame_esc);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, i_user_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "i_user_sql_delete failed to execute DELETE for username %s", username_str); return -1; }
  
  /* Cache is invalidated on callback */

  return 0;
}

/*
 * List Loading and Caching
 */

int i_user_sql_invalidate_cache()
{
  /* Flush the cache */
  if (static_usercache_list) 
  { 
    i_list_free(static_usercache_list);
    static_usercache_list = NULL;
  }
}

int i_user_sql_init_cache()
{
  /* Synchronously load the list of users (and cache it) */
  
  /* Connect */
  PGconn *conn = i_pg_connect_customer(self);
  if (!conn) { i_printf(1, "i_user_sql_init_cache failed to connect to SQL database"); return -1; }

  /* Create list */
  if (static_usercache_list) i_list_free(static_usercache_list);
  static_usercache_list = i_list_create();
  i_list_set_destructor(static_usercache_list, i_user_free);

  /* Query */
  char *query = "SELECT username, fullname, password, access FROM users";
  PGresult *result = PGexec(conn, query);
  if (result && PQresultStatus(result)==PGRES_TUPLES_OK)
  {
    int row_count = PQntuples(result);
    int y;
    for (y=0; y < row_count; y++)
    {
      char *username_str = PQgetvalue(result, y, 0);
      char *fullname_str = PQgetvalue(result, y, 1);
      char *password_str = PQgetvalue(result, y, 2);
      char *access_str = PQgetvalue(result, y, 3);

      i_user *user = i_user_create();
      if (username_str && strlen(username_str) > 0) user->auth->username_str = strdup(username_str);
      if (password_str && strlen(password_str) > 0) user->auth->password_str = strdup(password_str);
      if (fullname_str && strlen(fullname_str) > 0) user->fullname_str = strdup(fullname_str);
      if (access_str && strlen(access_str) > 0) user->access = atoi(access_str);
      i_list_enqueue(static_usercache_list, user);
    }
  }
  if (result) PQclear(result);
  i_pg_close(conn);

  return 0;
}

