/*
 * Actions - User-defined scripts to be run in the event
 *           of an incident occuring.
 */

typedef struct l_procpro_s
{
  /* Service Info */
  unsigned long id;           /* Auto-assigned by SQL */
  char *desc_str;             /* User-defined */

  /* Behaviour */
  char *match_str;            /* Process match string */
  char *argmatch_str;         /* Process arguments match string */

  /* Object */
  struct i_object_s *obj;

  /* Default metrics */
  struct i_metric_s *status_met;
  struct i_metric_s *count_met;
  struct i_metric_s *highest_count_met;
  struct i_metric_s *mem_total_met;
  struct i_metric_s *cpu_total_met;
  struct i_metric_s *mem_maxsingle_met;
  struct i_metric_s *cpu_maxsingle_met;

} l_procpro;

/* procpro.c */
struct i_container_s* l_procpro_cnt ();
i_list* l_procpro_list ();
int l_procpro_enable (i_resource *self);
int l_procpro_enable_loadcb (i_resource *self, struct i_list_s *list, void *passdata);
l_procpro* l_procpro_create ();
void l_procpro_free (void *procproptr);

/* procpro_sql.c */
int l_procpro_sql_insert (i_resource *self, l_procpro *procpro);
int l_procpro_sql_update (i_resource *self, l_procpro *procpro);
int l_procpro_sql_delete (i_resource *self, int procpro_id);
struct i_callback_s* l_procpro_sql_load_list (i_resource *self, char *id_str, char *site_name, char *dev_name, int (*cbfunc) (), void *passdata);
int l_procpro_sql_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_procpro_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);

/* procpro_object.c */
int l_procpro_object_add (i_resource *self, l_procpro *procpro);
int l_procpro_object_update (i_resource *self, l_procpro *procpro);
int l_procpro_object_remove (i_resource *self, unsigned long id);

/* procpro_refresh.c */
int l_procpro_refresh (i_resource *self, struct i_object_s *obj, int opcode);
int l_procpro_refresh_procpro (i_resource *self, l_procpro *procpro);


