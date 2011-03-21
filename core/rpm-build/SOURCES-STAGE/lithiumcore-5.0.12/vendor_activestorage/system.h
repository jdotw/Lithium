typedef struct v_system_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *status;
  struct i_metric_s *enclosure;
  struct i_metric_s *serial;

} v_system_item;

/* system.c */
struct i_container_s* v_system_cnt ();
int v_system_enable (i_resource *self);

/* system_item.c */
v_system_item* v_system_item_create ();
void v_system_item_free (void *itemptr);

/* system_objfact.c */
int v_system_objfact_fab ();
int v_system_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_system_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

