typedef struct v_cooling_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *type;

  struct i_metric_s *opstate;
  struct i_metric_s *speed;

} v_cooling_item;

/* cooling.c */
struct i_container_s* v_cooling_cnt ();
int v_cooling_enable (i_resource *self, int enclosure, char *oidindex_str);
int v_cooling_disable (i_resource *self);

/* cooling_item.c */
v_cooling_item* v_cooling_item_create ();
void v_cooling_item_free (void *itemptr);

/* cooling_objfact.c */
int v_cooling_objfact_fab ();
int v_cooling_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_cooling_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

