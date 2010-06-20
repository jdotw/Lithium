/* pgsql_maint.h */

int l_pgsql_maint_enable (i_resource *self);
int l_pgsql_maint_disable (i_resource *self);
int l_pgsql_maint_timercb (i_resource *self, i_timer *timer, void *passdata);
int l_pgsql_maint_vacuumcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);
int l_pgsql_maint_analyzecb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);

