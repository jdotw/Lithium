typedef struct v_revision_item_s
{
  struct i_object_s *obj;
  
  struct i_metric_s *version;

} v_revision_item;

/* revision.c */
struct i_container_s* v_revision_cnt ();
int v_revision_enable (i_resource *self);
int v_revision_disable (i_resource *self);

/* revision_item.c */
v_revision_item* v_revision_item_create ();
void v_revision_item_free (void *itemptr);

/* revision_objfact.c */
int v_revision_objfact_fab ();
int v_revision_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_revision_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* revision_cntform.c */
int v_revision_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* revision_objform.c */
int v_revision_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_revision_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

