typedef struct v_battery_item_s
{
  struct i_object_s *obj;

  /* Power */
  struct i_metric_s *present;

} v_battery_item;

/* battery.c */
struct i_container_s* v_battery_cnt ();
v_battery_item* v_battery_static_left_item ();
v_battery_item* v_battery_static_right_item ();
int v_battery_enable (i_resource *self);
int v_battery_disable (i_resource *self);

/* battery_item.c */
v_battery_item* v_battery_item_create ();
void v_battery_item_free (void *itemptr);

/* battery_cntform.c */
int v_battery_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* battery_objform.c */
int v_battery_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

