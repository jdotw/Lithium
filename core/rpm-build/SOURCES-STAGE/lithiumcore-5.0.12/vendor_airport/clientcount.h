typedef struct v_clientcount_item_s
{
  struct i_object_s *obj;

  /* Power */
  struct i_metric_s *count;

} v_clientcount_item;

/* clientcount.c */
struct i_container_s* v_clientcount_cnt ();
v_clientcount_item* v_clientcount_static_item ();
int v_clientcount_enable (i_resource *self);
int v_clientcount_disable (i_resource *self);

/* clientcount_item.c */
v_clientcount_item* v_clientcount_item_create ();
void v_clientcount_item_free (void *itemptr);

/* clientcount_cntform.c */
int v_clientcount_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* clientcount_objform.c */
int v_clientcount_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_clientcount_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* clientcount_refcb.c */
int v_clientcount_clientlist_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);


