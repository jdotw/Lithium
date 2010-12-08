#include <libpq-fe.h>

/* postgresql.c */

PGconn* i_pg_connect (i_resource *self, char *dbname);
PGconn* i_pg_connect_customer (i_resource *self);
int i_pg_close (PGconn *conn);

int i_pg_begin (PGconn *conn);
int i_pg_end (PGconn *conn);
int i_pg_commit (PGconn *conn);
void i_postgres_pqclear_wrapper (void *resptr);
char* i_postgres_escape (char *in_str);

/* postgresql_create.c */

int i_pg_create_db (i_resource *self, char *dbname);

/* postgresql_check.c */

int i_pg_check_db (i_resource *self, char *dbname);
int i_pg_checkcreate_db (i_resource *self, char *dbname);

/* postgres_drop.c */

int i_pg_drop_db (i_resource *self, char *dbname);
typedef struct i_pg_async_conn_s
{
  PGconn *pgconn;
  struct i_postgres_async_query_s *query;     /* The current request/query */

  struct i_list_s *query_q;                            /* Query queue */
  
  int usage;                                  /* Number of instances where this conn is in use */
} i_pg_async_conn;

/* postgresql_async.c */

typedef struct i_postgres_async_query_s
{
  int current_op;
  struct i_list_s *operations;
  
  int (*callback_func) (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);
  void *passdata;

  struct timeval start;
  struct timeval last_result;
} i_pg_async_query;

typedef struct
{
  int code;
  char *command;
} i_pg_async_query_op;

#define PG_FLAG_BEGIN 1
#define PG_FLAG_QUERY 2 
#define PG_FLAG_COMMIT 4
#define PG_FLAG_END 8

int i_pg_async_enable (i_resource *self);
int i_pg_async_disable (i_resource *self);

/* postgresql_async_conn.c */
i_pg_async_conn* i_pg_async_conn_persistent ();
int i_pg_async_conn_enable (i_resource *self);
int i_pg_async_conn_disable (i_resource *self);
i_pg_async_conn* i_pg_async_conn_create ();
void i_pg_async_conn_free (void *connptr);
struct i_list_s* i_pg_async_conn_list ();
i_pg_async_conn* i_pg_async_conn_open (i_resource *self, char *dbname);
void i_pg_async_conn_close (void *connptr);
i_pg_async_conn* i_pg_async_conn_open_customer (i_resource *self);

/* postgresql_async_query.c */

#define PGASYNC_QUERY_MAXQ 5000

i_pg_async_query* i_pg_async_query_create ();
void i_pg_async_query_free (void *queryptr);
i_pg_async_query_op* i_pg_async_query_op_create ();
void i_pg_async_query_op_free (void *opptr);
int i_pg_async_query_exec (i_resource *self, i_pg_async_conn *conn, char *query, int flags, int (*callback_func) (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata), void *passdata);

/* postgres_async_result.c */

int i_pg_async_result_process (i_resource *self, i_pg_async_conn *conn, PGresult *result);

/* postgresql_async_fdset.c */

int i_pg_async_fdset_preprocessor (i_resource *self, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data);
int i_pg_async_fdset_postprocessor (i_resource *self, int select_num, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data);

/* postgresql_async_resptime.c */

int i_pg_async_resptime_record (i_resource *self, i_pg_async_conn *conn);
  
