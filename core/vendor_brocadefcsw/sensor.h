typedef struct v_sensor_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  int disabled_metrics;

  /* Sensor */
  struct i_metric_s *type;
  struct i_metric_s *status;
  struct i_metric_s *value;
  struct i_metric_s *info;

} v_sensor_item;

/* sensor.c */
struct i_container_s* v_sensor_cnt ();
int v_sensor_enable (i_resource *self);

/* sensor_item.c */
v_sensor_item* v_sensor_item_create ();
void v_sensor_item_free (void *itemptr);

/* sensor_objfact.c */
int v_sensor_objfact_fab ();
int v_sensor_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_sensor_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* sensor_refcb.c */
int v_sensor_status_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);
int v_sensor_type_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);
