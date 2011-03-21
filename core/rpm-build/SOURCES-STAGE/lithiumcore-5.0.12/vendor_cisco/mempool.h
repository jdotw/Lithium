typedef struct v_mempool_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *used;
  struct i_metric_s *free;
  struct i_metric_s *largest_free;
  struct i_metric_s *size;
  struct i_metric_s *used_pc;
  struct i_metric_s *contig_free_pc;

} v_mempool_item;

/* mempool.c */
struct i_container_s* v_mempool_cnt ();
int v_mempool_enable (i_resource *self);
int v_mempool_disable (i_resource *self);

/* mempool_item.c */
v_mempool_item* v_mempool_item_create ();
void v_mempool_item_free (void *itemptr);

/* mempool_objfact.c */
int v_mempool_objfact_fab ();
int v_mempool_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_mempool_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* mempool_cntform.c */
int v_mempool_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* mempool_objform.c */
int v_mempool_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_mempool_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

