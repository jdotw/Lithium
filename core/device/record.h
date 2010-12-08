/* Recording Rules */

typedef struct l_record_recrule_s
{
  long id;
  char *site_name;
  char *site_desc;
  char *dev_name;
  char *dev_desc;
  char *obj_name;
  char *obj_desc;
  char *met_name;
  char *met_desc;
  int recordflag;
} l_record_recrule;

/* record.c */
int l_record_enable (i_resource *self, struct i_metric_s *met);
int l_record_disable (i_resource *self, struct i_metric_s *met);

/* recoerd_eval.c */
int l_record_eval_recrules (i_resource *self, struct i_metric_s *met);
int l_record_eval_recrules_obj (i_resource *self, i_object *obj);

/* record_recrule.c */
l_record_recrule* l_record_recrule_create ();
void l_record_recrule_free (void *ruleptr);
int l_record_recrule_add (i_resource *self, struct i_metric_s *met, l_record_recrule *rule);
int l_record_recrule_update (i_resource *self, struct i_metric_s *met, l_record_recrule *rule);
int l_record_recrule_remove (i_resource *self, struct i_metric_s *met, long rule_id);

/* record_recrule_sql.c */
int l_record_recrule_sql_get (i_resource *self, long id, int (*cbfunc) (), void *passdata);
int l_record_recrule_sql_get_sqlcb ();
int l_record_recrule_sql_insert (i_resource *self, struct i_container_s *cnt, l_record_recrule *rule);
int l_record_recrule_sql_update (i_resource *self, l_record_recrule *rule);
int l_record_recrule_sql_delete (i_resource *self, long rule_id);
int l_record_recrule_sql_sqlcb ();
int l_record_recrule_sql_load (i_resource *self, struct i_metric_s *met, int (*cbfunc) (), void *passdata);
int l_record_recrule_sql_load_sqlcb ();
void l_record_recrule_sql_invalidate_cache ();
struct i_list_s* l_record_recrule_sql_load_sync (i_resource *self, struct i_metric_s *met);

/* record_form_recrule.c */
int form_record_recrule ();
int l_record_form_recrule_rulecb (i_resource *self, i_list *list, void *passdata);

/* record_form_recrule_edit.c */
int l_record_form_recrule_edit_rulecb (i_resource *self, l_record_recrule *rule, void *passdata);
