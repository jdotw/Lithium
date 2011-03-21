typedef struct v_fan_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  
  struct i_metric_s *ent_type;
  struct i_metric_s *hw_ver;
  struct i_metric_s *sw_ver;
  struct i_metric_s *state;
  struct i_metric_s *fault;

} v_fan_item;

/* fan.c */
struct i_container_s* v_fan_cnt ();
int v_fan_enable (i_resource *self);
int v_fan_disable (i_resource *self);

/* fan_item.c */
v_fan_item* v_fan_item_create ();
void v_fan_item_free (void *itemptr);

/* fan_objfact.c */
int v_fan_objfact_fab ();
int v_fan_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_fan_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* fan_cntform.c */
int v_fan_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* fan_objform.c */
int v_fan_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_fan_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

