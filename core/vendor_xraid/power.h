typedef struct v_power_item_s
{
  struct i_object_s *obj;

  /* Power */
  struct i_metric_s *status;

} v_power_item;

/* power.c */
struct i_container_s* v_power_cnt ();
v_power_item* v_power_static_left_item ();
v_power_item* v_power_static_right_item ();
int v_power_enable (i_resource *self);
int v_power_disable (i_resource *self);

/* power_item.c */
v_power_item* v_power_item_create ();
void v_power_item_free (void *itemptr);

/* power_cntform.c */
int v_power_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* power_objform.c */
int v_power_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

