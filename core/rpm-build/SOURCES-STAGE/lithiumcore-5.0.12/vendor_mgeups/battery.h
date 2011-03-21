typedef struct v_battery_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  /* Basic Data */
  struct i_metric_s *status;
  struct i_metric_s *timeonbatt;
  struct i_metric_s *lastreplaced;
  
  /* Advanced Data */
  struct i_metric_s *capacity;
  struct i_metric_s *temp;
  struct i_metric_s *runtime_remaining;
  struct i_metric_s *replace_indicator;
  struct i_metric_s *battpack_count;
  struct i_metric_s *battpack_badcount;

} v_battery_item;

/* battery.c */
struct i_container_s* v_battery_cnt ();
int v_battery_enable (i_resource *self);
int v_battery_disable (i_resource *self);

/* battery_item.c */
v_battery_item* v_battery_item_create ();
void v_battery_item_free (void *itemptr);

/* battery_objfact.c */
int v_battery_objfact_fab ();
int v_battery_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_battery_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* battery_cntform.c */
int v_battery_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* battery_objform.c */
int v_battery_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_battery_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

