typedef struct v_fan_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *opstate;

} v_fan_item;

/* fan.c */
struct i_container_s* v_fan_cnt ();
int v_fan_enable (i_resource *self, int enclosure, char *oidindex_str);
int v_fan_disable (i_resource *self);

/* fan_item.c */
v_fan_item* v_fan_item_create ();
void v_fan_item_free (void *itemptr);

/* fan_objfact.c */
int v_fan_objfact_fab ();
int v_fan_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_fan_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

