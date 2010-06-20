typedef struct v_sensor_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  
  struct i_metric_s *ent_type;
  struct i_metric_s *hw_ver;
  struct i_metric_s *sw_ver;
  struct i_metric_s *state;
  struct i_metric_s *fault;

} v_sensor_item;

/* sensor.c */
struct i_container_s* v_sensor_cnt ();
int v_sensor_enable (i_resource *self);
int v_sensor_disable (i_resource *self);

/* sensor_item.c */
v_sensor_item* v_sensor_item_create ();
void v_sensor_item_free (void *itemptr);

/* sensor_objfact.c */
int v_sensor_objfact_fab ();
int v_sensor_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_sensor_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* sensor_cntform.c */
int v_sensor_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* sensor_objform.c */
int v_sensor_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_sensor_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

