typedef struct v_temp_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *temperature;
  struct i_metric_s *opstate;

} v_temp_item;

/* temp.c */
struct i_container_s* v_temp_cnt ();
int v_temp_enable (i_resource *self, int enclosure, char *oidindex_str);
int v_temp_disable (i_resource *self);

/* temp_item.c */
v_temp_item* v_temp_item_create ();
void v_temp_item_free (void *itemptr);

/* temp_objfact.c */
int v_temp_objfact_fab ();
int v_temp_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_temp_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

