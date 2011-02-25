/* Cement - Trigger Sets */

typedef struct i_triggerset_s
{
  /* Triggerset Identifers */
  char *name_str;                     /* Unique name of the trigger set */
  char *desc_str;                     /* Description of the trigger set */
  struct i_container_s *cnt;          /* Parent container */

  /* Application Variables */
  char *metname_str;                  /* The name_str of metrics to which this triggerset is applicable */
  struct i_list_s *obj_list;          /* A list of objects to which this trigger set is applied */

  /* Triggerset config */
  int default_applyflag;              /* Default application flag */
  time_t default_duration;            /* Default duration */
  
  /* Trigger List */
  struct i_list_s *trg_list;          /* List of triggers */
  
} i_triggerset;

typedef struct i_triggerset_apprule_s
{
  long id;
  char *site_name;
  char *site_desc;
  char *dev_name;
  char *dev_desc;
  char *obj_name;
  char *obj_desc;
  int applyflag;
} i_triggerset_apprule;

typedef struct i_triggerset_valrule_s
{
  long id;
  char *site_name;
  char *site_desc;
  char *dev_name;
  char *dev_desc;
  char *obj_name;
  char *obj_desc;
  char *trg_name;
  char *trg_desc;
  char *xval_str;
  char *yval_str;
  time_t duration_sec;
  int trg_type;
  int adminstate;
} i_triggerset_valrule;

/* triggerset.c */

i_triggerset* i_triggerset_create (char *name_str, char *desc_str, char *metname_str);
void i_triggerset_free (void *tsetptr);
int i_triggerset_assign (i_resource *self, i_container *cnt, i_triggerset *tset);
int i_triggerset_assign_obj (i_resource *self, struct i_object_s *obj, i_triggerset *tset);
int i_triggerset_unassign (i_resource *self, i_container *cnt, i_triggerset *tset);

/* triggerset_apply.c */

int i_triggerset_apply (i_resource *self, struct i_object_s *obj, i_triggerset *tset);
int i_triggerset_apply_trg (i_resource *self, struct i_object_s *obj, struct i_metric_s *met, i_triggerset *tset, struct i_trigger_s *trg);
int i_triggerset_apply_allsets (i_resource *self, struct i_object_s *obj);
int i_triggerset_apply_allobjs (i_resource *self, struct i_container_s *cnt, i_triggerset *tset);
int i_triggerset_strip (i_resource *self, struct i_object_s *obj, i_triggerset *tset);
int i_triggerset_strip_trg (i_resource *self, struct i_object_s *obj, struct i_metric_s *met, i_triggerset *tset, struct i_trigger_s *trg);
int i_triggerset_strip_allsets (i_resource *self, struct i_object_s *obj);
int i_triggerset_strip_allobjs (i_resource *self, struct i_container_s *cnt, i_triggerset *tset);
int i_triggerset_evalapprules (i_resource *self, struct i_object_s *obj, i_triggerset *tset);
int i_triggerset_evalapprules_allsets (i_resource *self, struct i_object_s *obj);

/* triggerset_trg.c */

#define TSET_FLAG_VALAPPLY 1
#define TSET_FLAG_VALLOADAPPLY 2

int i_triggerset_addtrg (i_resource *self, i_triggerset *tset, char *name_str, char *desc_str, unsigned short val_type, unsigned short trg_type, double valflt, char *valstr, double yvalflt, char *yvalstr, time_t duration, unsigned short effect, unsigned short flags);
int i_triggerset_removetrg (i_resource *self, i_triggerset *tset, char *name_str);

/* triggerset_apprule.c */
i_triggerset_apprule* i_triggerset_apprule_create ();
void i_triggerset_apprule_free (void *ruleptr);
int i_triggerset_apprule_add (i_resource *self, struct i_object_s *obj, i_triggerset *tset, i_triggerset_apprule *rule);
int i_triggerset_apprule_add_exclusive (i_resource *self, struct i_object_s *obj, i_triggerset *tset, i_triggerset_apprule *rule);
int i_triggerset_apprule_update (i_resource *self, struct i_object_s *obj, i_triggerset *tset, i_triggerset_apprule *rule);
int i_triggerset_apprule_update_exclusive (i_resource *self, struct i_object_s *obj, i_triggerset *tset, i_triggerset_apprule *rule);
int i_triggerset_apprule_remove (i_resource *self, struct i_object_s *obj, i_triggerset *tset, long rule_id);
int i_triggerset_apprule_remove_exclusive (i_resource *self, struct i_entity_s *ent);

/* triggerset_apprule_sql.c */
int i_triggerset_apprule_sql_get (i_resource *self, long id, int (*cbfunc) (), void *passdata);
int i_triggerset_apprule_sql_get_sqlcb ();
int i_triggerset_apprule_sql_insert (i_resource *self, struct i_object_s *obj, i_triggerset *tset, i_triggerset_apprule *rule);
int i_triggerset_apprule_sql_update (i_resource *self, i_triggerset_apprule *rule, i_triggerset *tset);
int i_triggerset_apprule_sql_delete (i_resource *self, long rule_id, i_triggerset *tset);
int i_triggerset_apprule_sql_delete_specific (i_resource *self, struct i_object_s *obj, struct i_triggerset_s *tset);
int i_triggerset_apprule_sql_delete_exclusive (i_resource *self, struct i_object_s *obj, i_triggerset *tset, i_triggerset_apprule *rule);
int i_triggerset_apprule_sql_delete_wildcard (i_resource *self, struct i_entity_s *ent);
int i_triggerset_apprule_sql_sqlcb ();
int i_triggerset_apprule_sql_load (i_resource *self, i_triggerset *tset, struct i_object_s *obj, int (*cbfunc) (), void *passdata);
int i_triggerset_apprule_sql_load_sqlcb ();
void i_triggerset_apprule_sql_invalidate_cache ();
struct i_list_s* i_triggerset_apprule_sql_load_sync (i_resource *self, i_triggerset *tset, struct i_object_s *obj);

/* triggerset_valrule.c */
i_triggerset_valrule* i_triggerset_valrule_create ();
void i_triggerset_valrule_free (void *ruleptr);
int i_triggerset_valrule_add (i_resource *self, struct i_object_s *obj, i_triggerset *tset, i_triggerset_valrule *rule);
int i_triggerset_valrule_add_exclusive (i_resource *self, struct i_object_s *obj, i_triggerset *tset, i_triggerset_valrule *rule);
int i_triggerset_valrule_update (i_resource *self, struct i_object_s *obj, i_triggerset *tset, i_triggerset_valrule *rule);
int i_triggerset_valrule_update_exclusive (i_resource *self, struct i_object_s *obj, i_triggerset *tset, i_triggerset_valrule *rule);
int i_triggerset_valrule_remove (i_resource *self, struct i_object_s *obj, i_triggerset *tset, long rule_id);
int i_triggerset_valrule_remove_exclusive (i_resource *self, struct i_entity_s *ent);

/* triggset_valrule_sql.c */
int i_triggerset_valrule_sql_get (i_resource *self, long id, int (*cbfunc) (), void *passdata);
int i_triggerset_valrule_sql_get_sqlcb ();
int i_triggerset_valrule_sql_insert (i_resource *self, struct i_object_s *obj, i_triggerset *tset, i_triggerset_valrule *rule);
int i_triggerset_valrule_sql_update (i_resource *self, i_triggerset *tset, i_triggerset_valrule *rule);
int i_triggerset_valrule_sql_delete (i_resource *self, i_triggerset *tset, long rule_id);
int i_triggerset_valrule_sql_delete_specific (i_resource *self, struct i_object_s *obj, i_triggerset *tset, char *trg_name);
int i_triggerset_valrule_sql_delete_wildcard (i_resource *self, struct i_entity_s *ent);
int i_triggerset_valrule_sql_delete_exclusive (i_resource *self, struct i_object_s *obj, i_triggerset *tset, i_triggerset_valrule *rule);
int i_triggerset_valrule_sql_sqlcb ();
void i_triggerset_valrule_sql_invalidate_cache ();
i_list* i_triggerset_valrule_sql_load_sync (i_resource *self, i_triggerset *tset, struct i_object_s *obj, struct i_trigger_s *trg);

/* triggerset_valrule_eval.c */
int i_triggerset_evalvalrules (i_resource *self, struct i_object_s *obj, i_triggerset *tset);


