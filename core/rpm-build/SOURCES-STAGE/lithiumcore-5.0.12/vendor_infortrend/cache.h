typedef struct v_cache_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  struct i_metric_s *blocksize_index;
  struct i_metric_s *blocksize;

  struct i_metric_s *total_blocks;
  struct i_metric_s *total;

  struct i_metric_s *dirty_blocks;
  struct i_metric_s *dirty;

  struct i_metric_s *dirty_pc;

} v_cache_item;

/* cache.c */
struct i_container_s* v_cache_cnt ();
int v_cache_enable (i_resource *self);
int v_cache_disable (i_resource *self);

/* cache_item.c */
v_cache_item* v_cache_item_create ();
void v_cache_item_free (void *itemptr);

/* cache_objfact.c */
int v_cache_objfact_fab ();
int v_cache_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_cache_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* cache_cntform.c */
int v_cache_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* cache_objform.c */
int v_cache_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_cache_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

