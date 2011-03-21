typedef struct v_blower_item_s
{
  struct i_object_s *obj;

  /* Power */
  struct i_metric_s *present;
  struct i_metric_s *rpm;

} v_blower_item;

/* blower.c */
struct i_container_s* v_blower_cnt ();
v_blower_item* v_blower_static_top_item ();
v_blower_item* v_blower_static_bottom_item ();
int v_blower_enable (i_resource *self);
int v_blower_disable (i_resource *self);

/* blower_item.c */
v_blower_item* v_blower_item_create ();
void v_blower_item_free (void *itemptr);

/* blower_cntform.c */
int v_blower_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* blower_objform.c */
int v_blower_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_blower_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

