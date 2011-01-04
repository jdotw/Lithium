typedef struct v_sensor_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *status;
  struct i_metric_s *info;
  struct i_metric_s *message;

} v_sensor_item;

/* sensor.c */
struct i_container_s* v_sensor_enable (i_resource *self, char *unit_name, char *unit_desc, char *unit_oid_suffix);
v_sensor_item* v_sensor_item_create ();
void v_sensor_item_free (void *itemptr);

/* sensor_objfact.c */
int v_sensor_objfact_fab ();
int v_sensor_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_sensor_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

